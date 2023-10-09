import { NextApiRequest, NextApiResponse } from "next";

type DataType = { id: String; var1: boolean; var2: boolean };

const data: DataType[] = [];

const handler = (_req: NextApiRequest, res: NextApiResponse) => {
  try {
    if (_req.query.id.length !== 64) {
      res.status(500).json({ statusCode: 404, message: "bad id" });
      return;
    }

    let index: number = -1;
    data.forEach((object, i) => {
      if (object.id == _req.query.id) {
        index = i;
      }
    });

    if (index === -1) {
      data.push({ id: _req.query.id as string, var1: false, var2: false });
      index = data.length - 1;
      console.log(`Created new record with id: [${_req.query.id}]`);
    }

    if (data[index] !== undefined) {
      if (_req.method === "POST") {
        const body = _req.body;
        if (body.var1 != undefined && typeof body.var1 == "boolean") {
          data[index].var1 = body.var1;
        }
        if (body.var2 != undefined && typeof body.var2 == "boolean") {
          data[index].var2 = body.var2;
        }
      }
      res.status(200).send(data[index]);
    } else {
      res
        .status(500)
        .json({ statusCode: 500, message: "failed with added new record" });
    }
  } catch (err: any) {
    res.status(500).json({ statusCode: 500, message: err.message });
  }
};

export default handler;
