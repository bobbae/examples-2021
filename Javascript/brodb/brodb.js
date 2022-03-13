var mkdirp = require('mkdirp');
var rp = require('redis-protocol');
var lmdb = require('node-lmdb');
var argv = require('minimist')(process.argv.slice(2));
var EventEmitter = require('events').EventEmitter;
var util = require('util');

// use the wonderful lmdb 


var BroDB = function(conf) {
    var encoder;
    var self = this;
    self.env = new lmdb.Env();
    self.conf = conf;
    mkdirp(conf.path, function(err) {
        if (!!err)
            console.log(err);
    });
    self.env.open(conf);
    console.log('conf',conf);
    self.dbi = self.env.openDbi(conf);
    self.txn = null;
    self.cursor = null;
    var key;

    // provide a 'pseudo' redis protocol, it will
    // support some redis commands and clients

    self.server = rp.createServer(function(command) {
        if (!command) {
            self.emit('error','null command');
            return;
        }
        //console.log('>>>', command);
        self.encoder = this;
        switch(command[0]) {
            case 'quit':
                self.encoder.singleline('OK bye');
                self.close();
                self.emit('quit', 'user requested quit');
                break;
            case 'info':
                self.encoder.encode(JSON.stringify(conf));
                break;
            case 'txn':         // custom command
            case 'transaction':
                switch(command[1]) {
                    case 'begin':
                    case 'start':
                        self.txn = self.env.beginTxn();
                        self.encoder.singleline('OK');
                        break;
                    case 'commit':
                        self.txn.commit();
                        self.txn = null;
                        self.encoder.singleline('OK');
                        break;
                    case 'abort':
                        self.txn.abort();
                        self.txn = null;
                        self.encoder.singleline('OK');
                        break;
                    default:
                        self.emit('error','bad transaction command', command[1]);
                        self.encoder.error('error, bad transaction command');
                }
                break;
            case 'cs':          // custom command
            case 'cursor':      // for iterating the always ordered keys
                if (!self.txn) {
                    self.emit('error', 'cursor open when txn is null');
                    self.encoder.error('error, no active transaction');
                    return;
                }
                switch(command[1]) {
                    case 'open':
                    case 'init':
                        self.cursor = new lmdb.Cursor(self.txn, self.dbi);
                        if (!self.cursor) {
                            self.emit('error','cannot get a cursor');
                            self.encoder.error('error, cannot get a cursor');
                            return;
                        }
                        self.encoder.singleline('OK');
                        break;
                    case 'close':
                    case 'delete':
                        self.cursor.close();
                        self.cursor = null;
                        self.encoder.singleline('OK');
                        break;
                    case 'first':
                        key = self.cursor.goToFirst();
                        if (key) {
                            self.encoder.singleline('OK');
                        } else {
                            self.encoder.error('error, cannot go to first');
                        }
                        break;
                    case 'next':
                        key = self.cursor.goToNext();
                        if (key) {
                            self.encoder.singleline('OK');
                        } else {
                            self.encoder.error('error, cannot go to next');
                        }
                        break;
                    case 'prev':
                        key = self.cursor.goToPrev();
                        if (key) {
                            self.encoder.singleline('OK');
                        } else {
                            self.encoder.error('error, cannot go to prev');
                        }
                        break;
                    case 'gorange':
                        if (!command[2]) {
                            self.encoder.error('error, range arg required');
                        } else {
                            key = self.cursor.goToRange(command[2]);
                            if (key) {
                                self.encoder.singleline('OK');
                            } else {
                                self.encoder.error('error, cannot go to range');
                            }
                        }
                        break;
                    case 'gokey':
                        if (!command[2]) {
                            self.encoder.error('error, key arg required');
                        } else {
                            key = self.cursor.goToKey(command[2]);
                            if (key) {
                                self.encoder.singleline('OK');
                            } else {
                                self.encoder.error('error, cannot go to that key');
                            }
                        }
                        break;
                    case 'getkey':
                        self.encoder.encode(key = self.cursor.getCurrentString());
                        break;
                    // so the lmdb keys are always ordered in btree on disk
                    // making it easy to iterate like arrays. a good data
                    // set may be array of json data
                    case 'getAll':
                        var result = [];
                        for (var key = self.cursor.getCurrentString(); key; key = self.cursor.goToNext()) {
                            result.push(self.txn.getString(self.dbi,key));
                        }
                        self.encoder.encode(result);
                        break;
                    default:
                        self.emit('error','bad cursor command', command[1]);
                        self.encoder.error('error, bad cursor command');
                        break;
                }
                break;
            default:
                self.lmdbCmd(self.encoder, command[0],command.slice(1));
        }
    });

    self.server.listen(conf.port, function() {
        console.log('brodb started at port',conf.port);
    });
};

util.inherits(BroDB, EventEmitter);

BroDB.prototype.close = function() {
    var self = this;
    self.dbi.close();
    self.env.close();
    self.server.close();
    console.log("db", self.conf.name, "closed");
};

BroDB.prototype.set = function(key, val) {
    var self = this;
    txn = self.env.beginTxn();
    txn.putString(self.dbi, key, val);
    txn.commit();
};

BroDB.prototype.get = function( key) {
    var self = this;
    txn = self.env.beginTxn();
    var res = txn.getString(self.dbi, key);
    txn.commit();
    return(res);
};

BroDB.prototype.del = function(key) {
    var self = this;
    txn = self.env.beginTxn();
    txn.del(self.dbi, key);
    txn.commit();
};

BroDB.prototype.lmdbCmd = function(encoder, cmd, args) {
    var self = this;
    var txn;
    // strategy is to use 'string' for everything.
    // that way JSON.stringify in/out of value for key
    // and then use jsonpath or js to search & process
    switch(cmd){
        case 'set':
            self.set(args[0], args[1]);
            encoder.singleline('OK');
            break;
        case 'get':
            var data = self.get(args[0]);
            if (data)
                encoder.encode(data);
            else
                encoder.error('not found');
            break;
        case 'del':
            self.del(args[0]);
            encoder.singleline('OK deleted');
            break;
        default:
            encoder.error("error, bad command");
    }
}

if (require.main == module) {
    // you can have multiple named dbs inside a db, like 'db1', 'db2',...
    var db1 = new BroDB({
        name: 'db1',
        path: !!argv.d ? argv.d : './dbdir',
        port: !!argv.p ? argv.p : 2666,
        maxDbs: !!argv.n ? argv.n : 10,
        mapSize: !!argv.s ? argv.s : 16 * 1024 * 1024,
        create: true,
        dupSort: true
    });

    // and you can have multiple instances of brodb in different
    // directories './dbdir1', './dbdir2', ...
    // since all data is transactionally written to the disk via mmap
    // and data length per key can be large (unlike leveldb and redis)
    // without slowing down, it is possible to store a lot of data,
    // much more than available RAM, and cluster these things together.

    db1.on('quit', function(err) {
        db1.close();
        console.log(err);
    });
    db1.on('error', function(err) {
        console.log(err);
    });
}

module.exports = BroDB;
