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

  // Execute a query
  const rs = await client.execute("SELECT * FROM system.local");
  console.log(`Your cluster returned ${rs.rowLength} row(s)`);

  await client.shutdown();
}

// Run the async function
run();
