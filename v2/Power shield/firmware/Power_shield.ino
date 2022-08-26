#include <TinyI2CMaster.h>
float voltage;

// INA219 register values
#define INA_ADDR    0b01000000          // I2C address of INA219
#define INA_CONFIG  0b0000011001100111  // INA config register according to datasheet
#define INA_CALIB   5120                // INA calibration register according to R_SHUNT
#define CONFIG_REG  0x00                // INA configuration register address
#define CALIB_REG   0x05                // INA calibration register address
#define SHUNT_REG   0x01                // INA shunt voltage register address
#define VOLTAGE_REG 0x02                // INA bus voltage register address
#define POWER_REG   0x03                // INA power register address
#define CURRENT_REG 0x04                // INA current register address

#include <avr/wdt.h>
#include <avr/sleep.h>
#include <avr/interrupt.h>
#define adc_disable() (ADCSRA &= ~(1<<ADEN)) // disable ADC (before power-off)
#define adc_enable()  (ADCSRA |=  (1<<ADEN)) // re-enable ADC

#define R 3
#define RELE 1
boolean charging = false;
void setup() {
  TinyI2C.init();
  initINA();

  pinMode(R, OUTPUT);
  pinMode(RELE, OUTPUT);
  digitalWrite(RELE, 1); //LOW

  adc_disable();          // отключить АЦП (экономия энергии)
  wdt_reset();            // инициализация ватчдога
  wdt_enable(WDTO_8S);    // разрешаем ватчдог
  // 15MS, 30MS, 60MS, 120MS, 250MS, 500MS, 1S, 2S, 4S, 8S
  WDTCR |= _BV(WDIE);     // разрешаем прерывания по ватчдогу. Иначе будет резет.
  sei();                  // разрешаем прерывания
  set_sleep_mode(SLEEP_MODE_PWR_DOWN); // максимальный сон
}

void loop() {
  updateINA();
  indi();

  if (voltage < 3.4) {
    digitalWrite(RELE, 0); //LOW
    charging = true;
  }

  if ((voltage >= 3.4) && (charging == false)) digitalWrite(RELE, 1); //High

  if (charging) {
    if (voltage >= 4.85) charging = false;
  }

  sleep_enable();   // разрешаем сон
  sleep_cpu();      // спать!

}
void indi() {
  if (voltage > 4.5) {
    for (byte i = 0; i < 3; i++) {
      digitalWrite(R, 1);
      delay(500);
      digitalWrite(R, 0);
      delay(500);
    }
  }

  if (voltage < 4.5 && voltage > 3.9) {
    for (byte i = 0; i <= 2; i++) {
      digitalWrite(R, 1);
      delay(500);
      digitalWrite(R, 0);
      delay(500);
    }

  }

  if (voltage < 3.9) {
    digitalWrite(R, 1);
    delay(500);
    digitalWrite(R, 0);
    delay(500);
  }
}

// read sensor values from INA219
void updateINA() {
  voltage = (readRegister(VOLTAGE_REG) >> 1) & 0xfffc;
  voltage = (voltage / 1000) + 1.4;
}



ISR (WDT_vect) {
  WDTCR |= _BV(WDIE); // разрешаем прерывания по ватчдогу. Иначе будет реcет.
}
void initINA() {
  writeRegister(CONFIG_REG, INA_CONFIG);
  writeRegister(CALIB_REG,  INA_CALIB);
}
void writeRegister(uint8_t reg, uint16_t value) {
  TinyI2C.start(INA_ADDR, 0);
  TinyI2C.write(reg);
  TinyI2C.write((value >> 8) & 0xff);
  TinyI2C.write(value & 0xff);
  TinyI2C.stop();
}
// reads a register from the INA219
uint16_t readRegister(uint8_t reg) {
  uint16_t result;
  TinyI2C.start(INA_ADDR, 0);
  TinyI2C.write(reg);
  TinyI2C.restart(INA_ADDR, 2);
  result = (uint16_t)(TinyI2C.read() << 8) | TinyI2C.read();
  TinyI2C.stop();
  return (result);
}
