#ifndef CONFIG_MQTT_H
#define CONFIG_MQTT_H

#define MQTT_BROKER "172.26.67.82"     
#define MQTT_PORT 1883
#define TOPIC_PREC_ALL "/chuva/dados" 

#include <WiFi.h>
#include <PubSubClient.h>

extern WiFiClient espClient;
extern PubSubClient mqttClient;

void conectaMQTT();

#endif