///timbangan mc2

#include <WiFi.h>
#include <esp_now.h>
#include <HardwareSerial.h>

// Masukkan MAC Address Receiver Anda
uint8_t receiverMacAddress[] = {0x7C, 0x9E, 0xBD, 0x61, 0x9B, 0x44};

#define NODE_ID         4
#define SEND_INTERVAL   500
#define SCALE_RX_PIN    16
#define SCALE_TX_PIN    17

// Struktur data harus sama persis dengan Receiver
typedef struct {
  uint8_t nodeID;
  float scaleWeight;
  unsigned long timestamp;
} ScaleDataPacket;

ScaleDataPacket packetData;
unsigned long lastSendTime = 0;
float currentWeight = 0.0;

void setup() {
  Serial.begin(115200);
  
  // Inisialisasi Serial2 untuk Timbangan (Baudrate 9600 sesuai manual)
  Serial2.begin(9600, SERIAL_8N1, SCALE_RX_PIN, SCALE_TX_PIN);

  // Setup WiFi dan ESP-NOW
  WiFi.mode(WIFI_STA);
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Registrasi Peer
  esp_now_peer_info_t peerInfo = {};
  memcpy(peerInfo.peer_addr, receiverMacAddress, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;
  
  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Failed to add peer");
    return;
  }
}

void loop() {
  // 1. Parsing Data dari Serial Timbangan
  if (Serial2.available()) {
    // Membaca data sampai karakter [D] / Carriage Return [cite: 147, 152]
    String dataMentah = Serial2.readStringUntil('\r'); 
    dataMentah.trim();

    if (dataMentah.length() > 0) {
      int posTitikDua = dataMentah.indexOf(':');
      if (posTitikDua != -1) {
        String nilaiString = dataMentah.substring(posTitikDua + 1);
        nilaiString.replace("kg", "");
        nilaiString.trim();
        currentWeight = nilaiString.toFloat();
      }
    }
  }

  // 2. Kirim Data secara berkala
  if (millis() - lastSendTime >= SEND_INTERVAL) {
    packetData.nodeID = NODE_ID;
    packetData.scaleWeight = currentWeight;
    packetData.timestamp = millis();

    esp_err_t result = esp_now_send(receiverMacAddress, (uint8_t *) &packetData, sizeof(packetData));
    
    // Debug ke Serial Monitor
    Serial.println(currentWeight, 3);

    lastSendTime = millis();
  }
}
