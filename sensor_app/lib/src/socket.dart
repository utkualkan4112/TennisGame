import 'dart:async';
import 'dart:convert';
import 'dart:io';

class MyServerSocket {
  late ServerSocket _serverSocket;
  List<Socket> _clients = [];

  // Start the server and listen on a specific port
  start(String address, int port) async {
    _serverSocket = await ServerSocket.bind(address, port);
    print('Server running on IP: $address, Port: $port');

    await for (Socket client in _serverSocket) {
      _handleClient(client);
      _clients.add(client);
    }
  }

  // Handle new client connections
  void _handleClient(Socket client) {
    print('Client connected: ${client.remoteAddress.address}:${client.remotePort}');
    client.listen(
            (data) {
          _handleData(client, data);
        },
        onDone: () {
          _handleDisconnect(client);
        },
        onError: (error) {
          print('Error: $error');
        }
    );
  }

  // Handle incoming data from a client
  void _handleData(Socket client, List<int> data) {
    String message = utf8.decode(data);
    print('Message from ${client.remoteAddress.address}:${client.remotePort}: $message');
    // Here you can also implement logic to process the received data or respond to the client
  }

  // Handle client disconnection
  void _handleDisconnect(Socket client) {
    print('Client disconnected: ${client.remoteAddress.address}:${client.remotePort}');
    _clients.remove(client);
    client.close();
  }

  // Send data to all connected clients
  void sendData(String data) {
    List<int> encodedData = utf8.encode(data);
    for (var client in _clients) {
      client.write(encodedData);
    }
  }

  // Send data to a specific client
  void sendDataToClient(Socket client, String data) {
    List<int> encodedData = utf8.encode(data);
    client.write(encodedData);
  }

  // Stop the server
  void stop() {
    for (var client in _clients) {
      client.close();
    }
    _serverSocket.close();
    print('Server stopped');
  }
}