void SSDP_init(void) {
  // SSDP дескриптор
  HTTP.on("/description.xml", HTTP_GET, []() {
    SSDP.schema(HTTP.client());
  });
  SSDP.setDeviceType("upnp:rootdevice");
  SSDP.setSchemaURL("description.xml");
  SSDP.setHTTPPort(80);
  SSDP.setName(NAME_R);
  SSDP.setSerialNumber("SerialNumber");
  SSDP.setURL("/");
  SSDP.setModelName("ModelName");
  SSDP.setModelNumber("ModelNumber");
  SSDP.setModelURL("ModelURL");
  SSDP.setManufacturer("Manufacturer");
  SSDP.setManufacturerURL("ManufacturerURL");
  SSDP.begin();
}
