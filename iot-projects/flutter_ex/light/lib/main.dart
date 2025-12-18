import 'package:flutter/material.dart';

class LampScreen extends StatefulWidget {
  @override
  _LampScreenState createState() => _LampScreenState();
}

class _LampScreenState extends State<LampScreen>
    with SingleTickerProviderStateMixin {
  bool isOn = false;
  double intensity = 0.5;

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      // base background (dark green-ish)
      backgroundColor: const Color(0xFF2E423B),
      body: SafeArea(
        child: Stack(
          alignment: Alignment.topCenter,
          children: [
            /// --- BACKGROUND LIGHT GLOW ---
            AnimatedContainer(
              duration: const Duration(milliseconds: 600),
              curve: Curves.easeInOut,
              decoration: BoxDecoration(
                gradient: RadialGradient(
                  center: Alignment.topCenter,
                  colors: isOn
                      ? [
                    // outer glow color (white with opacity scaled by intensity)
                    Colors.white.withOpacity(0.25 * intensity),
                    const Color(0xFF2E423B)
                  ]
                      : [
                    Colors.transparent,
                    const Color(0xFF2E423B),
                  ],
                  radius: isOn ? 1.2 : 0.5,
                ),
              ),
            ),

            Column(
              crossAxisAlignment: CrossAxisAlignment.start,
              children: [
                const SizedBox(height: 20),

                /// BACK BUTTON + TITLE
                Row(
                  children: const [
                    SizedBox(width: 20),
                    Icon(Icons.arrow_back, color: Colors.white),
                    SizedBox(width: 10),
                    Text(
                      "Kitchen",
                      style: TextStyle(color: Colors.white, fontSize: 18),
                    )
                  ],
                ),

                const SizedBox(height: 40),

                /// --- LAMP IMAGE + GLOW ---
                Center(
                  child: Column(
                    children: [
                      Image.asset(
                        "images/lamp_on.png",
                        height: 220,
                      ),

                      const SizedBox(height: 8),

                      /// --------- LIGHT BULB GLOW (fixed) ----------
                      // Use BoxDecoration + boxShadow to create soft glow
                      AnimatedOpacity(
                        duration: const Duration(milliseconds: 500),
                        opacity: isOn ? intensity : 0,
                        child: Container(
                          // size of the glowing blob under the lamp
                          height: 80,
                          width: 120,
                          decoration: BoxDecoration(
                            shape: BoxShape.circle,
                            color: Colors.white.withOpacity(0.15 * intensity),
                            boxShadow: [
                              BoxShadow(
                                color: Colors.white.withOpacity(0.25 * intensity),
                                blurRadius: 50.0,
                                spreadRadius: 30.0,
                                offset: Offset(0, 0),
                              ),
                            ],
                          ),
                        ),
                      ),
                    ],
                  ),
                ),

                const SizedBox(height: 40),

                Padding(
                  padding: const EdgeInsets.symmetric(horizontal: 20),
                  child: Column(
                    crossAxisAlignment: CrossAxisAlignment.start,
                    children: [
                      /// NAME
                      const Text(
                        "Island Kitchen Bar\nLED Pendant Ceiling Light",
                        style: TextStyle(
                          color: Colors.white,
                          fontSize: 18,
                          height: 1.4,
                        ),
                      ),

                      const SizedBox(height: 20),

                      /// SWITCH
                      Row(
                        children: [
                          GestureDetector(
                            onTap: () {
                              setState(() => isOn = !isOn);
                            },
                            child: AnimatedContainer(
                              duration: Duration(milliseconds: 300),
                              width: 60,
                              height: 32,
                              padding: EdgeInsets.symmetric(horizontal: 4),
                              decoration: BoxDecoration(
                                borderRadius: BorderRadius.circular(20),
                                color: isOn
                                    ? Colors.green.shade700
                                    : Colors.white24,
                              ),
                              child: Align(
                                alignment: isOn
                                    ? Alignment.centerRight
                                    : Alignment.centerLeft,
                                child: Container(
                                  width: 24,
                                  height: 24,
                                  decoration: BoxDecoration(
                                    color: Colors.white,
                                    shape: BoxShape.circle,
                                  ),
                                ),
                              ),
                            ),
                          ),
                          const SizedBox(width: 10),
                          Text(
                            isOn ? "ON" : "OFF",
                            style: const TextStyle(
                                color: Colors.white, fontSize: 16),
                          )
                        ],
                      ),

                      const SizedBox(height: 25),

                      /// SLIDER ONLY WHEN ON
                      if (isOn) ...[
                        const Text(
                          "Light Intensity",
                          style: TextStyle(color: Colors.white70),
                        ),
                        const SizedBox(height: 5),
                        Row(
                          children: [
                            const Icon(Icons.wb_incandescent_outlined,
                                color: Colors.white70),
                            Expanded(
                              child: Slider(
                                value: intensity,
                                onChanged: (val) {
                                  setState(() => intensity = val);
                                },
                                activeColor: Colors.green.shade400,
                                inactiveColor: Colors.white30,
                                min: 0.1,
                                max: 1.0,
                              ),
                            ),
                            const Icon(Icons.wb_incandescent,
                                color: Colors.white),
                          ],
                        ),
                      ],
                    ],
                  ),
                ),
              ],
            ),
          ],
        ),
      ),
    );
  }
}
