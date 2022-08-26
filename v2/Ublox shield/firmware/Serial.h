/*КОМАНДЫ SERIAL
   #help - помощь по командам
   #name - название приемника. Пример #name NEWR
   #ip [ip]- ввод ip адреса. Пример #ip 192.168.0.1
   #subnet [subnet]- ввод ip макси. Пример #subnet 192.168.0.1
   #gateway [gateway]- ввод ip шлюза. Пример #gateway 192.168.0.1
   #ser - вывод данных с модуля в serial
   #restart - перезагрузка wifi
   #reset - сбрасывает настройки wifi
   #dhcp - перекоючение МК на получение IP через DHCP
   #static - перекоючение МК на статический IP
   #baud - скорость Serial порта. Пример #baud 9600
   #soft_baud - скорость SoftSerial. Пример #soft_baud 9600
   #show_wifi - вывести сообщение о SSID и PersKey и IP
   #set_pass - установка пароля для подключения к модулю по wifi. Пример #set_pass password
   #res_pass - сброс пароля wifi по умолчанию - password
   #connect - подключение к WiFi сети. Пример #connect ssid password

*/

//--------------------------------------------------------------  SERIAL CONTROL-----------------------------------------------------------
String strData = "";
boolean recievedFlag = false;

const String com_name = "#name";
const String com_ip = "#ip";
const String com_subnet = "#subnet";
const String com_gateway = "#gateway";
const String com_ser = "#ser";
const String com_restart = "#restart";
const String com_res = "#reset";
const String com_dhcp = "#dhcp";
const String com_static = "#static";
const String com_baud = "#baud";
const String com_soft_baud = "#soft_baud";
const String com_show_wifi = "#show_wifi";
const String com_set_pass = "#set_pass";
const String com_res_pass = "#res_pass";
const String com_help = "#help";
const String com_connect = "#connect";

void com_name_v()
{
  EEPROM.put(EEPROM_addr_NAME_R, NAME_R);
  if (EEPROM.commit())
    Serial.println("EEPROM successfully committed");
  else
    Serial.println("ERROR! EEPROM commit failed");
}

void com_set_pass_v(String set_pass)
{
  set_pass.replace(" ", "");
  if ((set_pass.length() == 8))
  {
    EEPROM.put(EEPROM_addr_set_pass, set_pass);
    if (EEPROM.commit())
      Serial.println("EEPROM successfully committed");
    else
      Serial.println("ERROR! EEPROM commit failed");
  }
  else
  {
    Serial.println("Enter 8 characters!");
  }
}

void com_ip_gateway_v(String ip_gateway)
{
  unsigned char *buf = new unsigned char[17];
  ip_gateway.getBytes(buf, 100, 0);
  const char *ip_buf = (const char *)buf;

  int b1, b2, b3, b4;
  sscanf(ip_buf, "%d.%d.%d.%d", &b1, &b2, &b3, &b4);
  delete[] buf;
  if (((b1 >= 0) && (b1 <= 255)) && ((b2 >= 0) && (b2 <= 255)) && ((b3 >= 0) && (b3 <= 255)) && ((b4 >= 0) && (b4 <= 255)))
  {
    Serial.println(String(b1) + "." + String(b2) + "." + String(b3) + "." + String(b4));
    EEPROM.write(EEPROM_gw_0, b1);
    EEPROM.write(EEPROM_gw_1, b2);
    EEPROM.write(EEPROM_gw_2, b3);
    EEPROM.write(EEPROM_gw_3, b4);
    if (EEPROM.commit())
    {
      Serial.println("EEPROM successfully committed");
    }
    else
    {
      Serial.println("ERROR! EEPROM commit failed");
    }
  }
}

void com_subnet_v(String subnet)
{
  unsigned char *buf = new unsigned char[17];
  subnet.getBytes(buf, 100, 0);
  const char *ip_buf = (const char *)buf;

  int b1, b2, b3, b4;
  sscanf(ip_buf, "%d.%d.%d.%d", &b1, &b2, &b3, &b4);
  delete[] buf;

  if (((b1 >= 0) && (b1 <= 255)) && ((b2 >= 0) && (b2 <= 255)) && ((b3 >= 0) && (b3 <= 255)) && ((b4 >= 0) && (b4 <= 255)))
  {
    Serial.println(String(b1) + "." + String(b2) + "." + String(b3) + "." + String(b4));
    EEPROM.write(EEPROM_sn_0, b1);
    EEPROM.write(EEPROM_sn_1, b2);
    EEPROM.write(EEPROM_sn_2, b3);
    EEPROM.write(EEPROM_sn_3, b4);
    if (EEPROM.commit())
    {
      Serial.println("EEPROM successfully committed");
    }
    else
    {
      Serial.println("ERROR! EEPROM commit failed");
    }
  }
}

