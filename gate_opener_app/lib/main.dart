import 'package:flutter/material.dart';
import 'package:gate_opener_app/views/home.dart';

void main() {
  runApp(const MyApp());
}

class MyApp extends StatelessWidget {
  const MyApp({super.key});

  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      title: 'gate opener app',
      home: const Home(),
      themeMode: ThemeMode.dark,
      theme: ThemeData(
        colorScheme: const ColorScheme(
            background: Color.fromARGB(255, 29, 29, 29),
            brightness: Brightness.dark,
            primary: Color.fromARGB(255, 255, 255, 255),
            onPrimary: Color.fromARGB(255, 0, 0, 0),
            secondary: Color.fromARGB(255, 209, 10, 10),
            onSecondary: Color.fromARGB(255, 241, 3, 3),
            error: Color.fromARGB(255, 35, 35, 35),
            onError: Color.fromARGB(255, 35, 35, 35),
            onBackground: Color.fromARGB(255, 35, 35, 35),
            surface: Color.fromARGB(255, 36, 36, 36),
            onSurface: Color.fromARGB(255, 255, 255, 255)),
        useMaterial3: true,
      ),
    );
  }
}
