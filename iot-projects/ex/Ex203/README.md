Cảm biến chạm TTP223 với ESP32 MQTT LED RGB
=============================================================
Mục tiêu: Xây dựng hệ thống điều khiển bật/tắt đèn LED RGB bằng ESP32 thông qua 2
phương thức điều khiển song song:
1. MQTT (điều khiển từ xa qua Internet).
2. Cảm ứng chạm điện dung (điều khiển trực tiếp trên thiết bị)
Điều khiển qua MQTT
✓ Kết nối tới broker:
o ClientID: esp32s3-<mssv>
o Topic lệnh (subscribe): iot/<mssv>/led/cmd
o Topic trạng thái (publish): iot/<mssv>/led/state
o Topic sự kiện chạm (publish): iot/<mssv>/touch
o QoS: 1, retain cho cmd.
- Điều khiển qua MQTT
    ✓ Kết nối tới broker:
    o ClientID: esp32s3-<mssv>
    o Topic lệnh (subscribe): iot/<mssv>/led/cmd
    o Topic trạng thái (publish): iot/<mssv>/led/state
    o Topic sự kiện chạm (publish): iot/<mssv>/touch
    o QoS: 1, retain cho cmd.
    ✓ Hỗ trợ 2 định dạng payload:
    Đơn giản (chuỗi): "on" | "off" | "red" | "green" | "blue" | "white" | "toggle"
    JSON (chuẩn – bắt buộc):
    {
        "on": true,
        "color": "#RRGGBB",
        "brightness": 0-255,
        "effect": "solid|blink|breathe",
        "duration_ms": 0-60000
    }
    Phản hồi trạng thái (publish lên .../state) mỗi khi LED đổi hoặc mỗi 10s: (Done)
        {
        "on": true,
        "color": "#12AB34",
        "brightness": 128,
        "effect": "breathe",
        "src": "mqtt|touch",
        "ts": 173... // epoch ms
        }   
- Điều khiển bằng cảm ứng chạm (Done)
    ✓ Xử lý chống dội (debounce) ≥ 50–100 ms. Ưu tiên interrupt trên cạnh lên của SIG.
    ✓ Quy ước thao tác:
    o Chạm ngắn (< 400 ms): toggle LED.
    o Chạm đúp (2 lần trong 500 ms): đổi màu kế tiếp theo vòng:
    red→green→blue→white→…
    o Chạm dài (≥ 1.2 s): tăng brightness theo bậc (20%→40%→…→100%→20%).
    ✓ Mỗi sự kiện chạm cần publish lên .../touch:
    {"event":"single|double|long","ts":173...}