void com_ip_add_v(String ip_add)
{
  unsigned char *buf = new unsigned char[17];
  ip_add.getBytes(buf, 100, 0);
  const char *ip_buf = (const char *)buf;

  int b1, b2, b3, b4;
  sscanf(ip_buf, "%d.%d.%d.%d", &b1, &b2, &b3, &b4);
  delete[] buf;
  if (((b1 >= 0) && (b1 <= 255)) && ((b2 >= 0) && (b2 <= 255)) && ((b3 >= 0) && (b3 <= 255)) && ((b4 >= 0) && (b4 <= 255)))
  {
    Serial.println(String(b1) + "." + String(b2) + "." + String(b3) + "." + String(b4));
    EEPROM.write(EEPROM_ip_0, b1);
    EEPROM.write(EEPROM_ip_1, b2);
    EEPROM.write(EEPROM_ip_2, b3);
    EEPROM.write(EEPROM_ip_3, b4);
    if (EEPROM.commit())
    {
      Serial.println("EEPROM successfully committed");
    }
    else
    {
      Serial.println("ERROR! EEPROM commit failed");
    }
  }
}

void com_baud_v(unsigned long baud)
{
  if ((baud >= 1) && (baud <= 4294967295))
  {
    EEPROM.put(EEPROM_addr_baud, baud);
    if (EEPROM.commit())
    {
      Serial.println("EEPROM successfully committed");
      EEPROM.get(EEPROM_addr_baud, baud);
      Serial.println("Baud - " + String(baud));
      delay(100);
      Serial.begin(baud);
    }
    else
      Serial.println("ERROR! EEPROM commit failed");
  }
  else
  {
    Serial.println("Please enter 0 - 4294967295!");
  }
}

void com_soft_baud_v(unsigned long soft_baud)
{

  if ((soft_baud >= 1) && (soft_baud <= 4294967295))
  {
    EEPROM.put(EEPROM_addr_soft_baud, soft_baud);
    if (EEPROM.commit())
    {
      Serial.println("EEPROM successfully committed");
      EEPROM.get(EEPROM_addr_soft_baud, soft_baud);
      Serial.println("Soft Baud - " + String(soft_baud));
      delay(100);
      mySerial.begin(soft_baud);
    }
    else
      Serial.println("ERROR! EEPROM commit failed");
  }
  else
  {
    Serial.println("Please enter 0 - 4294967295!");
  }
}

void com_static_v()
{
  EEPROM.write(EEPROM_addr_ip_mode, 1);
  if (EEPROM.commit())
  {
    Serial.println("EEPROM successfully committed");
    Serial.println("Static On. Reset MK.");
    delay(10);
    ESP.restart();
  }
  else
    Serial.println("ERROR! EEPROM commit failed");
}

void com_dhcp_v()
{
  EEPROM.write(EEPROM_addr_ip_mode, 0);
  if (EEPROM.commit())
  {
    Serial.println("EEPROM successfully committed");
    Serial.println("DHCP On. Reset MK.");
    delay(10);
    ESP.restart();
  }
  else
    Serial.println("ERROR! EEPROM commit failed");
}

void com_ser_v()
{
  serial_on = !serial_on;
  Serial.println("");
  if (serial_on)
    Serial.println("Output Serial + TCP: ON");
  if (!serial_on)
    Serial.println("Output Serial + TCP: OFF");
}

void com_restart_v()
{
  Serial.println("Restart MK");
  delay(10);
  ESP.restart();
}

void com_res_v()
{
  Serial.println("");
  Serial.println("Reset Settings WiFi");
  wifiManager.resetSettings();
  delay(10);
  ESP.restart();
}

