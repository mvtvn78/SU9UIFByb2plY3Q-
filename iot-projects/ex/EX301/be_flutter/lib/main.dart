// pubspec.yaml dependencies:
// flutter_blue_plus: ^1.31.0
// mobile_scanner: ^5.2.3
// permission_handler: ^11.0.1

import 'package:flutter/material.dart';
import 'package:flutter_blue_plus/flutter_blue_plus.dart';
import 'package:mobile_scanner/mobile_scanner.dart';
import 'package:permission_handler/permission_handler.dart';
import 'dart:convert';

void main() {
  runApp(MyApp());
}

class MyApp extends StatelessWidget {
  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      title: 'BLE ESP32 Demo',
      theme: ThemeData(primarySwatch: Colors.blue),
      home: HomePage(),
    );
  }
}

class HomePage extends StatefulWidget {
  @override
  _HomePageState createState() => _HomePageState();
}

class _HomePageState extends State<HomePage> {
  MobileScannerController cameraController = MobileScannerController();
  String scannedData = '';
  BluetoothDevice? connectedDevice;
  BluetoothCharacteristic? writeCharacteristic;
  String receivedMessage = '';
  TextEditingController messageController = TextEditingController();
  bool isScanning = true;
  bool isConnecting = false;

  @override
  void initState() {
    super.initState();
    requestPermissions();
  }

  Future<void> requestPermissions() async {
    await [
      Permission.bluetooth,
      Permission.bluetoothScan,
      Permission.bluetoothConnect,
      Permission.location,
      Permission.camera,
    ].request();
  }

  Future<void> connectToDevice(String deviceName) async {
    setState(() {
      isConnecting = true;
      isScanning = false;
    });

    try {
      // Bắt đầu scan
      await FlutterBluePlus.startScan(timeout: Duration(seconds: 10));
      
      // Lắng nghe kết quả scan
      FlutterBluePlus.scanResults.listen((results) async {
        for (var result in results) {
          // Tìm device theo tên
          if (result.device.platformName.contains(deviceName) || 
              result.advertisementData.advName.contains(deviceName)) {
            
            await FlutterBluePlus.stopScan();
            
            setState(() {
              connectedDevice = result.device;
            });
            
            // Kết nối
            await result.device.connect();
            
            // Tăng MTU để truyền data tốt hơn
            try {
              await result.device.requestMtu(512);
            } catch (e) {
              print('MTU request failed: $e');
            }
            
            // Discover services
            List<BluetoothService> services = await result.device.discoverServices();
            
            for (var service in services) {
               print('Service UUID: ${service.uuid}');
              for (var characteristic in service.characteristics) {
                print('Char UUID: ${characteristic.uuid}, Properties: W=${characteristic.properties.write}, N=${characteristic.properties.notify}');
                // Tìm characteristic để write
                 if (characteristic.uuid.toString().toLowerCase().contains('ff01')) {
                  writeCharacteristic = characteristic;
                  print('✓ Found WRITE characteristic: ${characteristic.uuid}');
                }
                // Subscribe để nhận data
                if (characteristic.uuid.toString().toLowerCase().contains('ff02') && 
                    characteristic.properties.notify) {
                   await characteristic.setNotifyValue(true);
                   print('✓ Subscribed to NOTIFY characteristic: ${characteristic.uuid}');
                  characteristic.lastValueStream.listen((value) {
                    if (value.isNotEmpty) {
                      String message = utf8.decode(value);
                      print('Received: $message'); // Debug
                      setState(() {
                        receivedMessage = message;
                      });
                    }
                  });
                }
              }
            }
            
            setState(() {
              isConnecting = false;
            });
            
            if (mounted) {
              ScaffoldMessenger.of(context).showSnackBar(
                SnackBar(
                  content: Text('✓ Đã kết nối thành công!'),
                  backgroundColor: Colors.green,
                ),
              );
            }
            return;
          }
        }
      });

      // Timeout handling
      await Future.delayed(Duration(seconds: 11));
      
      if (connectedDevice == null && mounted) {
        setState(() {
          isConnecting = false;
          isScanning = true;
        });
        ScaffoldMessenger.of(context).showSnackBar(
          SnackBar(
            content: Text('⚠ Không tìm thấy thiết bị!'),
            backgroundColor: Colors.orange,
          ),
        );
      }
      
    } catch (e) {
      setState(() {
        isConnecting = false;
        isScanning = true;
      });
      
      if (mounted) {
        ScaffoldMessenger.of(context).showSnackBar(
          SnackBar(
            content: Text('Lỗi: $e'),
            backgroundColor: Colors.red,
          ),
        );
      }
    }
  }

  bool isSending = false;

  Future<void> sendMessage(String message) async {
    if (writeCharacteristic == null || message.isEmpty || isSending) {
      return;
    }

    setState(() {
      isSending = true;
    });

    try {
      await writeCharacteristic!.write(
        utf8.encode(message),
        withoutResponse: false,
      );
      
      // Đợi ESP32 xử lý xong
      await Future.delayed(Duration(milliseconds: 200));
      
      if (mounted) {
        ScaffoldMessenger.of(context).showSnackBar(
          SnackBar(
            content: Text('✓ Đã gửi: $message'),
            backgroundColor: Colors.green,
            duration: Duration(seconds: 1),
          ),
        );
      }
    } catch (e) {
      if (mounted) {
        ScaffoldMessenger.of(context).showSnackBar(
          SnackBar(
            content: Text('Lỗi gửi: $e'),
            backgroundColor: Colors.red,
          ),
        );
      }
    } finally {
      setState(() {
        isSending = false;
      });
    }
  }

