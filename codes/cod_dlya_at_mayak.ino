#include "Ultrasonic.h"

Ultrasonic ultrasonic(2, 3); // Создаем объект класса Ultrasonic
float dist_cm = 0;
int distit_cm = 0;

#define attention 4


void setup() {
  Serial.begin(9600); // Начинаем последовательное соединение


  pinMode(attention, OUTPUT);

  digitalWrite(attention, LOW);

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
  if (dist_cm < 90 && dist_cm >= 5) {
    Serial.println("attention");
    digitalWrite(attention, HIGH);
    delay(100);
    digitalWrite(attention, LOW);
  } 
  else{
    return;
  }
}
