#include <esp_now.h>
#include <WiFi.h>

#define RELAY_PIN 26
typedef struct struct_message {
  char msg[32];
} struct_message;

struct_message incomingData;
bool newMessageReceived = false;

void onReceive(const esp_now_recv_info_t *recvInfo, const uint8_t *incomingDataBytes, int len) {
  if (len == sizeof(incomingData)){
    memcpy(&incomingData, incomingDataBytes, sizeof(incomingData));
    newMessageReceived = true;
  }
}

void enterLightSleep(){
  esp_sleep_enable_timer_wakeup(10 * 1000000);
  esp_light_sleep_start();
}
void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, HIGH);
  
  WiFi.mode(WIFI_STA);
  WiFi.setSleep(true);
  
  if (esp_now_init() != ESP_OK) {
    Serial.println("Errore inizializzazione ESP-NOW");
    return;
  }

  esp_now_register_recv_cb(onReceive);
}

void loop() {
  // put your main code here, to run repeatedly:
  if (newMessageReceived) {
    newMessageReceived = false;

    if (strcmp(incomingData.msg, "ON") == 0){
      digitalWrite(RELAY_PIN, LOW);
    }
    else if (strcmp(incomingData.msg, "OFF") == 0){
      digitalWrite(RELAY_PIN, HIGH);
    }
  }

  enterLightSleep();
}
