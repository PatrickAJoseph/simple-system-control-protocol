const express = require("express");
const { sendCommand } = require("../services/python.service");

const router = express.Router();

router.post("/", async (req, res) => {
  console.log("Call has come to post request");
  
  try {
    const result = await sendCommand(req.body);
    res.json({ success: true, result });
  } catch (err) {
    res.status(500).json({ success: false, error: err.message });
  }
});

module.exports = router;
