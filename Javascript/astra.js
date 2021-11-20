//https://github.com/DataStax-Examples/quickstart-nodejs/blob/master/quickstart-complete.js
const { Client } = require("cassandra-driver");

async function run() {
    console.log(`home => ${process.env.HOME} user => ${process.env.ASTRA_DB_USERNAME} pass => ${process.env.ASTRA_DB_PASSWORD}`)
    
  const client = new Client({
    cloud: {
      secureConnectBundle: `${process.env.HOME}/secure-connect-astratest1.zip`,
    },
    credentials: { username: `${process.env.ASTRA_DB_USERNAME}`, password: `${process.env.ASTRA_DB_PASSWORD}` },
  });

    
    await client.connect();

    const queryString = `INSERT INTO ${process.env.ASTRA_DB_KEYSPACE}.${process.env.ASTRA_DB_TABLE} (sessionid, toptexttext, bottomtext, template, votes) VALUES (?, ?, ?, ?, ?) `;
    const params =  ['aag:toptext:bottomtext:12:1', 'toptext', 'bottomtext', 'aaa', 12];

    await client.execute( queryString, params, {prepare:true})
    //https://docs.datastax.com/en/developer/nodejs-driver/3.1/features/datatypes/numerical/

    //const rs = await client.execute(`SELECT * FROM ${process.env.ASTRA_DB_KEYSPACE}.${process.env.ASTRA_DB_ID}");
    const rs = await client.execute(`SELECT * FROM ${process.env.ASTRA_DB_KEYSPACE}.${process.env.ASTRA_DB_TABLE}`);
  console.log(`Your cluster returned ${rs.rowLength} row(s), ${rs.rows[0]['sessionid']}`);

  await client.shutdown();
}

// Run the async function
run();
