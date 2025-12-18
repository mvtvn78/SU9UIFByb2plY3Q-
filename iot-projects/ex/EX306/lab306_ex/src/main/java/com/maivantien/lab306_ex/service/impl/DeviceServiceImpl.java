package com.maivantien.lab306_ex.service.impl;
import lombok.AllArgsConstructor;

import java.util.List;
import java.util.Optional;

import org.springframework.stereotype.Service;

import com.maivantien.lab306_ex.entity.Device;
import com.maivantien.lab306_ex.repository.DeviceRepository;
import com.maivantien.lab306_ex.service.DeviceService;
@Service
@AllArgsConstructor
public class DeviceServiceImpl implements DeviceService {
    private DeviceRepository deviceRepository;
    @Override
    public List<Device> getAllDevices() {
        return deviceRepository.findAll();
    }

    @Override
    public Device getDeviceById(Long id) {
        Optional<Device> optionalDevice = deviceRepository.findById(id);
        return optionalDevice.get();
    }

    @Override
    public Device createDevice(Device device) {
         return deviceRepository.save(device);
    }
    
}
