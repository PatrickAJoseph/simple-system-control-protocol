const express = require("express");
const { sendCommand } = require("../services/python.service");

const router = express.Router();

router.post("/set", async (req, res) => {
  console.log("Call has come to post request", req.body);

  try {
    const result = await sendCommand(req.body);
    res.json({ success: true, result });
  } catch (err) {
    res.status(500).json({ success: false, error: err.message });
  }
});

router.get("/get/:register/:parameter", async (req, res) => {
  const register = req.params.register;
  const parameter = req.params.parameter;
  console.log("Call has come to get request", "param: ", parameter, " reg: ", register);
  try {
    const result = await sendCommand({type:"get", register, parameter});
    res.json({ success: true, result });
  } catch (err) {
    res.status(500).json({ success: false, error: err.message });
  }
});


module.exports = router;
