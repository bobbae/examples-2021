var dgram = require('dgram');
var argv = require('minimist')(process.argv.slice(2));
 
var IPMIPing = function() {
    var self = this;
    self.sockClosed = false;
};

IPMIPing.prototype.ping = function(ip, cb)  {
    var self = this;
    self.sock = dgram.createSocket('udp4');
    self.sock.on('error', function(err) {
        cb('error', err);
    });
    self.sock.on('message', function(data,peer) {
        cb('message', {data:data, peer:peer});
        // rmcp.ver	= 0x06		// RMCP Version 1.0
        // rmcp.__res	= 0x00		// RESERVED
        // rmcp.seq	= 0xff		// no RMCP ACK
        // rmcp.class	= 0x06		// RMCP_CLASS_ASF
        // asf.iana	= 0x000011be	// ASF_RMCP_IANA
        // asf.type	= 0x40		// ASF_TYPE_PONG
        // asf.tag	= ?		// asf.tag from rmcp-ping
        // asf.__res	= 0x00		// RESERVED
        // asf.len	= 0x10		// 16 bytes
        // asf.data[3:0]= 0x000011be	// IANA# = RMCP_ASF_IANA if no OEM
        // asf.data[7:4]= 0x00000000	// OEM-defined (not for IPMI)
        // asf.data[8]	= 0x81		// supported entities
        // 				// [7]=IPMI [6:4]=RES [3:0]=ASF_1.0
        // asf.data[9]	= 0x00		// supported interactions (reserved)
        // asf.data[f:a]= 0x000000000000
        if (data[0] == 6 && data[1] == 0 && data[2] == 0xff && data[3] == 6 && data[8] == 0x40) {
            cb('done', {ip: ip, bmcExists: true});
        } else {
            cb('error', {error: 'unexpected input:',data: data, peer:peer});
            cb('done', {ip:ip, bmcExists: false});
        }
    });
    if (self.sockClosed) {
        cb('error', {error: 'socket is closed'});
        return;
    }
    // rmcp.ver	= 0x06		// RMCP Version 1.0
    // rmcp.__res	= 0x00		// RESERVED
    // rmcp.seq	= 0xff		// no RMCP ACK
    // rmcp.class	= 0x06		// RMCP_CLASS_ASF
    // asf.iana	= 0x000011be	// ASF_RMCP_IANA
    // asf.type	= 0x80		// ASF_TYPE_PING
    // asf.tag	= ?		// ASF sequence number
    // asf.__res	= 0x00		// RESERVED
    // asf.len	= 0x00
    var pkt = new Buffer(12);
    pkt[0] = 6;
    pkt[1] = 0;
    pkt[2] = 0xff;
    pkt[3] = 6;
    pkt.writeUInt32LE(0x11be,4);
    pkt[8] = 0x80;
    pkt[9] = 0;
    pkt[10] = 0;
    pkt[11] = 0;
    self.sock.send(pkt, 0, pkt.length, 623, ip, function(err,bytes) {
        if (err) {
            cb('error', {error:err, bytes:bytes});
        } else {
            //cb('sent', {error:err, bytes:bytes});
        }
    });
    var makeTO = function() {
        return function() {
            //cb('timeout', {ip:ip });
        };
    };
    setTimeout(makeTO(), 1000000); // XXX
};

IPMIPing.prototype.finish = function(ip, cb)  {
    var self = this;
    self.sock.close();
    self.sockClosed = true;
};

if (require.main == module) {
    var IC = new IPMIPing();
    argv._.forEach(function(ipa) {
        IC.ping(ipa, function(status, content) {
            console.log(status,content);
        });
    });
}

module.exports = IPMIPing;
