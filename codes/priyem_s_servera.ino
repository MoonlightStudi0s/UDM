#include <WiFi.h>
#include <HTTPClient.h>
#include <Wire.h>

// Задайте свои SSID и пароль WiFi
const char* ssid = "vladivostok";
const char* password = "312312Vl5";

// Базовый URL для запросов
const char* baseUrl = "http://192.168.236.100";

// Определим переменные для статусов и расстояний
String status[3];
int distance[3];

#define low 16
#define middle 17
#define high 4
#define attention 13

void setup() {
  Serial.begin(9600);
  WiFi.begin(ssid, password);
  Wire.begin();  // Инициализация шины I2C как мастер

  // Ждем подключения к WiFi
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Подключение к WiFi...");
  }
  
  Serial.println("Подключено к WiFi");

  pinMode(low, OUTPUT);
  pinMode(middle, OUTPUT);
  pinMode(high, OUTPUT);
  pinMode(attention, OUTPUT);
}

void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    for (int i = 1; i <= 3; i++) {
      String statusURL = String(baseUrl) + "/status" + String(i);
      String pinURL = String(baseUrl) + "/pin" + String(i);

      // Получение статуса маячка
      HTTPClient httpStatus;
      httpStatus.begin(statusURL);
      int httpResponseCodeStatus = httpStatus.GET();
      if (httpResponseCodeStatus > 0) {
        status[i - 1] = httpStatus.getString(); // Сохраняем статус
        Serial.println("Статус маячка " + String(i) + ": " + status[i - 1]);
      } else {
        Serial.print("Ошибка при выполнении запроса статуса " + String(i) + ": ");
        Serial.println(httpResponseCodeStatus);
      }
      httpStatus.end();

      // Получение значения расстояния
      HTTPClient httpPin;
      httpPin.begin(pinURL);
      int httpResponseCodePin = httpPin.GET();
      if (httpResponseCodePin > 0) {
        distance[i - 1] = httpPin.getString().toInt(); // Сохраняем расстояние
        Serial.println("Расстояние маячка " + String(i) + ": " + String(distance[i - 1]));
      } else {
        Serial.print("Ошибка при выполнении запроса расстояния " + String(i) + ": ");
        Serial.println(httpResponseCodePin);
      }
      httpPin.end();
    }

    // Отправка данных на слейв
    Wire.beginTransmission(2); // Присоединяемся к слейву с адресом 2
    for (int i = 0; i < 3; i++) {
        Wire.write((distance[i] >> 8) & 0xFF); // Отправляем старший байт
        Wire.write(distance[i] & 0xFF);        // Отправляем младший байт
    }
    Wire.endTransmission();

    // Управление сигналами на основе статуса
    for (int i = 0; i < 3; i++) {
      controlSignals(i);
    }
  } else {
    Serial.println("WiFi не подключен");
  }

  delay(1000); // Задержка на 1 секунду
}

// Функция для управления выходами на основе статуса маячка
void controlSignals(int i) {
  if (status[i] == "Опасно") {
    digitalWrite(high, HIGH);
    digitalWrite(low, LOW);
    digitalWrite(middle, LOW);
    digitalWrite(attention, LOW);
    delay(100);
    digitalWrite(high, LOW);
  } else if (status[i] == "Внимание") {
    digitalWrite(middle, HIGH);
    digitalWrite(low, LOW);
    digitalWrite(high, LOW);
    digitalWrite(attention, LOW);
    delay(100);
    digitalWrite(middle, LOW);
  } else if (status[i] == "Всё хорошо") {
    digitalWrite(low, HIGH);
    digitalWrite(middle, LOW);
    digitalWrite(high, LOW);
    digitalWrite(attention, LOW);
    delay(100);
    digitalWrite(low, LOW);
  } else {
    digitalWrite(low, LOW);
    digitalWrite(middle, LOW);
    digitalWrite(high, LOW);
    digitalWrite(attention, LOW);
  }
}
