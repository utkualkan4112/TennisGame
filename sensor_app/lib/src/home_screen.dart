import 'package:flutter/material.dart';
import 'package:sensor_app/src/socket.dart';
import 'package:sensor_app/src/udp_server.dart';
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
  UdpServer server = UdpServer(9090);

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

            ElevatedButton(
              onPressed: () {
                  server.start();
                  _goToSecondPage();
                },
              child: const Text('Connect'),
            ),

          ],
        ),
      ),
    );
  }



  void _goToSecondPage () {
    Navigator.push(
      context,
      MaterialPageRoute(
        builder: (context) => SecondPage(server: server,),
      ),
    );
  }

}

