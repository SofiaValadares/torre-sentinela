#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

// ===================== CONFIGURAÇÕES =====================
#define PREC_DIGITAL_PIN 14  

const char *WIFI_SSID      = "uaifai-tiradentes";   
const char *WIFI_PASSWORD  = "bemvindoaocesar";     
const char *MQTT_BROKER    = "172.26.67.82";       
const uint16_t MQTT_PORT   = 1883;
const char *TOPIC_PREC_ALL = "/chuva/dados";        

WiFiClient espClient;
PubSubClient mqttClient(espClient);

// ===================== ID DA PLACA =====================
char deviceId[20];

void gerarDeviceId() {
  uint64_t chipid = ESP.getEfuseMac();
  sprintf(deviceId, "%04X%08X",
          (uint16_t)(chipid >> 32),
          (uint32_t)chipid);
}

// ===================== FREE RTOS =====================

SemaphoreHandle_t xSemTempoSemChuva;

volatile uint32_t tempoSemChuva_s = 0;

TaskHandle_t xTaskSensorChuvaHandle = NULL;
TaskHandle_t xTaskMqttPubHandle     = NULL;

// ===================== FUNÇÕES DE REDE =====================

void conectaWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
}

void conectaMQTT() {
  while (!mqttClient.connected()) {
    mqttClient.connect("esp32_chuva_client");
    if (!mqttClient.connected()) {
      delay(2000);
    }
  }
}

// ===================== TASK: SENSOR DE CHUVA =====================
void tarefaSensorChuva(void *pvParameters) {
  pinMode(PREC_DIGITAL_PIN, INPUT_PULLUP);

  for (;;) {
    int leitura = digitalRead(PREC_DIGITAL_PIN);

    if (xSemaphoreTake(xSemTempoSemChuva, pdMS_TO_TICKS(100)) == pdTRUE) {
      if (leitura == LOW) {
        tempoSemChuva_s = 0;
      } else {
        tempoSemChuva_s++;
      }
      xSemaphoreGive(xSemTempoSemChuva);
    }

    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}

// ===================== TASK: MQTT PUBLISH =====================
void tarefaMqttPublish(void *pvParameters) {
  const TickType_t intervaloEnvio = pdMS_TO_TICKS(2000); 

  for (;;) {
    if (!mqttClient.connected()) {
      conectaMQTT();
    }
    mqttClient.loop();

    uint32_t tempoCopia = 0;
    if (xSemaphoreTake(xSemTempoSemChuva, pdMS_TO_TICKS(100)) == pdTRUE) {
      tempoCopia = tempoSemChuva_s;
      xSemaphoreGive(xSemTempoSemChuva);
    }

    char payload[128];
    snprintf(payload, sizeof(payload),
             "{\"id\":\"%s\",\"tempo_sem_chuva\":%lu}",
             deviceId,
             (unsigned long)tempoCopia);

    bool ok = mqttClient.publish(TOPIC_PREC_ALL, payload);

    if (ok) {
      Serial.println(payload);
    }

    vTaskDelay(intervaloEnvio);
  }
}

// ===================== SETUP / LOOP =====================

void setup() {
  Serial.begin(115200);

  gerarDeviceId();
  conectaWiFi();

  mqttClient.setServer(MQTT_BROKER, MQTT_PORT);

  xSemTempoSemChuva = xSemaphoreCreateMutex();
  if (xSemTempoSemChuva == NULL) {
    while (true) {
      delay(1000);
    }
  }

  xTaskCreatePinnedToCore(
      tarefaSensorChuva,
      "SensorChuva",
      4096,
      NULL,
      1,
      &xTaskSensorChuvaHandle,
      1);

  xTaskCreatePinnedToCore(
      tarefaMqttPublish,
      "MqttPublish",
      4096,
      NULL,
      1,
      &xTaskMqttPubHandle,
      1);
}

void loop() {
  vTaskDelay(pdMS_TO_TICKS(1000));
}
