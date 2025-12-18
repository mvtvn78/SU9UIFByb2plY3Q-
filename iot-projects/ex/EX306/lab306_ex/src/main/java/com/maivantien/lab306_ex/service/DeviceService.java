package com.maivantien.lab306_ex.service;
import java.util.List;

import com.maivantien.lab306_ex.entity.Device;
public interface DeviceService {
   List<Device> getAllDevices();
   Device getDeviceById(Long id);
   Device createDevice(Device device);
} 