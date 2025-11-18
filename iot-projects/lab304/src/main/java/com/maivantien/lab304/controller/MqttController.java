package com.maivantien.lab304.controller;
import com.maivantien.lab304.config.MqttConfig;
// import com.maivantien.lab304.exceptions.ExceptionMessages;
// import com.maivantien.lab304.exceptions.MqttException;
import com.maivantien.lab304.model.MqttPublishModel;
import com.maivantien.lab304.model.MqttSubscribeModel;

import org.eclipse.paho.client.mqttv3.MqttException;
import org.eclipse.paho.client.mqttv3.MqttMessage;
import org.springframework.validation.BindingResult;
import org.springframework.web.bind.annotation.*;
import javax.validation.Valid;
import java.util.ArrayList;
import java.util.List;
import java.util.concurrent.CountDownLatch;
import java.util.concurrent.TimeUnit;
@RestController
@RequestMapping(value = "/api/mqtt")
public class MqttController {
    @PostMapping("publish")
    public void publishMessage(@RequestBody @Valid MqttPublishModel messagePublishModel,
        BindingResult bindingResult) throws MqttException {
        if (bindingResult.hasErrors()) {
            // return;
            throw new MqttException(2);
        }
        MqttMessage mqttMessage = new MqttMessage(messagePublishModel.getMessage().getBytes());
        mqttMessage.setQos(messagePublishModel.getQos());
        mqttMessage.setRetained(messagePublishModel.getRetained());
        MqttConfig.getInstance().publish(messagePublishModel.getTopic(), mqttMessage);
    }
    @GetMapping("subscribe")
    public List<MqttSubscribeModel> subscribeChannel(@RequestParam(value = "topic") String topic,
        @RequestParam(value = "wait_millis") Integer waitMillis)
    throws InterruptedException, org.eclipse.paho.client.mqttv3.MqttException {
        List<MqttSubscribeModel> messages = new ArrayList<>();
        CountDownLatch countDownLatch = new CountDownLatch(10);
        MqttConfig.getInstance().subscribeWithResponse(topic, (s, mqttMessage) -> {
            MqttSubscribeModel mqttSubscribeModel = new MqttSubscribeModel();
            mqttSubscribeModel.setId(mqttMessage.getId());
            mqttSubscribeModel.setMessage(new String(mqttMessage.getPayload()));
            mqttSubscribeModel.setQos(mqttMessage.getQos());
            messages.add(mqttSubscribeModel);
            countDownLatch.countDown();
        });
        countDownLatch.await(waitMillis, TimeUnit.MILLISECONDS);
        return messages;
    }
}
