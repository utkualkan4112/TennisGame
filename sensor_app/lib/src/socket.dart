import 'dart:async';
import 'dart:convert';
import 'dart:io';

class MySocket {

  late Socket _socket;

  MySocket();

  connect (String serverAddress, int port) async {

      _socket = await Socket.connect(serverAddress, port)
          .timeout(const Duration(seconds: 5), onTimeout: () {
        throw TimeoutException('Connection to server timed out');
      });

  }

  void sendData(String data) async {

    try {
      List<int> encodedData = utf8.encode(data);
      _socket.add(encodedData);
    } catch (e) {
     print('Error: $e');
    }

  }

  void closeConnection() async {
    _socket.close();
  }

}

