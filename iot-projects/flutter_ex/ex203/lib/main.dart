import 'dart:convert';
import 'package:flutter/material.dart';
import 'package:http/http.dart' as http;
void main() {
  runApp(const MyApp());
}

class MyApp extends StatelessWidget {
  const MyApp({super.key});

  @override
  Widget build(BuildContext context) {
    return const MaterialApp(
      debugShowCheckedModeBanner: false,
      home: ControlPage(),
    );
  }
}

class ControlPage extends StatefulWidget {
  const ControlPage({super.key});

  @override
  State<ControlPage> createState() => _ControlPageState();
}

class _ControlPageState extends State<ControlPage> {
  final String _baseUrl = "http://192.168.1.8:8080";
  bool isOn = true;

  int r = 255;
  int g = 255;
  int b = 255;

  double brightness = 20;
  String effect = "solid";
  int durationMs = 0;

  final TextEditingController durationController =
      TextEditingController(text: "0");

  String get colorText => "R=$r,G=$g,B=$b";

  void setColorRGB(int rr, int gg, int bb) {
    setState(() {
      r = rr;
      g = gg;
      b = bb;
    });
  }

  Map<String, dynamic> buildJson() {
    return {
      "on": isOn,
      "color": colorText,
      "brightness": brightness.toInt(),
      "effect": effect,
      "duration_ms": durationMs
    };
  }

  Future<void>  sendJson() async{
    final jsonString =
        const JsonEncoder.withIndent("  ").convert(buildJson());
    final res = await http.post(
        Uri.parse('$_baseUrl/devices/1/control'),
        headers: {'Content-Type': 'application/json'},
        body: jsonString,
        // body: jsonEncode({'topic': 'home/relay/cmd', 'payload': payload}),
      );

      if (!mounted) return;

      // if (res.statusCode == 200) {
      //   ScaffoldMessenger.of(context).showSnackBar(
      //     SnackBar(
      //       content: Text('Đã gửi lệnh: $jsonString'),
      //       duration: const Duration(seconds: 1),
      //     ),
      //   );
      // } else {
      //   ScaffoldMessenger.of(context).showSnackBar(
      //     SnackBar(
      //       content: Text('Lỗi gửi lệnh: ${res.statusCode}'),
      //       duration: const Duration(seconds: 2),
      //     ),
      //   );
      // }
  }

  Widget rgbSlider(
      String label, int value, Color color, ValueChanged<int> onChanged) {
    return Column(
      crossAxisAlignment: CrossAxisAlignment.start,
      children: [
        Text("$label: $value"),
        Slider(
          min: 0,
          max: 255,
          value: value.toDouble(),
          activeColor: color,
          onChanged: isOn
              ? (v) => onChanged(v.toInt())
              : null,
        ),
      ],
    );
  }

