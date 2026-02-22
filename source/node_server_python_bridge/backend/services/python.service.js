const net = require("net");

const HOST = "127.0.0.1";
const PORT = 5000;

exports.sendCommand = (payload) => {
  return new Promise((resolve, reject) => {
    const client = new net.Socket();

    client.connect(PORT, HOST, () => {
      console.log("JSON payload to be sent to Python script:" + JSON.stringify(payload))
      client.write(JSON.stringify(payload));
    });

    client.on("data", (data) => {
      console.log("JSON payload received from Python script:" + data.toString())
      resolve(JSON.parse(data.toString()));
      client.destroy();
    });

    client.on("error", reject);
  });
};