  void handleBarcode(BarcodeCapture capture) {
    final List<Barcode> barcodes = capture.barcodes;
    if (barcodes.isNotEmpty && isScanning) {
      final String? code = barcodes.first.rawValue;
      if (code != null && code.isNotEmpty) {
        setState(() {
          scannedData = code;
        });
        connectToDevice(code);
      }
    }
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
        title: Text('BLE ESP32-C3 Demo'),
        actions: [
          if (connectedDevice != null)
            IconButton(
              icon: Icon(Icons.bluetooth_disabled),
              onPressed: () async {
                await connectedDevice?.disconnect();
                setState(() {
                  connectedDevice = null;
                  writeCharacteristic = null;
                  receivedMessage = '';
                  isScanning = true;
                });
              },
            ),
        ],
      ),
      body: Column(
        children: [
          Expanded(
            flex: 3,
            child: connectedDevice == null
                ? Stack(
                    children: [
                      MobileScanner(
                        controller: cameraController,
                        onDetect: handleBarcode,
                      ),
                      if (isConnecting)
                        Container(
                          color: Colors.black54,
                          child: Center(
                            child: Column(
                              mainAxisAlignment: MainAxisAlignment.center,
                              children: [
                                CircularProgressIndicator(color: Colors.white),
                                SizedBox(height: 20),
                                Text(
                                  'Đang kết nối...',
                                  style: TextStyle(
                                    color: Colors.white,
                                    fontSize: 18,
                                  ),
                                ),
                              ],
                            ),
                          ),
                        ),
                      Positioned(
                        bottom: 20,
                        left: 0,
                        right: 0,
                        child: Center(
                          child: Container(
                            padding: EdgeInsets.all(16),
                            decoration: BoxDecoration(
                              color: Colors.black54,
                              borderRadius: BorderRadius.circular(8),
                            ),
                            child: Text(
                              'Quét QR Code trên ESP32-C3',
                              style: TextStyle(
                                color: Colors.white,
                                fontSize: 16,
                              ),
                            ),
                          ),
                        ),
                      ),
                    ],
                  )
                : Container(
                    color: Colors.blue.shade50,
                    child: Center(
                      child: Column(
                        mainAxisAlignment: MainAxisAlignment.center,
                        children: [
                          Icon(
                            Icons.bluetooth_connected,
                            size: 100,
                            color: Colors.blue,
                          ),
                          SizedBox(height: 20),
                          Text(
                            'Đã kết nối',
                            style: TextStyle(
                              fontSize: 24,
                              fontWeight: FontWeight.bold,
                              color: Colors.blue,
                            ),
                          ),
                          SizedBox(height: 10),
                          Text(
                            connectedDevice!.platformName.isNotEmpty
                                ? connectedDevice!.platformName
                                : 'ESP32-C3',
                            style: TextStyle(fontSize: 18),
                          ),
                          SizedBox(height: 30),
                          Container(
                            margin: EdgeInsets.symmetric(horizontal: 20),
                            padding: EdgeInsets.all(16),
                            decoration: BoxDecoration(
                              color: Colors.white,
                              borderRadius: BorderRadius.circular(12),
                              boxShadow: [
                                BoxShadow(
                                  color: Colors.black12,
                                  blurRadius: 8,
                                ),
                              ],
                            ),
                            child: Column(
                              children: [
                                Text(
                                  'Tin nhận được:',
                                  style: TextStyle(
                                    fontWeight: FontWeight.bold,
                                    fontSize: 16,
                                  ),
                                ),
                                SizedBox(height: 8),
                                Text(
                                  receivedMessage.isEmpty
                                      ? 'Chưa có dữ liệu'
                                      : receivedMessage,
                                  style: TextStyle(
                                    fontSize: 18,
                                    color: receivedMessage.isEmpty
                                        ? Colors.grey
                                        : Colors.black87,
                                  ),
                                  textAlign: TextAlign.center,
                                ),
                              ],
                            ),
                          ),
                        ],
                      ),
                    ),
                  ),
          ),
          Container(
            color: Colors.white,
            padding: EdgeInsets.all(16),
            child: Column(
              children: [
                TextField(
                  controller: messageController,
                  decoration: InputDecoration(
                    labelText: 'Tin nhắn gửi đến ESP32-C3',
                    border: OutlineInputBorder(),
                    prefixIcon: Icon(Icons.message),
                  ),
                  enabled: connectedDevice != null,
                ),
                SizedBox(height: 12),
                SizedBox(
                  width: double.infinity,
                  height: 50,
                  child: ElevatedButton.icon(
                    onPressed: (connectedDevice != null && !isSending)
                        ? () {
                            if (messageController.text.isNotEmpty) {
                              sendMessage(messageController.text);
                            }
                          }
                        : null,
                    icon: isSending 
                        ? SizedBox(
                            width: 20,
                            height: 20,
                            child: CircularProgressIndicator(
                              strokeWidth: 2,
                              color: Colors.white,
                            ),
                          )
                        : Icon(Icons.send),
                    label: Text(
                      isSending ? 'Đang gửi...' : 'Gửi tin nhắn',
                      style: TextStyle(fontSize: 16),
                    ),
                    style: ElevatedButton.styleFrom(
                      shape: RoundedRectangleBorder(
                        borderRadius: BorderRadius.circular(8),
                      ),
                    ),
                  ),
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
    cameraController.dispose();
    connectedDevice?.disconnect();
    messageController.dispose();
    super.dispose();
  }
}