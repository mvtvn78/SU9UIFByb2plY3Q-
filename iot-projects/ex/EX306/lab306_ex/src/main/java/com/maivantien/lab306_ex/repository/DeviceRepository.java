package com.maivantien.lab306_ex.repository;

import org.springframework.data.jpa.repository.JpaRepository;
import com.maivantien.lab306_ex.entity.Device;
public interface DeviceRepository extends JpaRepository<Device, Long> {
    
}
