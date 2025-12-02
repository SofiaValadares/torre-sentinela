#include "./includes.h"

WiFiClient espClient;
PubSubClient mqttClient(espClient);

void conectaMQTT() {
  while (!mqttClient.connected()) {
    mqttClient.connect("esp32_chuva_client");
    if (!mqttClient.connected()) {
      delay(2000);
    }
  }
}