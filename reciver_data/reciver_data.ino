#include <WiFi.h>
#include <esp_now.h>

// ===================== STRUCT =====================
#pragma pack(push, 1)
typedef struct {
  uint8_t nodeID;
  bool headA_start;
  bool headA_timer;
  bool headA_emergency;
  uint8_t headA_state;
  bool headB_start;
  bool headB_timer;
  bool headB_emergency;
  uint8_t headB_state;
  unsigned long timestamp;
} DataPacket;

typedef struct {
  uint8_t nodeID;
  float scaleWeight;
  unsigned long timestamp;
} WeightPacket;
#pragma pack(pop)

// ===================== GLOBAL VAR =====================
DataPacket dataNode1;  // MC1 - tombol
WeightPacket dataNode3;  // MC1 - timbangan
DataPacket dataNode2;  // MC2 - tombol
WeightPacket dataNode4;  // MC2 - timbangan

const char* s(bool v) { return v ? "1" : "0"; }

// ===================== SETUP & LOOP =====================
void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  delay(1000);

  Serial.println("\n--- RECEIVER READY ---");
  Serial.print("MAC: "); Serial.println(WiFi.macAddress());

  initEspNow();  // fungsi di espnow_cb.ino
}

unsigned long lastPrint = 0;

void loop() {
  if (millis() - lastPrint >= 500) {
    printMC1();
    printMC2();
    lastPrint = millis();
  }
}
