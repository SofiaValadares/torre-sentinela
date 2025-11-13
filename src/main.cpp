#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>

// ===================== CONFIGURAÇÕES =====================
#define PREC_DIGITAL_PIN 14
#define PREC_ANALOG_PIN  32

// ---- Wi-Fi / MQTT ----
const char *WIFI_SSID     = "uaifai-tiradentes";
const char *WIFI_PASSWORD = "bemvindoaocesar";
const char *MQTT_BROKER   = "172.26.67.82";
const uint16_t MQTT_PORT  = 1883;
const char *TOPIC_PREC_ALL = "/chuva/dados"; // único tópico com tudo

// ================= SENSOR DE PRECIPITAÇÃO =================
static const int PREC_N = 10;
static uint32_t prec_soma = 0;
static uint16_t prec_buf[PREC_N];
static int prec_idx = 0;
static bool prec_cheio = false;

void precInit() {
  pinMode(PREC_DIGITAL_PIN, INPUT_PULLUP);
  pinMode(PREC_ANALOG_PIN, INPUT);
  analogSetPinAttenuation(PREC_ANALOG_PIN, ADC_11db);
  for (int i = 0; i < PREC_N; i++) prec_buf[i] = 0;
}

int precReadDigital() {
  return digitalRead(PREC_DIGITAL_PIN);
}

// =================== CONEXÃO REDE / MQTT ==================
WiFiClient net;
PubSubClient mqtt(net);

void ensureWiFi() {
  if (WiFi.status() == WL_CONNECTED) return;
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("[WiFi] Conectando");
  while (WiFi.status() != WL_CONNECTED) {
    delay(250);
    Serial.print(".");
  }
  Serial.printf("\n[WiFi] OK. IP: %s\n", WiFi.localIP().toString().c_str());
}

void ensureMqtt() {
  if (mqtt.connected()) return;
  mqtt.setServer(MQTT_BROKER, MQTT_PORT);
  String cid = "ESP32-" + String((uint32_t)ESP.getEfuseMac(), HEX);
  Serial.printf("[MQTT] Conectando em %s:%u ...\n", MQTT_BROKER, MQTT_PORT);
  while (!mqtt.connected()) {
    if (mqtt.connect(cid.c_str())) {
      Serial.println("[MQTT] Conectado.");
    } else {
      Serial.printf("[MQTT] Falhou (state=%d). Tentando novamente...\n", mqtt.state());
      delay(500);
    }
  }
}

// ========================= SETUP ==========================
void setup() {
  Serial.begin(115200);
  precInit();
  ensureWiFi();
  ensureMqtt();
  Serial.println("[APP] Pronto. Publicando tempo sem chover em /chuva/dados...");
}

// ========================== LOOP ==========================
void loop() {
  ensureWiFi();
  ensureMqtt();
  mqtt.loop();

  static uint32_t lastPub = 0;

  // Publica 1 vez por segundo
  if (millis() - lastPub >= 1000) {
    lastPub = millis();

    int digitalValue = precReadDigital();

    // =================== TEMPO SEM CHUVA ===================
    static uint32_t ultimoTempoChuva = millis();
    uint32_t agora = millis();

    if (digitalValue == 0) {
      // Sensor digital = 0 → ESTÁ CHOVENDO → reset do contador
      ultimoTempoChuva = agora;
    }

    uint32_t tempoSemChuva = (agora - ultimoTempoChuva) / 1000; // segundos sem chuva

    // Captura o ID único da placa
    uint64_t chipid = ESP.getEfuseMac();

    // Monta o payload com APENAS o tempo sem chover
    char payload[128];
    snprintf(payload, sizeof(payload),
      "{\"id\":\"%04X%08X\",\"sem_chuva_s\":%lu}",
      (uint16_t)(chipid >> 32), (uint32_t)chipid,
      tempoSemChuva
    );

    // Publica no MQTT
    bool ok = mqtt.publish(TOPIC_PREC_ALL, payload);
    Serial.printf("[MQTT] Publicado: %s -> %s\n", payload, ok ? "OK" : "FAIL");
  }

  delay(5);
}
