const express = require("express");
const app = express();
const port = 3000;

app.get("/", (req, res) => {
  res.send("<h1>Siemano</h1>");
});

app.get("/api/:id", (req, res) => {
  res.send(req.params.id);
});

app.listen(port, () => {
  console.log(`Started on port : ${port}`);
});
