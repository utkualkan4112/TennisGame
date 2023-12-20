import 'dart:async';
import 'package:flutter/material.dart';
import 'package:sensor_app/src/socket.dart';
import 'package:sensors_plus/sensors_plus.dart';



class SecondPage extends StatefulWidget {
  final MySocket socket;
  const SecondPage({Key? key, required this.socket}) : super(key: key);


  @override
  State<SecondPage> createState() => _SecondPageState();

}

class _SecondPageState extends State<SecondPage> {

  static const Duration _ignoreDuration = Duration(milliseconds: 20);

  UserAccelerometerEvent? _userAccelerometerEvent;
  GyroscopeEvent? _gyroscopeEvent;

  DateTime? _userAccelerometerUpdateTime;
  DateTime? _gyroscopeUpdateTime;


  int? _userAccelerometerLastInterval;
  int? _gyroscopeLastInterval;
  final _streamSubscriptions = <StreamSubscription<dynamic>>[];

  Duration sensorInterval = SensorInterval.gameInterval;


  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
        title: const Text('Sensors Plus Example'),
        elevation: 4,
      ),
      body: Column(
        mainAxisAlignment: MainAxisAlignment.spaceEvenly,
        children: <Widget>[
          Center(
            child: DecoratedBox(
              decoration: BoxDecoration(
                border: Border.all(width: 1.0, color: Colors.black38),
              ),
            ),
          ),
          Padding(
            padding: const EdgeInsets.all(20.0),
            child: Table(
              columnWidths: const {
                0: FlexColumnWidth(4),
                4: FlexColumnWidth(2),
              },
              children: [
                const TableRow(
                  children: [
                    SizedBox.shrink(),
                    Text('X'),
                    Text('Y'),
                    Text('Z'),
                    Text('Interval'),
                  ],
                ),
                TableRow(
                  children: [
                    const Padding(
                      padding: EdgeInsets.symmetric(vertical: 8.0),
                      child: Text('UserAccelerometer'),
                    ),
                    Text(_userAccelerometerEvent?.x.toStringAsFixed(1) ?? '?'),
                    Text(_userAccelerometerEvent?.y.toStringAsFixed(1) ?? '?'),
                    Text(_userAccelerometerEvent?.z.toStringAsFixed(1) ?? '?'),
                    Text(
                        '${_userAccelerometerLastInterval?.toString() ?? '?'} ms'),
                  ],
                ),
                TableRow(
                  children: [
                    const Padding(
                      padding: EdgeInsets.symmetric(vertical: 8.0),
                      child: Text('Gyroscope'),
                    ),
                    Text(_gyroscopeEvent?.x.toStringAsFixed(1) ?? '?'),
                    Text(_gyroscopeEvent?.y.toStringAsFixed(1) ?? '?'),
                    Text(_gyroscopeEvent?.z.toStringAsFixed(1) ?? '?'),
                    Text('${_gyroscopeLastInterval?.toString() ?? '?'} ms'),
                  ],
                ),
              ],
            ),
          ),

        ],
      ),
    );
  }

  @override
  void dispose() {
    super.dispose();
    for (final subscription in _streamSubscriptions) {
      subscription.cancel();
    }
    widget.socket.closeConnection();
  }

  @override
  void initState() {
    super.initState();

    _streamSubscriptions.add(
      userAccelerometerEventStream(samplingPeriod: sensorInterval).listen(
            (UserAccelerometerEvent event) {
          final now = DateTime.now();
          setState(() {
            //widget.socket.sendData("Kemal");
            widget.socket.sendData(':GYRO:${_userAccelerometerEvent?.x.toStringAsFixed(1)} ${_userAccelerometerEvent?.y.toStringAsFixed(1)} ${_userAccelerometerEvent?.z.toStringAsFixed(1)} \n');
            //print(':GYRO:${_userAccelerometerEvent?.x.toStringAsFixed(1)} ${_userAccelerometerEvent?.y.toStringAsFixed(1)} ${_userAccelerometerEvent?.z.toStringAsFixed(1)} \n');
            _userAccelerometerEvent = event;
            if (_userAccelerometerUpdateTime != null) {
              final interval = now.difference(_userAccelerometerUpdateTime!);
              if (interval > _ignoreDuration) {
                _userAccelerometerLastInterval = interval.inMilliseconds;
              }
            }
          });
          _userAccelerometerUpdateTime = now;
        },
        onError: (e) {
          showDialog(
              context: context,
              builder: (context) {
                return const AlertDialog(
                  title: Text("Sensor Not Found"),
                  content: Text(
                      "It seems that your device doesn't support User Accelerometer Sensor"),
                );
              });
        },
        cancelOnError: true,
      ),
    );

    _streamSubscriptions.add(
      gyroscopeEventStream(samplingPeriod: sensorInterval).listen(
            (GyroscopeEvent event) {
          final now = DateTime.now();
          setState(() {
            widget.socket.sendData(':ACC:${_gyroscopeEvent?.x.toStringAsFixed(1)} ${_gyroscopeEvent?.y.toStringAsFixed(1)} ${_gyroscopeEvent?.z.toStringAsFixed(1)} \n');
            _gyroscopeEvent = event;
            if (_gyroscopeUpdateTime != null) {
              final interval = now.difference(_gyroscopeUpdateTime!);
              if (interval > _ignoreDuration) {
                _gyroscopeLastInterval = interval.inMilliseconds;
              }
            }
          });
          _gyroscopeUpdateTime = now;
        },
        onError: (e) {
          showDialog(
              context: context,
              builder: (context) {
                return const AlertDialog(
                  title: Text("Sensor Not Found"),
                  content: Text(
                      "It seems that your device doesn't support Gyroscope Sensor"),
                );
              });
        },
        cancelOnError: true,
      ),
    );


  }

}