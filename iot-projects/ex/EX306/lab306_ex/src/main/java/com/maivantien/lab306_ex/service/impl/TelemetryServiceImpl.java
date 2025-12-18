package com.maivantien.lab306_ex.service.impl;

import java.util.List;

import org.springframework.stereotype.Service;

import com.maivantien.lab306_ex.entity.Telemetry;
import com.maivantien.lab306_ex.repository.TelemetryRepository;
import com.maivantien.lab306_ex.service.TelemetryService;

import lombok.AllArgsConstructor;
@Service
@AllArgsConstructor
public class TelemetryServiceImpl implements TelemetryService {
    private TelemetryRepository telemetryRepository;

    @Override
    public List<Telemetry> findByDevice(Long id) {
       return telemetryRepository.findByDeviceId(id);
    }
  
}
