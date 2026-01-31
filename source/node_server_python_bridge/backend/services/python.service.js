const net = require("net");

const HOST = "127.0.0.1";
const PORT = 5000;

exports.sendCommand = (payload) => {
  return new Promise((resolve, reject) => {
    const client = new net.Socket();

    client.connect(PORT, HOST, () => {
      client.write(JSON.stringify(payload));
    });

    client.on("data", (data) => {
      resolve(JSON.parse(data.toString()));
      client.destroy();
    });

    client.on("error", reject);
  });
};
