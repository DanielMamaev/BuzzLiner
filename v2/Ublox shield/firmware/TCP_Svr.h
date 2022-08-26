//--------------------------------------------------------------TCP-----------------------------------------------------------

void TCP_void()
{
  // check if there are any new clients
  if (server.hasClient())
  {
    int i;
    for (i = 0; i < MAX_SRV_CLIENTS; i++)
      if (!serverClients[i])
      {
        serverClients[i] = server.available();
        break;
      }
    if (i == MAX_SRV_CLIENTS) server.available().println("busy");
    
  }

  // check TCP clients for data
  for (int i = 0; i < MAX_SRV_CLIENTS; i++)
    while (serverClients[i].available() && mySerial.available())
    {
      size_t maxToSerial = std::min(serverClients[i].available(), mySerial.available());
      maxToSerial = std::min(maxToSerial, (size_t)STACK_PROTECTOR);
      uint8_t buf[maxToSerial];
      size_t tcp_got = serverClients[i].read(buf, maxToSerial);
      size_t serial_sent = mySerial.write(buf, tcp_got);

      //ФИЧА КОТОРАЯ ВЫВОДИТ КОМАНДЫ U-BLOX В БАЙТЫ
      //Serial.println(F("Start")); Serial.println("TCP GOT = " + String(tcp_got));
        //for (int i = 0; i < tcp_got; i++) {
        //Serial.print(buf[i]);Serial.print(", ");
        //}
      //Serial.println(F("\nEnd")); 
    }

  size_t maxToTcp = 0;
  for (int i = 0; i < MAX_SRV_CLIENTS; i++)
    if (serverClients[i])
    {
      size_t afw = serverClients[i].availableForWrite();
      if (afw)
      {
        if (!maxToTcp) maxToTcp = afw;
        else maxToTcp = std::min(maxToTcp, afw);
      }
    }

  // check UART for data
  size_t len = std::min((size_t)mySerial.available(), maxToTcp);
  len = std::min(len, (size_t)STACK_PROTECTOR);
  if (len)
  {
    uint8_t sbuf[len];
    // size_t serial_got = Serial.readBytes(sbuf, len);
    for (int i = 0; i < len; i++)
    {
      sbuf[i] = mySerial.read();
      char c = sbuf[i];

      if (serial_on)
      {
        Serial.write(c);
        // Serial_lora.write(c);
      }
    }

    // push UART data to all connected telnet clients
    for (int i = 0; i < MAX_SRV_CLIENTS; i++)
      if (serverClients[i].availableForWrite())
      {
        size_t tcp_sent = serverClients[i].write(sbuf, len);
        // Serial.write(sbuf, len);
        // Serial_lora.write(sbuf, len);
      }
  }
}
