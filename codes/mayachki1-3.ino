#include "Ultrasonic.h"
#include <Wire.h>

Ultrasonic ultrasonic(2, 3); // Создаем объект класса Ultrasonic
float dist_cm = 0;
int distit_cm = 0;

#define low 4
#define middle 5
#define high 6

void setup() {
  Serial.begin(9600); // Начинаем последовательное соединение
  Wire.begin(2);      // Устанавливаем адрес I2C для ведомой платы
  Wire.onRequest(requestEvent); // Регистрация обработчика запросов

  pinMode(low, OUTPUT);
  pinMode(middle, OUTPUT);
  pinMode(high, OUTPUT);
  digitalWrite(middle, LOW);
  digitalWrite(high, LOW);
}

void loop() {
  dist_cm = ultrasonic.Ranging(CM); // Получаем расстояние
  distit_cm = (int)dist_cm;          // Приведение расстояния к целому числу
  Serial.print("Measured distance: ");
  Serial.println(distit_cm);

  // Обработка состояния для вывода на светодиоды
  Carrr(dist_cm);

  delay(100); // ожидание перед следующим измерением
}

void Carrr(float dist_cm) {
  if (dist_cm >= 45 && dist_cm <= 55) {
    Serial.println("low");
    digitalWrite(low, HIGH);
    delay(100);
    digitalWrite(low, LOW);
  } else if ((dist_cm >= 30 && dist_cm < 45) || (dist_cm >= 55 && dist_cm < 70)) {
    Serial.println("middle");
    digitalWrite(middle, HIGH);
    delay(100);
    digitalWrite(middle, LOW);
  } else if ((dist_cm >= 10 && dist_cm < 30) || (dist_cm >= 70 && dist_cm < 90)) {
    Serial.println("high");
    digitalWrite(high, HIGH);
    delay(100);
    digitalWrite(high, LOW);
  } else {
    digitalWrite(low, LOW);
    digitalWrite(middle, LOW);
    digitalWrite(high, LOW);
  }
}

void requestEvent() {
  Wire.write(distit_cm); // Передаем целое число
}