//---- ГЛАВНАЯ ФУНКЦИЯ ПО SERIAL
void ser_con()
{
  if (Serial.available() > 0)
  {
    String inp = Serial.readString();

    if (inp.startsWith(com_name))
    {
      NAME_R = inp.substring(com_name.length());
      com_name_v();
    }

    //-- ВВОД IP АДРЕСА #ip
    else if (inp.startsWith(com_ip))
    {
      String ip_add = inp.substring(com_ip.length());
      com_ip_add_v(ip_add);
    }

    //-- ВВОД IP МАСКИ #submask
    else if (inp.startsWith(com_subnet))
    {
      String ip_subnet = inp.substring(com_subnet.length());
      com_subnet_v(ip_subnet);
    }

    //-- ВВОД IP ШЛЮЗА #gateway
    else if (inp.startsWith(com_gateway))
    {
      String ip_gateway = inp.substring(com_gateway.length());
      com_ip_gateway_v(ip_gateway);
    }

    //-- ВЫВОД ДАННЫХ С ГНСС МОДУЛЯ В SERIAL #ser
    else if (inp.startsWith(com_ser))
    {
      com_ser_v();
    }

    //-- ПЕРЕЗАПУСК ESP #restart
    else if (inp.startsWith(com_restart))
    {
      com_restart_v();
    }

    //-- СБРОС НАСТРОЕК WIFI #reset
    else if (inp.startsWith(com_res))
    {
      com_res_v();
    }

    //-- Включение DHCP #dhcp
    else if (inp.startsWith(com_dhcp))
    {
      com_dhcp_v();
    }

    //-- Включение Static
    else if (inp.startsWith(com_static))
    {
      com_static_v();
    }

    //-- Установка Serial скорости
    else if (inp.startsWith(com_baud))
    {
      unsigned long baud = inp.substring(com_baud.length()).toInt();
      com_baud_v(baud);
    }

    //-- Установка SoftSerial скорости
    else if (inp.startsWith(com_soft_baud))
    {
      unsigned long soft_baud = inp.substring(com_soft_baud.length()).toInt();
      com_soft_baud_v(soft_baud);
    }

    //-- Вывести сообщение о SSID и PersKey и IP
    else if (inp.startsWith(com_show_wifi))
    {
      Serial.println(WiFi.SSID());
      Serial.println(WiFi.psk());
      Serial.println(WiFi.localIP());
    }

    //-- установка пароля для подключения к модулю по wifi
    else if (inp.startsWith(com_set_pass))
    {
      String set_pass = inp.substring(com_set_pass.length());
      com_set_pass_v(set_pass);
    }

    //-- сброс пароля wifi по умолчанию - password
    else if (inp.startsWith(com_res_pass))
    {
      com_set_pass_v("password");
    }

    //-- подключению к wifi
    else if (inp.startsWith(com_connect))
    {
      String ssid_psk = inp.substring(com_connect.length() + 1);
      String ssid = ssid_psk.substring(0, ssid_psk.indexOf(" "));
      String psk = ssid_psk.substring(ssid_psk.indexOf(" ") + 1);
      WiFi.begin(ssid.c_str(), psk.c_str());
    }

    //-- help
    else if (inp.startsWith(com_help))
    {
      Serial.println(F("КОМАНДЫ SERIAL"));
      Serial.println(F("#help - помощь по командам"));
      Serial.println(F("#name - название приемника. Пример #name NEWR"));
      Serial.println(F("#ser - вывод данных с модуля в serial"));
      Serial.println(F("#baud - скорость Serial порта. Пример #baud 9600"));
      Serial.println(F("#soft_baud - скорость SoftSerial. Пример #soft_baud 9600"));
      Serial.println(F("#restart - перезагрузка wifi"));
      Serial.println(F("#reset - сбрасывает настройки wifi"));
      Serial.println(F("#dhcp - переключение МК на получение IP через DHCP"));
      Serial.println(F("#static - перекоючение МК на статический IP"));
      Serial.println(F("#ip [ip]- ввод ip адреса. Пример #ip 192.168.0.1"));
      Serial.println(F("#subnet [subnet]- ввод ip макси. Пример #subnet 192.168.0.1"));
      Serial.println(F("#gateway [gateway]- ввод ip шлюза. Пример #gateway 192.168.0.1"));
      Serial.println(F("#show_wifi - вывести сообщение о SSID, Password и IP"));
      Serial.println(F("#set_pass - установка пароля для подключения к модулю по wifi. Пример #set_pass password"));
      Serial.println(F("#res_pass - сброс пароля wifi по умолчанию - password"));
      Serial.println(F("#connect - подключение к WiFi сети. Пример #connect ssid password"));
    }
    else
    {
      Serial.println("Command entered is unknown!");
    }
  }
}
