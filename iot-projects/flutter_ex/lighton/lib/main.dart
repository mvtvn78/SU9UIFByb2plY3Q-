import 'dart:ui';
import 'dart:convert'; // <-- thêm cái này để dùng jsonEncode
import 'package:flutter/material.dart';
import 'package:http/http.dart' as http;

void main() {
  runApp(const MyApp());
}

class MyApp extends StatelessWidget {
  const MyApp({super.key});

  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      debugShowCheckedModeBanner: false,
      home: const KitchenLightPage(),
    );
  }
}

class KitchenLightPage extends StatefulWidget {
  const KitchenLightPage({super.key});

  @override
  State<KitchenLightPage> createState() => _KitchenLightPageState();
}

class _KitchenLightPageState extends State<KitchenLightPage> {
  bool isOn = false;
  double intensity = 50;
  bool _isSending = false;
  int r = 0;
  int g = 255;
  int b = 0;
  // ĐỔI IP NÀY THÀNH IP BACKEND SPRING BOOT CỦA BẠN
  final String _baseUrl = "http://10.207.122.78:8080";

  Future<void> _sendLampCommand(bool turnOn) async {
    setState(() {
      isOn = turnOn;
      if(!  turnOn){
        intensity = 0;  
        r = 0;
        g= 0;
        b= 0;
      }
      else{
        intensity = 50;
        r = 0;
        g= 255;
        b= 0;
      }
      _isSending = true;
    });

    final payload = turnOn ? '1' : '0';

    try {
      final res = await http.post(
        Uri.parse('$_baseUrl/devices/1/control'),
        headers: {'Content-Type': 'application/json'},
        body: payload,
        // body: jsonEncode({'topic': 'home/relay/cmd', 'payload': payload}),
      );

      if (!mounted) return;

      if (res.statusCode == 200) {
        ScaffoldMessenger.of(context).showSnackBar(
          SnackBar(
            content: Text('Đã gửi lệnh: $payload'),
            duration: const Duration(seconds: 1),
          ),
        );

        // Nếu bật đèn thì gửi luôn độ sáng hiện tại
        if (turnOn) {
          _sendIntensityCommand(intensity);
        }
      } else {
        // Gửi lỗi => rollback trạng thái
        setState(() {
          isOn = !turnOn;
        });
        ScaffoldMessenger.of(context).showSnackBar(
          SnackBar(content: Text('Gửi lệnh lỗi: ${res.statusCode}')),
        );
      }
    } catch (e) {
      if (!mounted) return;
      setState(() {
        isOn = !turnOn;
      });
      ScaffoldMessenger.of(context).showSnackBar(
        const SnackBar(content: Text('Không gửi được lệnh (exception)')),
      );
    } finally {
      if (mounted) {
        setState(() {
          _isSending = false;
        });
      }
    }
  }

  Future<void> _sendIntensityCommand(double value) async {
    if (!isOn) return; // chỉ gửi khi đèn đang bật

    final payload = value.round().toString();
  if(value  <=0){
          r = 0;
          g= 255;
          b= 0;
        }
        else if(value <=24)
        {
          r = 255;
          g= 0;
          b= 0;
        }
        else if(value <=50)
        {
          r = 0;
          g= 255;
          b= 0;
        }
        else if(value <=76)
        {
          r = 0;
          g= 0;
          b= 255;
        }
         else if(value <=100)
        {
          r = 255;
          g= 255;
          b= 255;
        }
    try {
      final res = await http.post(
       Uri.parse('$_baseUrl/devices/2/control'),
        headers: {'Content-Type': 'application/json'},
        body: payload,
        // body: jsonEncode({
        //   'topic': 'home/relay/brightness',
        //   'payload': payload,
        // }),
      );

      if (!mounted) return;

      if (res.statusCode == 200) {
      
       
      } else {
        debugPrint('Lỗi gửi độ sáng: ${res.statusCode}');
      }
    } catch (e) {
      debugPrint('Exception khi gửi độ sáng: $e');
    }
  }

