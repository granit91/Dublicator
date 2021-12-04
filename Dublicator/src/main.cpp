#include <OneWire.h>
#define pin 11
OneWire ibutton (pin); // Пин D11 для подлючения iButton (Data)
byte addr[8];
byte ReadID[8] = { 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3D }; // "Универсальный" ключ. Прошивается последовательность 1:FF:FF:FF:FF:FF:FF:2F:CRC: 2F
// 0x01, 0x53, 0xD4, 0xFE, 0x00, 0x00, 0x00, 0x6F + синий 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x14   0x01, 0xFF, 0xFF, 0x01, 0x00, 0x00, 0x00, 0x00
// 0x01, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x2F + красный 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0xA0    0x01, 0x53, 0xD4, 0xFE, 0x00, 0x00, 0x7E, 0x88
// 0x01, 0xBE, 0x40, 0x11, 0x5A, 0x36, 0x00, 0xE1 + зеленый
// 0x01, 0x71, 0xA8, 0x75, 0x0F, 0x00, 0x00, 0xE9 + красный с точкой 0x01, 0xA9, 0xE4, 0x3C, 0x09, 0x00, 0x00, 0x00  0x01, 0x00, 0x00, 0x00, 0x00, 0x90, 0x19, 0xFF
const int buttonPin = 6; //Пин D6 Подключение кнопки
const int ledPin = 13;  // Пин D13 Подключение плюса светодиода
int buttonState = 0;
int writeflag = 0;
int readflag = 0;
int val = 0;


int writeByte(byte data) {
  int data_bit;
  for (data_bit = 0; data_bit < 8; data_bit++) {
    if (data & 1) {
      digitalWrite(pin, LOW); pinMode(pin, OUTPUT);
      delayMicroseconds(60);
      pinMode(pin, INPUT); digitalWrite(pin, HIGH);
      delay(10);
    } else {
      digitalWrite(pin, LOW); pinMode(pin, OUTPUT);
      pinMode(pin, INPUT); digitalWrite(pin, HIGH);
      delay(10);
    }
    data = data >> 1;
  }
  return 0;
}

void setup() {
  pinMode(ledPin, OUTPUT);
  pinMode(buttonPin, INPUT);
  Serial.begin(115200);
}

void loop() {
  buttonState = digitalRead(buttonPin);
  if (buttonState == HIGH) {
    readflag = 1;
    writeflag = 1;
    digitalWrite(ledPin, HIGH);
  }
  if (!ibutton.search (addr)) {
    ibutton.reset_search();
    delay(50);
    return;
  }

  digitalWrite(ledPin, HIGH);
  delay(50);
  for (byte x = 0; x < 8; x++) {
        Serial.print(addr[x], HEX);
    if (readflag == 0) {
      ReadID[x] = (addr[x]);
    }
     Serial.print(":");
  }
  byte crc; // Проверка контрольной суммы
  crc = ibutton.crc8(addr, 7);
  Serial.print("  CRC:");
  Serial.println(crc, HEX);
  digitalWrite(ledPin, LOW);

  if ((writeflag == 1) or (Serial.read() == 'w')) {
    ibutton.skip(); ibutton.reset(); ibutton.write(0x33);
    Serial.println();
    Serial.println("    ID ключа перед записью:");
    for (byte x = 0; x < 8; x++) {
      Serial.print(ibutton.read(), HEX);
     Serial.print(':');     
          }
     Serial.print("  CRC:");
     Serial.println(crc, HEX);
      // send reset
    ibutton.skip();
    ibutton.reset();
    // send 0xD1
    ibutton.write(0xD1);
    // send logical 0
    digitalWrite(pin, LOW); pinMode(pin, OUTPUT); delayMicroseconds(60);
    pinMode(pin, INPUT); digitalWrite(pin, HIGH); delay(10);
    Serial.println();
    Serial.println("     ID ключа после записи:");
    byte newID[8] = { (ReadID[0]), (ReadID[1]), (ReadID[2]), (ReadID[3]), (ReadID[4]), (ReadID[5]), (ReadID[6]), (ReadID[7]) };
    ibutton.skip();
    ibutton.reset();
    ibutton.write(0xD5);
    for (byte x = 0; x < 8; x++) {
      writeByte(newID[x]);
      Serial.print(newID[x], HEX);
      Serial.print(':');
     
    }
  Serial.print("  CRC:");
  Serial.println(crc, HEX);
  Serial.println();
    ibutton.reset();
    // send 0xD1
    ibutton.write(0xD1);
    //send logical 1
    digitalWrite(pin, LOW); pinMode(pin, OUTPUT); delayMicroseconds(10);
    pinMode(pin, INPUT); digitalWrite(pin, HIGH); delay(10);
    writeflag = 0;
    readflag = 0;
    digitalWrite(ledPin, LOW);
  }
}
