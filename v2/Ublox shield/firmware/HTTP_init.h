void handle_Set_name()
{
  if (HTTP.arg("name").length() == 4)
  {
    NAME_R = HTTP.arg("name");
    Serial.println(NAME_R);
    com_name_v();
  }
  else
  {
    Serial.println("Длина должна быть равна 4 символа");
  }
  HTTP.send(200, "text/plain", "OK");
}

void handle_Res_pass()
{
  com_set_pass_v("password");
  HTTP.send(200, "text/plain", "OK");
}

void handle_Set_wifi_pass()
{
  String set_pass = HTTP.arg("set_pass");
  Serial.println(set_pass);
  com_set_pass_v(set_pass);
  HTTP.send(200, "text/plain", "OK");
}

void handle_Set_baud()
{
  unsigned long baud = HTTP.arg("baud").toInt();
  com_baud_v(baud);
  HTTP.send(200, "text/plain", "OK");
}

void handle_Set_soft_baud()
{
  unsigned long soft_baud = HTTP.arg("soft_baud").toInt();
  com_soft_baud_v(soft_baud);
  HTTP.send(200, "text/plain", "OK");
}

void handle_Set_static()
{
  com_static_v();
  HTTP.send(200, "text/plain", "OK");
}

void handle_Set_dhcp()
{
  com_dhcp_v();
  HTTP.send(200, "text/plain", "OK");
}

void handle_Set_ser_output()
{
  com_ser_v();
  HTTP.send(200, "text/plain", "OK");
}

void handle_Set_ip_add()
{
  String ip_add = HTTP.arg("ip_add");
  com_ip_add_v(ip_add);
  HTTP.send(200, "text/plain", "OK");
}

void handle_Set_ip_gw()
{
  String ip_gw = HTTP.arg("ip_gw");
  com_ip_gateway_v(ip_gw);
  HTTP.send(200, "text/plain", "OK");
}

void handle_Set_sn()
{
  String sn = HTTP.arg("sm");
  com_subnet_v(sn);
  HTTP.send(200, "text/plain", "OK");
}

void handle_Restart()
{
  String restart = HTTP.arg("device");
  if (restart == "ok")
  {
    HTTP.send(200, "text/plain", "OK");
    delay(100);
    com_restart_v();
  }
}

void handle_Reset()
{
  String res = HTTP.arg("device");
  if (res == "ok")
  {
    HTTP.send(200, "text/plain", "OK");
    delay(100);
    com_res_v();
  }
}

void handle_Wifi()
{
  String data = HTTP.arg("wifi");
  String ssid = data.substring(0, data.indexOf("---"));
  String psk = data.substring(data.indexOf("---") + 3);
  WiFi.begin(ssid.c_str(), psk.c_str());
}

void HTTP_init(void)
{
  HTTP.on("/name", handle_Set_name);
  HTTP.on("/ip_add", handle_Set_ip_add);
  HTTP.on("/ip_gw", handle_Set_ip_gw);
  HTTP.on("/sm", handle_Set_sn);
  HTTP.on("/baud", handle_Set_baud);
  HTTP.on("/soft_baud", handle_Set_soft_baud);
  HTTP.on("/set_pass", handle_Set_wifi_pass);

  HTTP.on("/ser_output", handle_Set_ser_output);
  HTTP.on("/dhcp", handle_Set_dhcp);
  HTTP.on("/static", handle_Set_static);

  HTTP.on("/restart", handle_Restart);
  HTTP.on("/reset", handle_Reset);
  HTTP.on("/res_pass", handle_Res_pass);
  HTTP.on("/wifi", handle_Wifi);

  const char *update_path = "/firmware";
  httpUpdater.setup(&HTTP, update_path);

  HTTP.begin();
}
