package com.maivantien.lab306.model;

import java.time.LocalDateTime;

import jakarta.persistence.*;
@Entity
public class Telemetry {
    @Id
    @GeneratedValue(strategy = GenerationType.IDENTITY)
    private Long id;
    private Long deviceId;
    private String payload;
    private LocalDateTime timestamp = LocalDateTime.now();
    public Long getId() {
        return id;
    }
    public void setId(Long id) {
        this.id = id;
    }
    public Long getDeviceId() {
        return deviceId;
    }
    public void setDeviceId(Long deviceId) {
        this.deviceId = deviceId;
    }
    public String getPayload() {
        return payload;
    }
    public void setPayload(String payload) {
        this.payload = payload;
    }
    public LocalDateTime getTimestamp() {
        return timestamp;
    }
    public void setTimestamp(LocalDateTime timestamp) {
        this.timestamp = timestamp;
    }
}
