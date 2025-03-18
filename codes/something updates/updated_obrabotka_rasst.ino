#include <Wire.h>
#include <SoftwareSerial.h>

#define srx 2
#define stx 3

SoftwareSerial SUART(srx, stx); // SRX = 2, STX = 3

void setup() {
  Wire.begin(); // Инициализация I2C как ведущий
  Serial.begin(9600);
  SUART.begin(9600);
}

void loop() {
  int receivedValue1 = 0;
  int receivedValue2 = 0;
  int receivedValue3 = 0;

  // Запрос 2 байта от ведомой платы с адресом 2
  Wire.requestFrom(2, 2);
  if (Wire.available()) {
    receivedValue1 = Wire.read(); // Читаем 1 байт
  }
  Serial.print("Received from device 1: ");
  Serial.println(receivedValue1); // Выводим полученное значение

  // Запрос 2 байта от других ведомых плат, если они есть
  // Запрос от устройства 3
  Wire.requestFrom(3, 2);
  if (Wire.available()) {
    receivedValue2 = Wire.read(); // Читаем 1 байт
  }
  Serial.print("Received from device 2: ");
  Serial.println(receivedValue2); 

  // Запрос от устройства 4
  Wire.requestFrom(4, 2);
  if (Wire.available()) {
    receivedValue3 = Wire.read(); // Читаем 1 байт
  }
  Serial.print("Received from device 3: ");
  Serial.println(receivedValue3);


  // Проверяем, что значения находятся в диапазоне 0-99
  if (receivedValue1 < 0 || receivedValue1 > 99) receivedValue1 = 0;
  if (receivedValue2 < 0 || receivedValue2 > 99) receivedValue2 = 0;
  if (receivedValue3 < 0 || receivedValue3 > 99) receivedValue3 = 0;

  // Формируем строку в нужном формате
  SUART.print((receivedValue1 < 10 ? "0" : "") + String(receivedValue1) + "$");
  SUART.print((receivedValue2 < 10 ? "0" : "") + String(receivedValue2) + "$");
  SUART.print((receivedValue3 < 10 ? "0" : "") + String(receivedValue3) + "$");
  SUART.print(";");

  delay(1000); // Задержка для следующего запроса
}
