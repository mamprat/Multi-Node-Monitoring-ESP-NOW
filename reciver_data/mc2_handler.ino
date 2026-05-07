void printMC2() {
  Serial.printf("[MC2] Berat:%6.3f | A:[St:%s Ti:%s Em:%s Mode:%d] | B:[St:%s Ti:%s Em:%s Mode:%d]\n",
    dataNode4.scaleWeight,
    s(dataNode2.headA_start), s(dataNode2.headA_timer), s(dataNode2.headA_emergency), dataNode2.headA_state,
    s(dataNode2.headB_start), s(dataNode2.headB_timer), s(dataNode2.headB_emergency), dataNode2.headB_state);
}

void handleNode2(const uint8_t *data, int len) {
  if (len != sizeof(ButtonPacket)) return;
  memcpy(&dataNode2, data, sizeof(ButtonPacket));
  printMC2();
}

void handleNode4(const uint8_t *data, int len) {
  if (len != sizeof(WeightPacket)) return;
  memcpy(&dataNode4, data, sizeof(WeightPacket));
  printMC2();
}