  @override
  Widget build(BuildContext context) {
    final size = MediaQuery.of(context).size;

    return Scaffold(
      backgroundColor: const Color(0xFF233C34),
      body: Stack(
        children: [
          /// 🌟 GLOW TRẦN PHÍA SAU
          if (isOn)
            Positioned(
              top: 100,
              left: size.width * 0.62 - 230,
              child: Container(
                width: 460,
                height: 460,
                decoration: BoxDecoration(
                  shape: BoxShape.circle,
                  gradient: RadialGradient(
                    colors: [
                      Color.fromRGBO(
                        r,
                        g,
                        b,
                        (intensity / 120).clamp(0.0, 1.0),
                      ),
                       Color.fromRGBO(r, g, b, 0.0),
                    ],
                  ),
                ),
                child: BackdropFilter(
                  filter: ImageFilter.blur(sigmaX: 90, sigmaY: 90),
                  child: const SizedBox(),
                ),
              ),
            ),

          /// 💡 ĐÈN
          Positioned(
            top: -20,
            left: size.width * 0.7 - 90,
            child: Image.asset("images/lamp.png", width: 180),
          ),

          /// ✨ GLOW ĐUI ĐÈN
          Positioned(
            top: 235,
            left: size.width * 0.7 - 42,
            child: Opacity(
              opacity: isOn ? (intensity / 60).clamp(0.0, 1.0) : 0.0,
              child: Image.asset("images/bong.png", width: 85),
            ),
          ),

          /// ⬅ HEADER
          const Positioned(
            top: 40,
            left: 12,
            child: Row(
              children: [
                Icon(Icons.arrow_back, color: Colors.white),
                SizedBox(width: 6),
                Text(
                  "Kitchen",
                  style: TextStyle(color: Colors.white, fontSize: 16),
                ),
              ],
            ),
          ),

          /// 🧑‍🍳 UI CONTROL
          Positioned(
            left: 20,
            right: 20,
            bottom: 70,
            child: Column(
              crossAxisAlignment: CrossAxisAlignment.start,
              children: [
                const Text(
                  "Island Kitchen Bar",
                  style: TextStyle(
                    fontSize: 18,
                    fontWeight: FontWeight.w700,
                    color: Colors.white,
                  ),
                ),
                const SizedBox(height: 4),
                const Text(
                  "LED Pendant Ceiling Light",
                  style: TextStyle(color: Colors.white70),
                ),
                const SizedBox(height: 24),

                /// 🔘 SWITCH
                Row(
                  children: [
                    Switch(
                      value: isOn,
                      activeColor: const Color(0xFF4ADE80),
                      onChanged: _isSending
                          ? null
                          : (value) {
                            // print(value);
                              _sendLampCommand(value);
                            },
                    ),
                    const SizedBox(width: 10),
                    Text(
                      isOn ? "ON" : "OFF",
                      style: const TextStyle(color: Colors.white, fontSize: 16),
                    ),
                  ],
                ),

                const SizedBox(height: 26),
                const Text(
                  "Light Intensity",
                  style: TextStyle(color: Colors.white),
                ),
                const SizedBox(height: 8),
              
                /// 🎚 SLIDER
                Row(
                  children: [
                    const Icon(Icons.lightbulb_outline, color: Colors.white54),
                    Expanded(
                      child: Slider(
                        value: intensity,
                        min: 0,
                        max: 100,
                        activeColor: const Color(0xFF4ADE80),
                        onChanged: isOn? (value) {
                          setState(() {
                            intensity = value;
                            _sendIntensityCommand(value);
                          });
                        }: null,
                        // Gửi lệnh khi user thả tay
                        onChangeEnd: isOn ?(value) {
                          
                        } : null,
                      ),
                    ),
                    const Icon(Icons.lightbulb, color: Colors.white),
                  ],
                ),
              ],
            ),
          ),
        ],
      ),
    );
  }
}
