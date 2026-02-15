const express = require("express");
const cors = require("cors");
const commandRoutes = require("./routes/command.routes");

const app = express();
app.use(express.json());
app.use(cors());

app.use("/api", commandRoutes);
app.listen(3000, () => {
  console.log("Node API running on port 3000");
});