  Widget quickButton(
      String text, VoidCallback onPressed, Color color) {
    return ElevatedButton(
      style: ElevatedButton.styleFrom(backgroundColor: color),
      onPressed: onPressed,
      child: Text(text),
    );
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(title: const Text("LED JSON Controller")),
      body: SingleChildScrollView(
        padding: const EdgeInsets.all(16),
        child: Column(
          crossAxisAlignment: CrossAxisAlignment.start,
          children: [

           
            /// QUICK COLOR BUTTONS
          /// QUICK BUTTONS (ON / OFF / COLOR / TOGGLE)
          Wrap(
            spacing: 10,
            runSpacing: 10,
            children: [
              quickButton("ON", ()async {
                 final res = await http.post(
                      Uri.parse('$_baseUrl/devices/1/control'),
                      headers: {'Content-Type': 'application/json'},
                      body: "on",
                      // body: jsonEncode({'topic': 'home/relay/cmd', 'payload': payload}),
                    );
              }, Colors.green),
              quickButton("OFF", ()async {
                 final res = await http.post(
                      Uri.parse('$_baseUrl/devices/1/control'),
                      headers: {'Content-Type': 'application/json'},
                      body: "off",
                      // body: jsonEncode({'topic': 'home/relay/cmd', 'payload': payload}),
                    );
              }, Colors.grey),
              quickButton("TOGGLE", ()async {
                 final res = await http.post(
                      Uri.parse('$_baseUrl/devices/1/control'),
                      headers: {'Content-Type': 'application/json'},
                      body: "toggle",
                      // body: jsonEncode({'topic': 'home/relay/cmd', 'payload': payload}),
                    );
              }, Colors.orange),

              quickButton("RED", ()async {
                 final res = await http.post(
                      Uri.parse('$_baseUrl/devices/1/control'),
                      headers: {'Content-Type': 'application/json'},
                      body: "red",
                      // body: jsonEncode({'topic': 'home/relay/cmd', 'payload': payload}),
                    );
              }, Colors.red),
              quickButton("GREEN",()async {
                 final res = await http.post(
                      Uri.parse('$_baseUrl/devices/1/control'),
                      headers: {'Content-Type': 'application/json'},
                      body: "green",
                      // body: jsonEncode({'topic': 'home/relay/cmd', 'payload': payload}),
                    );
              }, Colors.green),
              quickButton("BLUE", ()async {
                 final res = await http.post(
                      Uri.parse('$_baseUrl/devices/1/control'),
                      headers: {'Content-Type': 'application/json'},
                      body: "blue",
                      // body: jsonEncode({'topic': 'home/relay/cmd', 'payload': payload}),
                    );
              }, Colors.blue),
              quickButton("WHITE", ()async {
                 final res = await http.post(
                      Uri.parse('$_baseUrl/devices/1/control'),
                      headers: {'Content-Type': 'application/json'},
                      body: "white",
                      // body: jsonEncode({'topic': 'home/relay/cmd', 'payload': payload}),
                    );
              }, Colors.white),
            ],
          ),
          const SizedBox(height: 10),
 /// ON / OFF
            SwitchListTile(
              title: const Text("ON"),
              value: isOn,
              onChanged: (v) => setState(() => isOn = v),
            ),

            

            const SizedBox(height: 20),

            /// RGB SLIDERS
            rgbSlider("R", r, Colors.red,
                (v) => setState(() => r = v)),
            rgbSlider("G", g, Colors.green,
                (v) => setState(() => g = v)),
            rgbSlider("B", b, Colors.blue,
                (v) => setState(() => b = v)),

            const SizedBox(height: 10),

            /// COLOR TEXT PREVIEW
            Text(
              "Color: $colorText",
              style: const TextStyle(
                  fontFamily: "monospace", fontSize: 16),
            ),

            const SizedBox(height: 20),

            /// BRIGHTNESS
            Text("Brightness: ${brightness.toInt()}"),
            Slider(
              min: 0,
              max: 100,
              value: brightness,
              onChanged:
                  isOn ? (v) => setState(() => brightness = v) : null,
            ),

            const SizedBox(height: 10),

            /// EFFECT
            DropdownButtonFormField<String>(
              value: effect,
              decoration: const InputDecoration(
                labelText: "Effect",
                border: OutlineInputBorder(),
              ),
              items: const [
                DropdownMenuItem(value: "solid", child: Text("Solid")),
                DropdownMenuItem(value: "blink", child: Text("Blink")),
                DropdownMenuItem(value: "breathe", child: Text("Breathe")),
              ],
              onChanged: (v) => setState(() => effect = v!),
            ),

            const SizedBox(height: 10),

            /// DURATION
            TextField(
              controller: durationController,
              keyboardType: TextInputType.number,
              decoration: const InputDecoration(
                labelText: "Duration (ms)",
                border: OutlineInputBorder(),
              ),
              onChanged: (v) => durationMs = int.tryParse(v) ?? 0,
            ),

            const SizedBox(height: 20),

            /// JSON PREVIEW
            Container(
              width: double.infinity,
              padding: const EdgeInsets.all(12),
              decoration: BoxDecoration(
                border: Border.all(color: Colors.grey),
                borderRadius: BorderRadius.circular(8),
              ),
              child: Text(
                const JsonEncoder.withIndent("  ")
                    .convert(buildJson()),
                style: const TextStyle(fontFamily: "monospace"),
              ),
            ),

            const SizedBox(height: 20),

            /// SEND
            SizedBox(
              width: double.infinity,
              child: ElevatedButton.icon(
                icon: const Icon(Icons.send),
                label: const Text("GỬI JSON"),
                onPressed: sendJson,
              ),
            ),
          ],
        ),
      ),
    );
  }
}
