var app = require("express")();
var http = require("http").Server(app);
var bodyParser = require("body-parser");
var mongoose = require("mongoose");
var cors = require("cors");

//mongoose.connect('mongodb://localhost/testdb');
config = {
  DB: "mongodb://localhost:27017/tryone?replicaSet=rs",
};
mongoose.connect(config.DB, { useNewUrlParser: true }).then(
  () => {
    console.log("Database is connected");
  },
  (err) => {
    console.log("Can not connect to the database" + err);
  }
);

const db = mongoose.connection;

db.on("error", console.error.bind(console, "Connection Error:"));

var io = require("socket.io");
var server = require("http").createServer(app);

var socket = io(server);

socket.on("connection", (client) => {
  console.log("user connected");
  socket.on("disconnect", () => {
    console.log("user disconnected");
  });
});

db.once("open", () => {
  server.listen(90, () => {
    console.log("listening on *:90");
  });

  const taskCollection = db.collection("tryonecol");
  const changeStream = taskCollection.watch();

  changeStream.on("change", (change) => {
    // console.log(change.updateDescription.updatedFields);
    //console.log(keys(change.updateDescription.updatedFields));
    if (change.operationType === "update") {
      socket.emit("server", change.updateDescription);
    }
  });
});

app.get("/", function (req, res) {
  res.sendFile(__dirname + "/graph.html");
});
app.get("/socket.io.js", function (req, res) {
  res.sendFile(__dirname + "/socket.io.js");
});
app.get("/jquery-3.5.1.js", function (req, res) {
  res.sendFile(__dirname + "/jquery-3.5.1.js");
});
app.get("/canvasjs.min.js", function (req, res) {
  res.sendFile(__dirname + "/canvasjs.min.js");
});

app.use(bodyParser.urlencoded({ extended: false }));
app.use(bodyParser.json());
//npm install async
const queue = require("async/queue");
const q = queue(function (task, cb) {
  // console.log(task);
  dbObject.updateOne({ name: task.docname }, { $push: { [task.field]: { $each: task.processed_data } } }, (err) => {
    if (err) console.log("DB error:", err);
    cb();
  });
  // console.log('done')
}, 1);
//Var for POST
var dbObject = db.collection("tryonecol");
var Data_From_NodeMCU;
var S_data_server;
var T_data_server;
var nDate;
var prefixes = ["P", "E", "T", "S", "B"]; // PAT,ECG,Temp,SpO2,BioZ
var loopbound;
var data;
var E_data_server;
var B_data_server;
var docname;
//POST to save out data in DB
app.post("/saveData", function (req, res) {
  console.log("Data", req.body);
  Data_From_NodeMCU = req.body.hello;
  S_data_server = req.body.SData;
  B_data_server = req.body.BData;
  T_data_server = req.body.TData;
  E_data_server = req.body.EData;
  //   nDate = Date.now();

  //  console.log(nDate);

  //["P","E","T","S","B"]
  data = [Data_From_NodeMCU, E_data_server, T_data_server, S_data_server, B_data_server];

  //////////////Sending Data\\\\\\\\\\\\\\\\

  loopbound = prefixes.length;
  for (dummy = 0; dummy < loopbound; dummy++) {
    var dummy;
    docname = "AKILA" + prefixes[dummy];
    if (data[dummy].length != 0) {
      var field = prefixes[dummy] + ".1";
      var time = prefixes[dummy] + ".0";
      const processed_data = data[dummy];
      //pusing to database one by one
      q.push({ processed_data: processed_data, docname: docname, field: field });
      // dbObject.updateOne(
      //   {name: docname},
      //   {'$push': {[field] :{$each: data[dummy]} }},
      //   function (err) {
      //     if (err)
      //     {
      //       console.log("DB error:");
      //       console.log(err);
      //     }})
      //     ;
      ////////////////
      // dbObject.updateOne(
      //   {name: docname},
      //   //{'$push': {[time] :{$each: time_cluster} }},
      //   {'$push': {[time] :nDate }},
      //   function (err) {
      //     if (err)
      //     {
      //       console.log("DB error:");
      //       console.log(err);
      //     }});
    }
  }
  res.send("a");
});