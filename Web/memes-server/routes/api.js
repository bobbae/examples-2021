var express = require('express');
const { Client } = require("cassandra-driver");
var path = require('path');
var fetch = require('cross-fetch');
var Minio = require('minio')

var router = express.Router();

/* GET home page. */
router.get('/', function (req, res, next) {
    res.render('index', { title: 'Express' });
});

router.get('/env', function (req, res, next) {
    console.log(process.env);
    res.send({ status: 'ok', 'env': process.env })
});

router.get('/list', function (req, res, next) {
    rs = get_from_cassandra(res);
});

router.post('/get', (request, response) => {
    get_cassandra_client();
    if (request.body === null ||
        request.body === undefined ){
        res.send({ status: 'missing body' });
    }
    if (request.body['name'] !== null &&
        request.body['name'] !== undefined) {
        get_from_cassandra_by_name(request.body['name'], response);
    }
});

router.post('/save', (request, response) => {
    save_to_cassandra(request.body);
    response.status(204).send();
});

router.post('/backup', (request, response) => {
    //var endPoint = `${process.env.MINIO}`;
    var endPoint = '192.168.49.2';
    console.log('/backup endPoint', endPoint );
    var minioClient = new Minio.Client({
        endPoint: endPoint, //XXX
        port: 9000,
        useSSL: false,
        accessKey: 'minio',
        secretKey: 'minio123' 
        /* endPoint: 'play.min.io',
        port: 9000,
        useSSL: true,
        accessKey: 'Q3AM3UQ867SPQQA43P2F',
        secretKey: 'zuf+tfteSlswRu7BJ86wekitnifILbZam1KYY3TG' */
    });
    minioClient.putObject('memes', request.body.name, request.body.image, 
        function(e) {  if (e) return console.log('/api/backup',e);    }
    );
   
    response.status(204).send();
});


var cassandra_client = null;

async function get_cassandra_client() {

    if (cassandra_client !== null && cassandra_client !== undefined) {
        console.log('get_cassandra_client, already connected')
        return cassandra_client;
    }

    var bundle = `${process.env.ASTRA_SECURE_CONNECT_BUNDLE}`
    console.log(`get_cassandra_client using connect bundle ${bundle}`);

    cassandra_client = new Client({
        cloud: {
            secureConnectBundle: bundle,
        },
        credentials: { username: `${process.env.ASTRA_DB_USERNAME}`, password: `${process.env.ASTRA_DB_PASSWORD}` },
    });
    console.log('get_cassandra_client, connecting...')
    await cassandra_client.connect();
    
}

async function get_from_cassandra_by_name(name, res) {
    console.log('get_from_cassandra_by_name')
    get_cassandra_client();

    var db = `${process.env.ASTRA_DB_KEYSPACE}.${process.env.ASTRA_DB_TABLE}`;
    var response = await cassandra_client.execute(
        `SELECT * FROM ${db} WHERE name = ?  ALLOW FILTERING `, [name]);
    //console.log(response);
    res.send({ status: 'ok', 'rows': response.rows });
}

async function save_to_cassandra(body) {
    console.log('save_to_cassandra')
    get_cassandra_client();
    var db = `${process.env.ASTRA_DB_KEYSPACE}.${process.env.ASTRA_DB_TABLE}`;

    const query = 
      `INSERT INTO ${db} (name, toptext, bottomtext, votes, uuid) VALUES (?,  ?, ?, ?, ?) `;
    const params = 
       [ body['name'],  body['toptext'], body['bottomtext'], body['votes'],body['uuid'] ];
    //console.log('save', params);
    await cassandra_client.execute(query, params, { prepare: true })
}

async function shutdown_cassandra_client() {
    await cassandra_client.shutdown();
}

async function get_from_cassandra(res) {
    console.log('get_from_cassandra')
    get_cassandra_client();
    var db = `${process.env.ASTRA_DB_KEYSPACE}.${process.env.ASTRA_DB_TABLE}`;
    var rs = await cassandra_client.execute(
        `SELECT * FROM ${db} PER PARTITION LIMIT 1`);
    for (i = 0; i < rs.rowLength; i++) {
        row = rs.rows[i];
        //console.log(row, row['key']);
    }
    res.send({ status: 'ok', 'rows': rs })
}



module.exports = [router];
