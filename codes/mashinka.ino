#include <SoftwareSerial.h>
#include <Wire.h>

char BTData;

#define rx 2
#define tx 3
SoftwareSerial bluetooth(rx, tx); // RX, TX

#define FORWARD 'F'
#define BACKWARD 'B'
#define LEFT 'L'
#define RIGHT 'R'
#define CIRCLE 'C'
#define CROSS 'X'
#define TRIANGLE 'T'
#define SQUARE 'S'
#define START 'A'
#define PAUSE 'P'

#define m1 5 // in3
#define n1 4 // in4

#define m2 6 // in1
#define n2 7 // in2

#define attention 12
#define low 8
#define middle 9
#define high 10

int a = 1;

const int d = 50;  // Константа равная 50
const float k = -2.25; // Коэффициент для вычисления времени поворота

#define SLAVE_ADDRESS 2 // Адрес I2C слейва

int distance1 = 0;
int distance2 = 0;
int distance3 = 0;

void setup() {
    Serial.begin(115200);  // Скорость для Serial Monitor

    bluetooth.begin(9600);
    Serial.println("Bluetooth Started! Ready to pair...");
    Wire.begin(SLAVE_ADDRESS); // Инициализация слейва
    Wire.onReceive(receiveEvent); // Установка функции, которая вызывается при получении данных

    pinMode(m1, OUTPUT);
    pinMode(n1, OUTPUT);
    pinMode(m2, OUTPUT);
    pinMode(n2, OUTPUT);

    pinMode(low, INPUT);
    pinMode(middle, INPUT);
    pinMode(high, INPUT);
    pinMode(attention, INPUT);

    digitalWrite(low, LOW);
    digitalWrite(middle, LOW);
    digitalWrite(high, LOW);
    digitalWrite(attention, LOW);
}

void loop() {
    while (a == 1) {
        if (bluetooth.available()) {
            BTData = bluetooth.read(); // Чтение данных из Bluetooth
            Serial.print("Получено сообщение: ");
            Serial.println(BTData); // Отображение полученного сообщения в Serial 
            executeCommand(BTData);
        }
    }
    if (digitalRead(attention) == HIGH) {
        a = 0;
        attentionn();
        delay(5000);
    } else {
        return;
    }
}

void attentionn() {
    executeCommand(CROSS);
    executeCommand(LEFT);
    delay(700);
    executeCommand(FORWARD);  // Начинаем движение вперед

    // Ждем, пока не получим значение расстояния
    while (distance1 == 0) {
      delay(100);
    }

    int D = distance1 - d;
    float C = k * D;

    // Вычисляем время для поворота
    if (C >= 0) {
        float timeToTurn = calculateTurnTime(C);
        executeCommand(RIGHT);
        delay(timeToTurn * 1000);  // Прокладываем паузу
        executeCommand(FORWARD);
    } else if (C <= 0){
        float timeToTurn = calculateTurnTime(C);
        executeCommand(LEFT);
        delay(timeToTurn * 1000);
        executeCommand(FORWARD);
      }
    while (distance2 == 0){
      delay(100);
    }

    D = distance2 - d;
    C = k * D;

    if (C >= 0) {
        float timeToTurn = calculateTurnTime(C);
        executeCommand(RIGHT);
        delay(timeToTurn * 1000);  // Прокладываем паузу
        executeCommand(FORWARD);
    } else if (C <= 0){
        float timeToTurn = calculateTurnTime(C);
        executeCommand(LEFT);
        delay(timeToTurn * 1000);
        executeCommand(FORWARD);
      }
    while (distance3 == 0){
      delay(100);
    }
    D = distance3 - d;
    C = k * D;

    if (C >= 0) {
        float timeToTurn = calculateTurnTime(C);
        executeCommand(RIGHT);
        delay(timeToTurn * 1000);  // Прокладываем паузу
        executeCommand(FORWARD);
    } else if (C <= 0){
        float timeToTurn = calculateTurnTime(C);
        executeCommand(LEFT);
        delay(timeToTurn * 1000);
        executeCommand(FORWARD);
      }

    a = 1;
    
}


float calculateTurnTime(float angle) {
    float time = 0;

    // Повороты по углам
    if (angle > 0) {
        time += (int)(angle / 90) * 3; // Повороты по 90°
        angle -= (int)(angle / 90) * 90;

        time += (int)(angle / 60) * 2; // Повороты по 60°
        angle -= (int)(angle / 60) * 60;

        time += (int)(angle / 30) * 1; // Повороты по 30°
    } else {
        angle = -angle; 
        time += (int)(angle / 90) * 3; 
        angle -= (int)(angle / 90) * 90;

        time += (int)(angle / 60) * 2; 
        angle -= (int)(angle / 60) * 60;

        time += (int)(angle / 30) * 1; 
    }

    return time;
}

void receiveEvent(int numBytes) {
    // Проверяем, что мы получили 6 байт, потому что 3 int = 6 байт
    if (numBytes == sizeof(int) * 3) {
        byte buffer[6]; // Массив для хранения 6 байт
        Wire.readBytes(buffer, 6); // Читаем 6 байт

        // Копируем байты обратно в целые числа
        distance1 = (buffer[0] << 8) | buffer[1]; // Конвертируем 2 байта в int для distance1
        distance2 = (buffer[2] << 8) | buffer[3]; // Конвертируем 2 байта в int для distance2
        distance3 = (buffer[4] << 8) | buffer[5]; // Конвертируем 2 байта в int для distance3

        Serial.print("Расстояние маячка 1: ");
        Serial.println(distance1);
        Serial.print("Расстояние маячка 2: ");
        Serial.println(distance2);
        Serial.print("Расстояние маячка 3: ");
        Serial.println(distance3);
    } else {
        Serial.println("Ошибка: неправильное количество данных. Получено: " + String(numBytes) + " байт.");
    }
}

void executeCommand(char BTData) {
    switch (BTData) {
        case FORWARD:
            Serial.println("FORWARD");
            digitalWrite(n1, LOW);
            digitalWrite(m1, HIGH);  
            digitalWrite(n2, HIGH);
            digitalWrite(m2, LOW);
            break;
            
        case BACKWARD:
            Serial.println("BACKWARD");
            digitalWrite(n1, HIGH);
            digitalWrite(m1, LOW);  
            digitalWrite(n2, LOW);
            digitalWrite(m2, HIGH);
            break;

        case LEFT:
            Serial.println("LEFT");
            digitalWrite(n1, LOW);
            digitalWrite(m1, HIGH);  
            digitalWrite(n2, LOW);
            digitalWrite(m2, HIGH); 
            break;      
            
        case RIGHT:
            Serial.println("RIGHT");
            digitalWrite(n1, HIGH);
            digitalWrite(m1, LOW);  
            digitalWrite(n2, HIGH);
            digitalWrite(m2, LOW); 
            break;

        case CROSS:
            Serial.println("CROSS");
            digitalWrite(m1, LOW);    // Останавливаем мотор M1
            digitalWrite(m2, LOW);    // Останавливаем мотор M2
            digitalWrite(n1, LOW);
            digitalWrite(n2, LOW);
            break;
            
        default:
            break;
    }
}
