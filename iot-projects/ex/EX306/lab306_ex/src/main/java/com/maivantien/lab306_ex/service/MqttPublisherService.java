package com.maivantien.lab306_ex.service;
import org.springframework.messaging.MessageChannel;
import org.springframework.integration.support.MessageBuilder;
import org.springframework.stereotype.Service;

import lombok.AllArgsConstructor;
@Service
@AllArgsConstructor
public class MqttPublisherService {
    private MessageChannel mqttOutboundChannel;
    public void publish(String topic, String payload) {
        mqttOutboundChannel.send(MessageBuilder.withPayload(payload)
        .setHeader("mqtt_topic", topic)
        .build());
    }
}
