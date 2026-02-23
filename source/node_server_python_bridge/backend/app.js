const express = require("express");
const cors = require("cors");
const commandRoutes = require("./routes/command.routes");

const fs = require("fs");
const path = require("path");
const os = require("os");

const app = express();
app.use(express.json());
app.use(cors());

app.use("/api", commandRoutes);
app.listen(3000, () => {
  console.log("Node API running on port 3000");
});

app.use(express.json());

app.post("/api/save-config", (req, res) => {
  try {
    const { filename, content } = req.body;

    // Use OS temp directory
    const tempDir = os.tmpdir();

    const filePath = path.join(tempDir, filename);

    fs.writeFileSync(filePath, content, "utf8");

    res.json({
      status: "ok",
      path: filePath
    });

  } catch (err) {
    console.error(err);
    res.status(500).json({ error: "File save failed" });
  }
});