package com.maivantien.lab306_ex.controller;
import com.maivantien.lab306_ex.entity.Device;
import com.maivantien.lab306_ex.service.DeviceService;
import com.maivantien.lab306_ex.service.MqttPublisherService;

import lombok.AllArgsConstructor;

import org.springframework.http.HttpStatus;
import org.springframework.http.ResponseEntity;
import org.springframework.integration.mqtt.inbound.MqttPahoMessageDrivenChannelAdapter;
import org.springframework.web.bind.annotation.*;
import java.util.List;
@CrossOrigin("*")
@RestController
@AllArgsConstructor
@RequestMapping("/devices")
public class DeviceController {
    private DeviceService deviceService;
    private MqttPublisherService mqttPublisherService;
    private MqttPahoMessageDrivenChannelAdapter mqttAdapter;
    @GetMapping
    public ResponseEntity<List<Device>> getAllDevices() {
        List<Device> devices= deviceService.getAllDevices();
        return new ResponseEntity<>(devices, HttpStatus.OK);
    }
    @PostMapping
    public ResponseEntity<Device> createDevice(@RequestBody Device device) {
        Device savedDevice = deviceService.createDevice(device);
        mqttAdapter.addTopic(savedDevice.getTopic(), 1);
        return new ResponseEntity<>(savedDevice, HttpStatus.CREATED);
    }
    @PostMapping("/{id}/control")
    public ResponseEntity<String> controlDevice(@PathVariable Long id, @RequestBody String payload) {
        Device device = deviceService.getDeviceById(id);
        String str = "";
        if (device != null) {
            mqttPublisherService.publish(device.getTopic(), payload);
            str= "Published to " + device.getTopic();
            return new ResponseEntity<>(str, HttpStatus.OK);
        }
        str= "Device not found";
        return new ResponseEntity<>(str, HttpStatus.NOT_FOUND);
    }
}