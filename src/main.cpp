#include "includes.h"

// ===================== FREE RTOS =====================
SemaphoreHandle_t xSemTempoSemChuva;

volatile uint32_t tempoSemChuva_s = 0;

TaskHandle_t xTaskSensorChuvaHandle = NULL;
TaskHandle_t xTaskMqttPubHandle     = NULL;

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
  Serial.printf("ID DA PLACA %s\n", deviceId);
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
