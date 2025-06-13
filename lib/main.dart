import 'package:flutter/material.dart';
import 'dart:io'; // Needed for InternetAddress
import 'package:udp/udp.dart';

// The main entry point for the application.
void main() {
  // This line tells Flutter to run the widget named 'RobotControllerApp'.
  runApp(const RobotControllerApp());
}

// This is the root widget of your application. It's a "class".
class RobotControllerApp extends StatelessWidget {
  const RobotControllerApp({super.key});

  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      title: 'Robot UDP Controller',
      theme: ThemeData(
        primarySwatch: Colors.blue,
        brightness: Brightness.dark,
        useMaterial3: true, // Recommended for new themes
      ),
      home: const ControlScreen(),
    );
  }
}

// This is the main screen widget with all the buttons and sliders.
class ControlScreen extends StatefulWidget {
  const ControlScreen({super.key});

  @override
  _ControlScreenState createState() => _ControlScreenState();
}

class _ControlScreenState extends State<ControlScreen> {
  final _ipController = TextEditingController(text: "192.168.4.1");
  final _portController = TextEditingController(text: "1234");
  double _currentSpeed = 200.0;

  Future<void> _sendUdpCommand(String command) async {
    if (_ipController.text.isEmpty || _portController.text.isEmpty) {
      ScaffoldMessenger.of(context).showSnackBar(
        const SnackBar(content: Text('IP Address and Port cannot be empty')),
      );
      return;
    }

    try {
      var ip = _ipController.text;
      var port = int.parse(_portController.text);

      var udp = await UDP.bind(Endpoint.any());
      await udp.send(
        command.codeUnits,
        Endpoint.unicast(InternetAddress(ip), port: Port(port)),
      );
      udp.close();
      print("Sent command: '$command' to $ip:$port");
    } catch (e) {
      print("Error sending UDP command: $e");
      ScaffoldMessenger.of(
        context,
      ).showSnackBar(SnackBar(content: Text('Error: $e')));
    }
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(title: const Text('Robot UDP Controller')),
      body: SingleChildScrollView(
        padding: const EdgeInsets.all(16.0),
        child: Column(
          crossAxisAlignment: CrossAxisAlignment.stretch,
          children: <Widget>[
            TextField(
              controller: _ipController,
              decoration: const InputDecoration(labelText: 'Robot IP Address'),
            ),
            const SizedBox(height: 10),
            TextField(
              controller: _portController,
              keyboardType: TextInputType.number,
              decoration: const InputDecoration(labelText: 'Port'),
            ),

            const SizedBox(height: 30),

            Text(
              'Speed: ${_currentSpeed.toInt()}',
              style: Theme.of(context).textTheme.headlineMedium,
            ),
            Slider(
              value: _currentSpeed,
              min: 0,
              max: 255,
              divisions: 255,
              label: _currentSpeed.round().toString(),
              onChanged: (double value) {
                setState(() {
                  _currentSpeed = value;
                });
              },
            ),

            const SizedBox(height: 30),

            // --- NEW AND IMPROVED MOVEMENT BUTTONS ---
            Column(
              crossAxisAlignment: CrossAxisAlignment.center,
              children: [
                // Forward Button
                ElevatedButton(
                  style: ElevatedButton.styleFrom(
                    minimumSize: const Size(150, 40),
                  ), // Make buttons a bit wider
                  child: const Text('Forward (f)'),
                  onPressed: () =>
                      _sendUdpCommand('f ${_currentSpeed.toInt()}'),
                ),

                const SizedBox(height: 10), // A little space
                // Row for Left, Stop, Right
                Row(
                  mainAxisAlignment: MainAxisAlignment.center,
                  children: <Widget>[
                    ElevatedButton(
                      child: const Text('Left (l)'),
                      onPressed: () => _sendUdpCommand('l'),
                    ),
                    const SizedBox(width: 10), // Space between buttons
                    ElevatedButton(
                      child: const Text('Stop (s)'),
                      style: ElevatedButton.styleFrom(
                        backgroundColor: Colors.red,
                      ),
                      onPressed: () => _sendUdpCommand('s'),
                    ),
                    const SizedBox(width: 10), // Space between buttons
                    ElevatedButton(
                      child: const Text('Right (r)'),
                      onPressed: () => _sendUdpCommand('r'),
                    ),
                  ],
                ),

                const SizedBox(height: 10), // A little space
                // *** THE NEW BACKWARD BUTTON ***
                ElevatedButton(
                  style: ElevatedButton.styleFrom(
                    minimumSize: const Size(150, 40),
                  ), // Make buttons a bit wider
                  child: const Text('Backward (b)'),
                  // Assuming the command for backward is 'b'
                  onPressed: () =>
                      _sendUdpCommand('b ${_currentSpeed.toInt()}'),
                ),
              ],
            ),
          ],
        ),
      ),
    );
  }
}
