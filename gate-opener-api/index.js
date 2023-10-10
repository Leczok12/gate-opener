const express = require("express");
const bodyParser = require("body-parser");
const app = express();
const port = 3000;

app.use(bodyParser.json());
app.use(
  bodyParser.urlencoded({
    extended: true,
  })
);

app.use((req, res, next) => {
  res.header("Access-Control-Allow-Origin", "*");
  res.header(
    "Access-Control-Allow-Headers",
    "Origin, X-Requested-With, Content-Type, Accept"
  );
  next();
});

app.listen(port, () => {
  console.log(`Started on port : ${port}`);
});

app.get("/", (req, res) => {
  res.send(`
  <html>
    <body style="background-color: #000; padding: 0; margin: 0; font-family: Consolas, Arial; display: flex;">
        <div style="height: 100vh; width: 100vw; display: flex; align-items: center; justify-content: center; color:#fff; flex-direction: column;">
            <h1>Siemano</h1>
            <p>generate a 64 chars string from <a href="https://miniwebtool.com/pl/random-string-generator/" target="_blank">there</a> and go to api/[string]</p>
        </div>
    
    </body>
  </html>
  `);
});

const data = [];

function findIndex(id) {
  let index = -1;
  data.forEach((e, i) => {
    if (e.id === id) {
      index = i;
    }
  });
  return index;
}

function createRecord(id) {
  data.push({ id: id, var1: false, var2: false });
  console.log(`Created new record with id [${id}]`);
  return data.length - 1;
}

app.get("/api/:id", (req, res) => {
  if (req.params.id.length !== 64) {
    res.status(404).json({
      errorCode: 404,
      message: "bad key",
    });
    return;
  }
  const index = findIndex(req.params.id);

  if (index === -1) {
    res.status(200).json(data[createRecord(req.params.id)]);
  } else {
    res.status(200).json(data[index]);
  }
});

app.post("/api/:id", (req, res) => {
  if (req.params.id.length !== 64) {
    res.status(404).json({
      errorCode: 404,
      message: "bad key",
    });
    return;
  }

  let index = findIndex(req.params.id);

  if (index === -1) {
    index = createRecord(req.params.id);
  }

  if (typeof req.body.var1 == "boolean") {
    data[index].var1 = req.body.var1;
  }
  if (typeof req.body.var2 == "boolean") {
    data[index].var2 = req.body.var2;
  }

  //   res.status(200).json(req);
  res.status(200).json(data[index]);
});
