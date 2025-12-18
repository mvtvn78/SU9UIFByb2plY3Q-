package com.maivantien.lab306_ex.service;

import java.util.List;

import com.maivantien.lab306_ex.entity.Telemetry;

public interface TelemetryService {
    List<Telemetry> findByDevice(Long id);
} 
