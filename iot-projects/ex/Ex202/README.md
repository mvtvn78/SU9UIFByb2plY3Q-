Mục tiêu: Xây dựng hệ thống điều khiển bật/tắt đèn điện xoay chiều (AC) bằng ESP32-C3 thông qua MQTT:
topic: /mvt/ex202
- Khi nhận lệnh "1" từ topic trên, ESP32-C3 sẽ kích hoạt relay để bật đèn AC.
- Khi nhận lệnh "0" từ topic trên, ESP32-C3 sẽ tắt relay để tắt đèn AC.
RELAY
IN -> GPI10
VCC -> 5V
GND -> GND
NO -> 5V
COM -> GND đèn AC
NC -> không kết nối                                                                                                          