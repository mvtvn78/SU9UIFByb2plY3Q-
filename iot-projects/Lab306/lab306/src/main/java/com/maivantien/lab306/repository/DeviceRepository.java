package com.maivantien.lab306.repository;
import org.springframework.data.jpa.repository.JpaRepository;
import com.maivantien.lab306.model.Device;
public interface DeviceRepository extends JpaRepository<Device, Long> {
    
}
