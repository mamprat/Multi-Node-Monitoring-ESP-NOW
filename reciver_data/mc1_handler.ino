void printMC1() {
  Serial.printf("[MC1] Berat:%6.3f | A:[St:%s Ti:%s Em:%s Mode:%d] | B:[St:%s Ti:%s Em:%s Mode:%d]\n",
    dataNode3.scaleWeight,
    s(dataNode1.headA_start), s(dataNode1.headA_timer), s(dataNode1.headA_emergency), dataNode1.headA_state,
    s(dataNode1.headB_start), s(dataNode1.headB_timer), s(dataNode1.headB_emergency), dataNode1.headB_state);
}

void handleNode1(const uint8_t *data, int len) {
  if (len != sizeof(ButtonPacket)) return;
  memcpy(&dataNode1, data, sizeof(ButtonPacket));
  printMC1();
}

void handleNode3(const uint8_t *data, int len) {
  if (len != sizeof(WeightPacket)) return;
  memcpy(&dataNode3, data, sizeof(WeightPacket));
  printMC1();
}
