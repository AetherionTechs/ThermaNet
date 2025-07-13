#include <Arduino.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <esp_now.h>
#include <WiFi.h>

const int oneWireBus = 10;
OneWire oneWire(oneWireBus);
DallasTemperature sensors(&oneWire);

uint8_t mainStationAddress[] = {0xXX, 0xXX, 0xXX, 0xXX, 0xXX, 0xXX};  // Insert real MAC Address

typedef struct struct_message {
  char msg[32];
} struct_message;

String roomCode = "a"; //other possibilities: b,c,d,e
struct_message temperature;

void onSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("Messaggio inviato: ");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Successo" : "Fallito");
}

void setup() {
  WiFi.mode(WIFI_STA);

  if (esp_now_init() != ESP_OK){
    Serial.println("Errore inizializzazione ESP-NOW");
    return;
  }

  esp_now_register_send_cb(onSent);

  esp_now_peer_info_t peerInfo = {};
  memcpy(peerInfo.peer_addr, mainStationAddress, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;

  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Errore nell'aggiunta peer");
    return;
  }
  sensors.begin();
  sensors.requestTemperatures();
  float temperatureC = sensors.getTempCByIndex(0);
  
  String tempString = roomCode + String(temperatureC, 1);
  strncpy(temperature.msg, tempString.c_str(), sizeof(temperature.msg) - 1);
  temperature.msg[sizeof(temperature.msg) - 1] = '\0';

  esp_now_send(mainStationAddress, (uint8_t *) &temperature, sizeof(temperature));
  delay(200);

  esp_deep_sleep(10 * 60 * 1000000ULL);
}

void loop() {}
