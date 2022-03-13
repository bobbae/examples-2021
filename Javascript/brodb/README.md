BrODB
=====

http://www.urbandictionary.com/define.php?term=brodb

Toy to play around with LMDB

to run it as a server:

```
    node brodb.js
```

Test using redis-cli:

```
    $ redis-cli -p 2666
    redis 127.0.0.1:2666> txn start
    OK
    redis 127.0.0.1:2666> cs start
    (error) error, bad cursor command
    redis 127.0.0.1:2666> cs open
    OK
    redis 127.0.0.1:2666> cs first
    OK
    redis 127.0.0.1:2666> cs getAll
    1) "111"
    2) "123"
    3) "12343"
    4) "388"
    5) "3"
    6) "8s"
    7) "8"
    redis 127.0.0.1:2666> cs close
    OK
    redis 127.0.0.1:2666> txn commit
    OK
    redis 127.0.0.1:2666> set aaa 777
    OK
    redis 127.0.0.1:2666> get aaa
    "777"
    redis 127.0.0.1:2666> del aaa
    OK deleted
    redis 127.0.0.1:2666> get aaa
    (error) not found
    redis 127.0.0.1:2666> 
```

or use client.js example to use node.js client api for redis and extend it to use extra commands 'txn' and 'cs':

```
    node client.js 
    Reply: OK
    err null res lasdfadf
    key a
```

alternatively, to run a http rest api server:

```
    node app.js
```

and use curl to post rest request and test:

```
    curl  -H "Accept: application/json" -H "Content-type: application/json" -X POST -d '{ "name": "db1", "path": "./dbdir", "port": 2666, "maxDbs" : 10, "mapSize" : 16777216, "dupSort": true, "create": true}' http://127.0.0.1:5000/initdb

    curl  -H "Accept: application/json" -H "Content-type: application/json" -X POST -d '{ "key": "a"}' http://127.0.0.1:5000/get

    curl  -H "Accept: application/json" -H "Content-type: application/json" -X POST -d '{ "name": "db1"}' http://127.0.0.1:5000/closedb

```
