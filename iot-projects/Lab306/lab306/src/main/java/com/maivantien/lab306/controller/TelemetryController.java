package com.maivantien.lab306.controller;
import com.maivantien.lab306.model.Telemetry;
import com.maivantien.lab306.repository.TelemetryRepository;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.web.bind.annotation.*;
import java.util.List;
@CrossOrigin("*")
@RestController
@RequestMapping("/telemetry")
public class TelemetryController {
    @Autowired
    private TelemetryRepository telemetryRepository;
    @GetMapping("/{deviceId}")
    public List<Telemetry> getByDevice(@PathVariable Long deviceId) {
        return telemetryRepository.findByDeviceId(deviceId);
    }
}
