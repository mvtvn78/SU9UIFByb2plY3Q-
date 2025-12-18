package com.maivantien.lab306_ex.controller;
import com.maivantien.lab306_ex.entity.Telemetry;
import com.maivantien.lab306_ex.service.TelemetryService;

import lombok.AllArgsConstructor;

import org.springframework.http.HttpStatus;
import org.springframework.http.ResponseEntity;
import org.springframework.web.bind.annotation.*;
import java.util.List;
@CrossOrigin("*")
@RestController
@AllArgsConstructor
@RequestMapping("/telemetry")
public class TelemetryController {
    private TelemetryService telemetryService;
    @GetMapping("/{deviceId}")
    public ResponseEntity<List<Telemetry>> getByDevice(@PathVariable Long deviceId) {
       List<Telemetry>  telemetries= telemetryService.findByDevice(deviceId);
       return new ResponseEntity<>(telemetries, HttpStatus.OK);
    }
}
