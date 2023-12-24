import 'dart:convert';
import 'dart:io';
import 'dart:async';

class UdpServer {
  late RawDatagramSocket _socket;
  int port;
  InternetAddress? _clientAddress;
  int? _clientPort;

  UdpServer(this.port);

  void start() async {
    try {
      _socket = await RawDatagramSocket.bind(InternetAddress.anyIPv4, port);
      print('UDP Server is listening on ${_socket.address.address}:${_socket.port}');

      _socket.listen((RawSocketEvent event) {
        if (event == RawSocketEvent.read) {
          Datagram? dg = _socket.receive();
          if (dg != null) {
            String message = String.fromCharCodes(dg.data);
            print('Received message: $message from ${dg.address.address}:${dg.port}');

            // Update the client address and port
            _clientAddress = dg.address;
            _clientPort = dg.port;
          }
        }
      });
    } catch (e) {
      print("Failed to bind the socket: $e");
      // Handle exception by logging or rethrowing
    }
  }

  void sendDataToClient(String message) {
    if (_clientAddress != null && _clientPort != null) {
      List<int> data = utf8.encode(message);
      _socket.send(data, _clientAddress!, _clientPort!);
      print('Sent message: $message to $_clientAddress:$_clientPort');
    } else {
      print("No client to send data to.");
    }
  }

  void stop() {
    print("Stopping server");
    _socket.close();
  }
}
