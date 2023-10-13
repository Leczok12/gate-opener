import 'dart:async';
import 'dart:convert';
import 'package:flutter/material.dart';
import 'package:getwidget/components/loader/gf_loader.dart';
import 'package:shared_preferences/shared_preferences.dart';
import 'package:web_socket_channel/web_socket_channel.dart';

class Home extends StatefulWidget {
  const Home({super.key});

  @override
  State<Home> createState() => _HomeState();
}

class _HomeState extends State<Home> {
  final Future<SharedPreferences> _prefs = SharedPreferences.getInstance();

  late WebSocketChannel ws;

  String _key = "";
  bool _connected = false;
  bool var1 = false;
  bool var2 = false;

  void _errorMsg(msg) {
    ScaffoldMessenger.of(context).showSnackBar(SnackBar(
      duration: const Duration(seconds: 5),
      content: Center(child: Text(msg)),
    ));
  }

  Future<void> _getKey() async {
    final SharedPreferences prefs = await _prefs;
    setState(() {
      _key = prefs.getString("key") ?? "";
    });
  }

  Future<void> _saveKey() async {
    final SharedPreferences prefs = await _prefs;
    prefs.setString("key", _key);
    _connectWs();
  }

  Future<void> _connectWs({bool close = true}) async {
    setState(() => _connected = false);
    if (close) {
      ws.sink.close();
    }
    ws = WebSocketChannel.connect(
        Uri.parse('ws://gate-opener-api.onrender.com/ws/$_key'));
    await ws.ready;
    ws.stream.listen((msg) {
      try {
        setState(() => var1 = json.decode(msg)["var1"]);
        setState(() => var2 = json.decode(msg)["var2"]);
      } catch (e) {
        _errorMsg("Bład strumienia");
      }
    }, onError: (msg) {
      _errorMsg(msg);
    });
    setState(() => _connected = true);
  }

  @override
  void dispose() {
    ws.sink.close();
    super.dispose();
  }

  @override
  void initState() {
    () async {
      await _getKey();
      _connectWs(close: false);
    }();

    super.initState();
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      body: _connected
          ? Center(
              child: Column(
                  mainAxisAlignment: MainAxisAlignment.center,
                  mainAxisSize: MainAxisSize.min,
                  children: [
                  Padding(
                    padding: const EdgeInsets.all(8.0),
                    child: SizedBox(
                        width: 180,
                        height: 140,
                        child: ElevatedButton(
                            style: const ButtonStyle(
                                shape: MaterialStatePropertyAll(
                                    RoundedRectangleBorder(
                                        borderRadius: BorderRadius.all(
                                            Radius.circular(15))))),
                            onPressed: var1
                                ? null
                                : () => {ws.sink.add('{"var1":true}')},
                            child: const Text(
                              "⚪",
                              style: TextStyle(fontSize: 25),
                            ))),
                  ),
                  Padding(
                    padding: const EdgeInsets.all(8.0),
                    child: SizedBox(
                        width: 180,
                        height: 140,
                        child: ElevatedButton(
                            style: const ButtonStyle(
                                shape: MaterialStatePropertyAll(
                                    RoundedRectangleBorder(
                                        borderRadius: BorderRadius.all(
                                            Radius.circular(15))))),
                            onPressed: var2
                                ? null
                                : () => {ws.sink.add('{"var2":true}')},
                            child: const Text(
                              "⚪ ⚪",
                              style: TextStyle(fontSize: 25),
                            ))),
                  ),
                ]))
          : const GFLoader(
              loaderstrokeWidth: 8,
              size: 80,
            ),
      floatingActionButton: IconButton(
        onPressed: () => showDialog(
            context: context,
            builder: (context) {
              String input = _key;

              return Dialog(
                child: Padding(
                  padding: const EdgeInsets.all(20.0),
                  child: Column(
                      mainAxisSize: MainAxisSize.min,
                      mainAxisAlignment: MainAxisAlignment.spaceBetween,
                      children: [
                        TextFormField(
                          initialValue: input,
                          maxLength: 64,
                          maxLines: 1,
                          onChanged: (value) => input = value,
                          decoration: const InputDecoration(
                            border: OutlineInputBorder(),
                            hintText: 'Klucz api',
                          ),
                        ),
                        Padding(
                          padding: const EdgeInsets.only(top: 20),
                          child: Wrap(
                            alignment: WrapAlignment.center,
                            runSpacing: 20,
                            spacing: 20,
                            children: [
                              SizedBox(
                                width: 150,
                                child: ElevatedButton(
                                    onPressed: () => ws.sink
                                        .add('{"var1":false,"var2":false}'),
                                    child: const Text("Reset Api")),
                              ),
                              SizedBox(
                                width: 150,
                                child: ElevatedButton(
                                    onPressed: () {
                                      if (input.length == 64) {
                                        setState(() {
                                          _key = input;
                                        });
                                        _saveKey();
                                        Navigator.pop(context);
                                      } else {
                                        ScaffoldMessenger.of(context)
                                            .showSnackBar(const SnackBar(
                                          duration: Duration(seconds: 3),
                                          content: Center(
                                              child: Text(
                                                  "Klucz musi mieć długość 64 znaków")),
                                        ));
                                      }
                                    },
                                    child: const Text("Zapisz")),
                              ),
                              SizedBox(
                                width: 150,
                                child: ElevatedButton(
                                    onPressed: () => Navigator.pop(context),
                                    child: const Text("Zamknij")),
                              )
                            ],
                          ),
                        )
                      ]),
                ),
              );
            }),
        icon: const Icon(Icons.settings_rounded),
      ),
    );
  }
}
