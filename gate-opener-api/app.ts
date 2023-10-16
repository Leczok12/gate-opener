import express, { Request, Response } from "express";
import expressWs from "express-ws";
import { WebSocket as WebSocket_ } from "ws";
import dotenv from "dotenv";

dotenv.config();

const _expressWs = expressWs(express());
const app = _expressWs.app;
const port = process.env.PORT;

app.use((req, res, next) => {
  res.header("Access-Control-Allow-Origin", "*");
  res.header(
    "Access-Control-Allow-Headers",
    "Origin, X-Requested-With, Content-Type, Accept"
  );
  next();
});

app.get("/", (req: Request, res: Response) => {
  console.log(req.get("host"));
  res.send(`
  <html>
    <head>
        <title>gate opener api</title>
    </head>
    <body style="background-color: #000; padding: 0; margin: 0; font-family: Consolas, Arial; display: flex;">
        <div style="height: 100vh; width: 100vw; display: flex; align-items: center; justify-content: center; color:#fff; flex-direction: column;">
            <h1>GATE-OPENER-API</h1>
            <p>generate a 64 chars string from <a href="https://miniwebtool.com/pl/random-string-generator/" target="_blank">there</a> and your socket is available on ws://${req.get(
              "host"
            )}/ws/[string]</p>
        </div>
    
    </body>
  </html>
  `);
});

const data: {
  id: string;
  clients: WebSocket_[];
  data: { var1: boolean; var2: boolean };
}[] = [];

function getIndex(id: string): number {
  const i1 = data.findIndex((e) => {
    if (e.id === id) {
      return true;
    }
  });

  if (i1 != -1) return i1;

  data.push({ id: id, clients: [], data: { var1: false, var2: false } });
  console.log(`Created new record with id => ${id}`);
  const i2 = data.findIndex((e) => {
    if (e.id === id) {
      return true;
    }
  });
  return i2;
}

app.ws("/ws/:id", (ws, req) => {
  const id = req.params.id;
  if (id.length === 64) {
    const index = getIndex(id);

    data[index].clients.push(ws);
    ws.send(JSON.stringify(data[index].data));

    ws.on("message", (msg) => {
      try {
        const jMsg = JSON.parse(msg.toString());
        if (typeof jMsg.var1 === "boolean" || typeof jMsg.var2 === "boolean") {
          if (typeof jMsg.var1 === "boolean") data[index].data.var1 = jMsg.var1;
          if (typeof jMsg.var2 === "boolean") data[index].data.var2 = jMsg.var2;
          data[index].clients.forEach((e) => {
            e.send(JSON.stringify(data[index].data));
          });
        }
      } catch (error) {}
    });

    ws.on("close", () => {
      data[index].clients.splice(
        data[index].clients.findIndex((e) => e === ws ?? true),
        1
      );
    });
  } else {
    ws.close();
  }
});

app.listen(port, () => {
  console.log(`App started on port => ${port}`);
});
