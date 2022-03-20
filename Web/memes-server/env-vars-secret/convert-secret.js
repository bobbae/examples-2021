const lineReader = require('line-reader');
var fs = require('fs');

lineReader.eachLine('env-vars.txt', function(line) {
    console.log(line);
    let items = line.split('=');
    console.log(items);
    let data = items[1];
    let buff = Buffer.from (data, 'utf8');
    let base64data = buff.toString('base64');
    fs.appendFile('secret-vars.txt', `${items[0]}: ${base64data} \n`, function(err) {
        if (err) {
            console.log('append failed');
        } 
    });
});
