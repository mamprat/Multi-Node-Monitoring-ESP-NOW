//sender mc1

#include <WiFi.h>
#include <esp_now.h>
#include <HardwareSerial.h>

// MAC Address Receiver sesuai foto Serial Monitor Anda
uint8_t receiverMacAddress[] = {0x7C, 0x9E, 0xBD, 0x61, 0x9B, 0x44};

#define NODE_ID          1
#define SEND_INTERVAL    500

#define PIN_START_A      32
#define PIN_TIMER_A      33
#define PIN_EMERG_A      25

#define PIN_START_B      26
#define PIN_TIMER_B      27
#define PIN_EMERG_B      21

// --- PENTING: Gunakan pragma pack agar ukuran data sama dengan Receiver ---
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
#pragma pack(pop)

DataPacket packetData;
unsigned long lastSendTime = 0;

bool latchedStartA = false;
bool lastTimerA = false;
bool latchedStartB = false;
bool lastTimerB = false;

// ================= FILTER DEBOUNCE =================
bool filteredRead(int pin) {
  if (digitalRead(pin) == LOW) {
    delay(60); // Debounce sederhana
    return digitalRead(pin) == LOW;
  }
  return false;
}

void setup() {
  Serial.begin(115200);
  int pins[] = {PIN_START_A, PIN_TIMER_A, PIN_EMERG_A, PIN_START_B, PIN_TIMER_B, PIN_EMERG_B};
  for (int p : pins) pinMode(p, INPUT_PULLUP);

  WiFi.mode(WIFI_STA);
  // Kadang perlu WiFi.disconnect() agar inisialisasi ESP-NOW lebih stabil
  WiFi.disconnect();

  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  esp_now_peer_info_t peerInfo = {};
  memcpy(peerInfo.peer_addr, receiverMacAddress, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;
  
  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Failed to add peer");
    return;
  }
  Serial.println("SENDER TOMBOL READY");
}

void loop() {
  // --- PROSES HEAD A ---
  bool curStartA = filteredRead(PIN_START_A);
  bool curTimerA = filteredRead(PIN_TIMER_A);
  bool curEmergA = filteredRead(PIN_EMERG_A);

  if (curEmergA) {
    latchedStartA = false;
  } else {
    if (curStartA) latchedStartA = true;
    if (curTimerA && !lastTimerA) latchedStartA = false;
  }
  lastTimerA = curTimerA;

  // --- PROSES HEAD B ---
  bool curStartB = filteredRead(PIN_START_B);
  bool curTimerB = filteredRead(PIN_TIMER_B);
  bool curEmergB = filteredRead(PIN_EMERG_B);

  if (curEmergB) {
    latchedStartB = false;
  } else {
    if (curStartB) latchedStartB = true;
    if (curTimerB && !lastTimerB) latchedStartB = false;
  }
  lastTimerB = curTimerB;

  // --- PENGIRIMAN DATA ---
  if (millis() - lastSendTime >= SEND_INTERVAL) {
    packetData.nodeID = NODE_ID;
    packetData.headA_start = latchedStartA;
    packetData.headA_timer = curTimerA;
    packetData.headA_emergency = curEmergA;
    packetData.headA_state = curEmergA ? 2 : (latchedStartA ? 1 : 0);

    packetData.headB_start = latchedStartB;
    packetData.headB_timer = curTimerB;
    packetData.headB_emergency = curEmergB;
    packetData.headB_state = curEmergB ? 2 : (latchedStartB ? 1 : 0);
    packetData.timestamp = millis();

    esp_err_t result = esp_now_send(receiverMacAddress, (uint8_t *)&packetData, sizeof(DataPacket));

    // Debugging untuk memastikan data terkirim
    if (result == ESP_OK) {
      Serial.printf("Sent ID:1 | A:[%d-%d-%d] B:[%d-%d-%d]\n", 
                    latchedStartA, curTimerA, curEmergA, 
                    latchedStartB, curTimerB, curEmergB);
    } else {
      Serial.println("Send Failed!");
    }
    
    lastSendTime = millis();
  }
}
