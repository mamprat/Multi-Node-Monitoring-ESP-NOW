#include <HardwareSerial.h>

#define SCALE_RX_PIN 16
#define SCALE_TX_PIN 17

void setup() {
  Serial.begin(115200); // Monitor PC
  Serial2.begin(9600, SERIAL_8N1, SCALE_RX_PIN, SCALE_TX_PIN); 
}

void loop() {
  if (Serial2.available()) {
    String dataMentah = Serial2.readStringUntil('\r');
    dataMentah.trim();

    if (dataMentah.length() > 0) {
      // --- LOGIKA EKSTRAKSI ANGKA ---
      int posTitikDua = dataMentah.indexOf(':');
      if (posTitikDua != -1) {
        String nilaiString = dataMentah.substring(posTitikDua + 1);
        nilaiString.replace("kg", ""); // Hapus tulisan kg
        nilaiString.trim();            // Hapus spasi sisa
        
        float berat = nilaiString.toFloat(); // Ubah jadi angka float
        Serial.println(berat, 3); // Tampilkan 3 angka di belakang koma
      }
    }
  }
}
