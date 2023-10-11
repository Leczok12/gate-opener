import 'dart:async';
import 'dart:convert';
import 'package:flutter/material.dart';
import 'package:shared_preferences/shared_preferences.dart';
import 'package:http/http.dart' as http;

class Home extends StatefulWidget {
  const Home({super.key});

  @override
  State<Home> createState() => _HomeState();
}

class _HomeState extends State<Home> {
  final Future<SharedPreferences> _prefs = SharedPreferences.getInstance();

  String _key = "";
  bool var1 = false;
  bool var2 = false;

  void _errorApiMsg() {
    ScaffoldMessenger.of(context).showSnackBar(const SnackBar(
      duration: Duration(seconds: 3),
      content: Center(
          child: Text("Błąd podczas wysyłana lub przyjmowania wiadomości")),
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
  }

  Future<bool> _getRequest() async {
    if (_key == "") {
      return false;
    }

    final uri = Uri.parse("https://gate-opener-api.onrender.com/api/$_key");
    final response = await http.get(uri);

    if (response.statusCode == 200) {
      setState(() {
        var1 = json.decode(response.body)["var1"];
        var2 = json.decode(response.body)["var2"];
      });
      return true;
    }
    return false;
  }

  Future<bool> _postRequest(int n, bool v) async {
    if (_key == "") {
      return false;
    }
    Map<String, dynamic> req = n == 1 ? {'var1': v} : {'var2': v};
    Map<String, String> headers = {
      'Content-type': 'application/json',
    };
    final uri = Uri.parse("https://gate-opener-api.onrender.com/api/$_key");
    final response =
        await http.post(uri, headers: headers, body: json.encode(req));

    if (response.statusCode == 200) {
      setState(() {
        if (n == 1) {
          var1 = v;
        } else {
          var2 = v;
        }
      });
      return true;
    }
    return false;
  }

  late Timer apiRequest;

  @override
  void dispose() {
    apiRequest.cancel();
    super.dispose();
  }

  @override
  void initState() {
    _getKey();
    apiRequest = Timer.periodic(const Duration(seconds: 5), (timer) async {
      if (apiRequest.isActive && _key != "") {
        _getRequest().then((value) {
          if (!value) {
            _errorApiMsg();
          }
        });
      }
    });
    super.initState();
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      body: Center(
        child: Column(
            mainAxisAlignment: MainAxisAlignment.center,
            mainAxisSize: MainAxisSize.min,
            children: [
              Padding(
                padding: const EdgeInsets.all(8.0),
                child: SizedBox(
                    width: 120,
                    height: 90,
                    child: ElevatedButton(
                        style: const ButtonStyle(
                            shape: MaterialStatePropertyAll(
                                RoundedRectangleBorder(
                                    borderRadius: BorderRadius.all(
                                        Radius.circular(15))))),
                        onPressed: var1
                            ? null
                            : () => _postRequest(1, true).then((value) {
                                  if (!value) {
                                    _errorApiMsg();
                                  }
                                }),
                        child: const Text("⚪"))),
              ),
              Padding(
                padding: const EdgeInsets.all(8.0),
                child: SizedBox(
                    width: 120,
                    height: 90,
                    child: ElevatedButton(
                        style: const ButtonStyle(
                            shape: MaterialStatePropertyAll(
                                RoundedRectangleBorder(
                                    borderRadius: BorderRadius.all(
                                        Radius.circular(15))))),
                        onPressed: var2
                            ? null
                            : () => _postRequest(2, true).then((value) {
                                  if (!value) {
                                    _errorApiMsg();
                                  }
                                }),
                        child: const Text("⚪ ⚪"))),
              ),
            ]),
      ),
      floatingActionButton: IconButton(
        onPressed: () => showDialog(
            context: context,
            builder: (context) {
              String input = "";

              return Dialog(
                child: Padding(
                  padding: const EdgeInsets.all(20.0),
                  child: Column(
                      mainAxisSize: MainAxisSize.min,
                      mainAxisAlignment: MainAxisAlignment.spaceBetween,
                      children: [
                        TextField(
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
                                    onPressed: () {
                                      _postRequest(1, false).then((value) {
                                        if (!value) {
                                          _errorApiMsg();
                                        }
                                      });
                                      _postRequest(2, false).then((value) {
                                        if (!value) {
                                          _errorApiMsg();
                                        }
                                      });
                                    },
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
