package com.maivantien.lab306.repository;
import com.maivantien.lab306.model.Telemetry;
import org.springframework.data.jpa.repository.JpaRepository;
import java.util.List;

public interface TelemetryRepository extends JpaRepository<Telemetry, Long> {
    List<Telemetry> findByDeviceId(Long deviceId);
}
