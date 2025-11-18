package com.maivantien.lab306.controller;
import com.maivantien.lab306.model.Device;
import com.maivantien.lab306.repository.DeviceRepository;
import com.maivantien.lab306.service.MqttPublisherService;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.integration.mqtt.inbound.MqttPahoMessageDrivenChannelAdapter;
import org.springframework.web.bind.annotation.*;
import java.util.List;
@CrossOrigin("*")
@RestController
@RequestMapping("/devices")
public class DeviceController {
    @Autowired
    private DeviceRepository deviceRepository;
    @Autowired
    private MqttPublisherService mqttPublisherService;
    @Autowired
    private MqttPahoMessageDrivenChannelAdapter mqttAdapter;
    @GetMapping
    public List<Device> getAllDevices() {
        return deviceRepository.findAll();
    }
    @PostMapping
    public Device createDevice(@RequestBody Device device) {
        mqttAdapter.addTopic(device.getTopic(), 1);
        return deviceRepository.save(device);
    }
    @PostMapping("/{id}/control")
    public String controlDevice(@PathVariable Long id, @RequestBody String payload) {
        Device device = deviceRepository.findById(id).orElse(null);
        if (device != null) {
        mqttPublisherService.publish(device.getTopic(), payload);
        return "Published to " + device.getTopic();
        }
        return "Device not found";
    }
}
