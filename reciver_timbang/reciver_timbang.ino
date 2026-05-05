#include <WiFi.h>
#include <esp_now.h>

// --- STRUKTUR DATA (Sama persis dengan Sender) ---
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
} ButtonPacket;

typedef struct {
  uint8_t nodeID;
  float scaleWeight;
  unsigned long timestamp;
} WeightPacket;
#pragma pack(pop)

// Variabel Global
ButtonPacket dataNode1;
WeightPacket dataNode3;

const char* s(bool v) {
  return v ? "1" : "0";
}

// Callback saat data diterima
void OnDataRecv(const esp_now_recv_info_t *info, const uint8_t *incomingData, int len) {
  uint8_t id = incomingData[0];

  // LOGIKA NODE TOMBOL (ID:1)
  if (id == 1 && len == sizeof(ButtonPacket)) {
    memcpy(&dataNode1, incomingData, sizeof(ButtonPacket));
    Serial.printf("[ID:1] Berat:%6.3f | A:[St:%s Ti:%s Em:%s Mode:%d] | B:[St:%s Ti:%s Em:%s Mode:%d]\n",
                  dataNode3.scaleWeight,
                  s(dataNode1.headA_start), s(dataNode1.headA_timer), s(dataNode1.headA_emergency), dataNode1.headA_state,
                  s(dataNode1.headB_start), s(dataNode1.headB_timer), s(dataNode1.headB_emergency), dataNode1.headB_state);
  }

  // LOGIKA NODE TIMBANGAN (ID:3) - DIUBAH AGAR TIDAK MENUNGGU ID:1
  else if (id == 3 && len == sizeof(WeightPacket)) {
    memcpy(&dataNode3, incomingData, sizeof(WeightPacket));
    // Kita langsung print saja, jika Node 1 belum ada, status tombol akan muncul 0 (normal)
    Serial.printf("[ID:3] Berat:%6.3f | A:[St:%s Ti:%s Em:%s Mode:%d] | B:[St:%s Ti:%s Em:%s Mode:%d]\n",
                  dataNode3.scaleWeight,
                  s(dataNode1.headA_start), s(dataNode1.headA_timer), s(dataNode1.headA_emergency), dataNode1.headA_state,
                  s(dataNode1.headB_start), s(dataNode1.headB_timer), s(dataNode1.headB_emergency), dataNode1.headB_state);
  }
}

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);

  // Paksa jeda agar WiFi siap
  delay(2000);

  Serial.println("\n--- RECEIVER DEBUG MODE ---");
  Serial.print("MAC: "); Serial.println(WiFi.macAddress());

  if (esp_now_init() != ESP_OK) {
    Serial.println("ESP-NOW Gagal");
    return;
  }
  esp_now_register_recv_cb(OnDataRecv);
}

void loop() {
  // Kosongkan agar tidak mengganggu Serial
}
