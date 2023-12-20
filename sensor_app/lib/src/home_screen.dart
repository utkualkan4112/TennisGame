import 'package:flutter/material.dart';
import 'package:sensor_app/src/socket.dart';
import 'package:sensors_plus/sensors_plus.dart';
import 'package:flutter_sensors/flutter_sensors.dart';

import 'second_page.dart';

class MyHomePage extends StatefulWidget {
  const MyHomePage({Key? key}) : super(key: key);

  @override
  State<MyHomePage> createState() => _MyHomePageState();

}

class _MyHomePageState extends State<MyHomePage> {

  final TextEditingController ipController = TextEditingController();
  final TextEditingController portController = TextEditingController();

  String connectionStatus = '';
  MySocket socket = MySocket();

  @override
  void dispose() {
    ipController.dispose();
    portController.dispose();
    super.dispose();
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
        title: const Text('Socket Connection'),
      ),
      body: Padding(
        padding: const EdgeInsets.all(20.0),
        child: Column(
          mainAxisAlignment: MainAxisAlignment.center,
          crossAxisAlignment: CrossAxisAlignment.stretch,
          children: <Widget>[
            TextField(
              controller: ipController,
              decoration: const InputDecoration(
                labelText: 'IP Address',
                border: OutlineInputBorder(),
              ),
            ),
            const SizedBox(height: 20),
            TextField(
              controller: portController,
              keyboardType: TextInputType.number,
              decoration: const InputDecoration(
                labelText: 'Port',
                border: OutlineInputBorder(),
              ),
            ),
            const SizedBox(height: 20),
            ElevatedButton(
              onPressed: _connectToServer,
              child: const Text('Connect'),
            ),
            const SizedBox(height: 20),
            Text(
              connectionStatus,
              textAlign: TextAlign.center,
              style: const TextStyle(
                color: Colors.red,
                fontWeight: FontWeight.bold,
              ),
            ),
          ],
        ),
      ),
    );
  }

  Future<void> _connectToServer() async {
    String ip = ipController.text.trim();
    int port = int.tryParse(portController.text.trim()) ?? 0;

    if (ip.isNotEmpty && port != 0) {
      try {
        await socket.connect(ip, port);
        _goToSecondPage();
      } catch (e) {
        setState(() {
          connectionStatus = 'Connection error: $e';
        });
      }
    } else {
      setState(() {
        connectionStatus = 'Please enter valid IP and port';
      });
    }
  }

  void _goToSecondPage () {
    Navigator.push(
      context,
      MaterialPageRoute(
        builder: (context) => SecondPage(socket: socket,),
      ),
    );
  }

}

