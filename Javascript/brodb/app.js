var brodb = require('./brodb');
var express = require('express');

var app = express();

app.use(express.bodyParser());
app.use(express.json());

var dbs = {};
// curl  -H "Accept: application/json" -H "Content-type: application/json" -X POST -d '{ "name": "db1", "path": "./dbdir", "port": 2666, "maxDbs" : 10, "mapSize" : 16777216, "dupSort": true, "create": true}' http://127.0.0.1:5000/initdb
app.post("/initdb", function(req, res) {
    var conf = req.body;
    console.dir(conf);
    db = new brodb(conf);
    db.on('quit', function(err) {
        db.close();
        console.log(err);
        db = null;
    });
    db.on('error', function(err) {
        console.log(err);
    });
    dbs[conf.name] = db;
    res.send("ok");
});

// curl  -H "Accept: application/json" -H "Content-type: application/json" -X POST -d '{ "name": "db1"}' http://127.0.0.1:5000/closedb
app.post("/closedb", function(req, res) {
    db.close(dbs[req.body.name]);
    res.send("ok");
});

app.post("/set", function(req, res) {
    db.set(req.body.key, req.body.value);
    res.send("ok");
});

//curl  -H "Accept: application/json" -H "Content-type: application/json" -X POST -d '{ "key": "a"}' http://127.0.0.1:5000/get
app.post("/get", function(req, res) {
    var val = db.get(req.body.key);
    res.send('{ "value":' + val + '}');
});

app.post("/del", function(req, res) {
    db.del(req.body.key);
    res.send("ok");
});

app.post("/closedb", function(req, res) {
    db.close(dbs[req.body.name]);
    res.send("ok");
});

var port = process.env.PORT || 5000;
app.listen(port, function() {
    console.log("Listening on " + port);
});
