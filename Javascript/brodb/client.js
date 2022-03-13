var redis = require('redis'),
    client = redis.createClient(2666),
    RedisClient = require('redis').RedisClient;

client.set("c1","lasdfadf", redis.print);
client.get("c1", function(err,res) {
  console.log('err',err,'res',res);
  client.send_command('txn',['start']);
  client.send_command('cs',['open']);
  client.send_command('cs',['first']);
  var key = client.send_command('cs',['getkey','a'], function(err,res) {
              console.log('key',res);
            });

  client.send_command('cs',['close']);
  client.send_command('txn',['commit'], function(err,res) {
    client.end();
  });
});
