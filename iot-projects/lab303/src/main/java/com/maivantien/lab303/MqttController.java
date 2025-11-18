package com.maivantien.lab303;
import org.eclipse.paho.client.mqttv3.MqttException;
import org.springframework.web.bind.annotation.*;
@RestController
@RequestMapping("/api/mqtt")

public class MqttController {
    private final MqttService mqttService;
    public MqttController(MqttService mqttService) {
        this.mqttService = mqttService;
    }
    @PostMapping("/publish")
    public String publish() {
        try {
            mqttService.publishMessage("Hi Mai Van Tien");
            return "Message published";
        } catch (MqttException e) {
            return "Failed to publish: " + e.getMessage();
        }
    }
}
