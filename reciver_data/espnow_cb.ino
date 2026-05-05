void initEspNow() {
  if (esp_now_init() != ESP_OK) {
    Serial.println("ESP-NOW Gagal Init");
    return;
  }
  esp_now_register_recv_cb(OnDataRecv);
  Serial.println("ESP-NOW OK");
}

void OnDataRecv(const esp_now_recv_info_t *info, const uint8_t *incomingData, int len) {
  uint8_t id = incomingData[0];

  switch (id) {
    case 1: handleNode1(incomingData, len); break;
    case 2: handleNode2(incomingData, len); break;
    case 3: handleNode3(incomingData, len); break;
    case 4: handleNode4(incomingData, len); break;
    default:
      Serial.printf("[UNKNOWN] ID:%d len:%d\n", id, len);
  }
}
