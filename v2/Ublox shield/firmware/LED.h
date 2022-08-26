//--------------------------------------------------------------LED INDI-----------------------------------------------------------
unsigned long led_timer;
bool on_flag;

void indi() {
  if (millis() - led_timer > freq_flash) {
    led_timer = millis();
    on_flag = !on_flag;
    digitalWrite(PIN_LED, on_flag);
  }
}

void tcp_led() {
  //---статус tcp подключения
  flag_con = false;
  for (byte i = 0; i < MAX_SRV_CLIENTS; i++) {
    if (serverClients[i].connected()) flag_con = true;
    
  }
  
  if (flag_con) {
    freq_flash = 200;
  }
  else {
    freq_flash = 1500;
  }
}
