#include <WiFi.h>
#include <WebServer.h>
#include <Wire.h>

#define attention 16
#define low1 17
#define middle1 4
#define high1 13
#define low2 23
#define middle2 19
#define high2 18
#define low3 14
#define middle3 15
#define high3 5

#define stx 21 // Пин TX для SUART
#define srx 22 // Пин RX для SUART

HardwareSerial SUART(1); // Используем второй последовательный порт

const char* ssid = "vladivostok";
const char* password = "312312Vl5";

// Определите статический IP-адрес, шлюз и маску подсети
IPAddress staticIP(192, 168, 236, 100);  // Статический IP-адрес
IPAddress gateway(192, 168, 236, 27);    // Основной шлюз
IPAddress subnet(255, 255, 255, 0);      // Маска подсети

WebServer server(80);

int num1 = 0, num2 = 0, num3 = 0; // Переменные для хранения расстояний

void setup() {
    Serial.begin(9600);
    SUART.begin(9600, SERIAL_8N1, srx, stx); // Настраиваем HardwareSerial

    pinMode(low1, INPUT);
    pinMode(middle1, INPUT);
    pinMode(high1, INPUT);
    
    pinMode(low2, INPUT);
    pinMode(middle2, INPUT);
    pinMode(high2, INPUT);
    
    pinMode(low3, INPUT);
    pinMode(middle3, INPUT);
    pinMode(high3, INPUT);

    pinMode(attention, INPUT);
  
    // Настройка Wi-Fi с использованием статического IP
    WiFi.config(staticIP, gateway, subnet);
    WiFi.begin(ssid, password);
  
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.println("Подключение к WiFi...");
    }
    
    Serial.println("Подключено к WiFi");
    Serial.print("Статический IP: ");
    Serial.println(WiFi.localIP());

    server.on("/", handleRoot);
    server.on("/status1", mayakStatus1);
    server.on("/pin1", mayakRasst1);
    server.on("/status2", mayakStatus2);
    server.on("/pin2", mayakRasst2);
    server.on("/status3", mayakStatus3);
    server.on("/pin3", mayakRasst3);
    
    server.begin();
}

void loop() {
    server.handleClient();
    readDistanceData(); // Читаем данные о расстоянии в цикле
}

void handleRoot() {
    String html = "<html><body>";
    
    html += "<h1>Статус маячка 1:</h1>";
    html += "<p id='status1'>" + getPinStatus1() + "</p>";
    html += "<p id='pin1-status'>Расстояние 1: " + String(num1) + "</p>"; 

    html += "<h1>Статус маячка 2:</h1>";
    html += "<p id='status2'>" + getPinStatus2() + "</p>";
    html += "<p id='pin2-status'>Расстояние 2: " + String(num2) + "</p>"; 

    html += "<h1>Статус маячка 3:</h1>";
    html += "<p id='status3'>" + getPinStatus3() + "</p>";
    html += "<p id='pin3-status'>Расстояние 3: " + String(num3) + "</p>"; 

    // JavaScript для обновления интерфейса
    html += "<script>setInterval(function() {fetch('/status1').then(response => response.text()).then(data => document.getElementById('status1').innerHTML = data);}, 1000);</script>";
    html += "<script>setInterval(function() {fetch('/pin1').then(response => response.text()).then(data => document.getElementById('pin1-status').innerHTML = data);}, 1000);</script>";
    html += "<script>setInterval(function() {fetch('/status2').then(response => response.text()).then(data => document.getElementById('status2').innerHTML = data);}, 1000);</script>";
    html += "<script>setInterval(function() {fetch('/pin2').then(response => response.text()).then(data => document.getElementById('pin2-status').innerHTML = data);}, 1000);</script>";
    html += "<script>setInterval(function() {fetch('/status3').then(response => response.text()).then(data => document.getElementById('status3').innerHTML = data);}, 1000);</script>";
    html += "<script>setInterval(function() {fetch('/pin3').then(response => response.text()).then(data => document.getElementById('pin3-status').innerHTML = data);}, 1000);</script>";

    html += "</body></html>";
    server.send(200, "text/html; charset=utf-8", html);
}

// Функции для маячка 1
void mayakStatus1() {
    String status = getPinStatus1();
    server.send(200, "text/plain; charset=utf-8", status);
}

void mayakRasst1() {
    readDistanceData();
    Serial.print("Значение расстояния маячка 1: ");
    Serial.println(num1);
    server.send(200, "text/plain; charset=utf-8", String(num1));
}

// Функции для маячка 2
void mayakStatus2() {
    String status = getPinStatus2();
    server.send(200, "text/plain; charset=utf-8", status);
}

void mayakRasst2() {
    readDistanceData();
    Serial.print("Значение расстояния маячка 2: ");
    Serial.println(num2);
    server.send(200, "text/plain; charset=utf-8", String(num2));
}

// Функции для маячка 3
void mayakStatus3() {
    String status = getPinStatus3();
    server.send(200, "text/plain; charset=utf-8", status);
}

void mayakRasst3() {
    readDistanceData();
    Serial.print("Значение расстояния маячка 3: ");
    Serial.println(num3);
    server.send(200, "text/plain; charset=utf-8", String(num3));
}

// Получение статуса для маячка 1
String getPinStatus1() { 
    int lowValue = digitalRead(low1);
    int middleValue = digitalRead(middle1);
    int highValue = digitalRead(high1);

    Serial.print("Маячок 1 - Low: "); Serial.print(lowValue);
    Serial.print(", Middle: "); Serial.print(middleValue);
    Serial.print(", High: "); Serial.println(highValue);

    if (lowValue == HIGH) {
        return "Всё хорошо";
    } else if (middleValue == HIGH) {
        return "Внимание";
    } else if (highValue == HIGH) {
        return "Опасно";
    } else {
        return "Ожидание";
    }
}

// Получение статуса для маячка 2
String getPinStatus2() { 
    int lowValue = digitalRead(low2);
    int middleValue = digitalRead(middle2);
    int highValue = digitalRead(high2);

    Serial.print("Маячок 2 - Low: "); Serial.print(lowValue);
    Serial.print(", Middle: "); Serial.print(middleValue);
    Serial.print(", High: "); Serial.println(highValue);

    if (lowValue == HIGH) {
        return "Всё хорошо";
    } else if (middleValue == HIGH) {
        return "Внимание";
    } else if (highValue == HIGH) {
        return "Опасно";
    } else {
        return "Ожидание";
    }
}

// Получение статуса для маячка 3
String getPinStatus3() { 
    int lowValue = digitalRead(low3);
    int middleValue = digitalRead(middle3);
    int highValue = digitalRead(high3);

    Serial.print("Маячок 3 - Low: "); Serial.print(lowValue);
    Serial.print(", Middle: "); Serial.print(middleValue);
    Serial.print(", High: "); Serial.println(highValue);

    if (lowValue == HIGH) {
        return "Всё хорошо";
    } else if (middleValue == HIGH) {
        return "Внимание";
    } else if (highValue == HIGH) {
        return "Опасно";
    } else {
        return "Ожидание";
    }
}

void readDistanceData() {
    if (SUART.available()) {
        Serial.println("Читаю данные");
        String numberString = ""; // Создаем пустую строку для хранения числа
        Serial.println("Данные доступны для чтения."); // Отладка
        // Читаем данные, пока есть доступные символы
        while (true) {
            char ch = SUART.read();
            Serial.print(ch); // Выводим получаемые символы для отладки
            if (ch == ';') {
                parseData(numberString); // Обработка завершенной строки
                numberString = ""; // Очищаем строку для следующей передачи
                break; // Выход из цикла
            }
            numberString += ch; // Добавляем символ в строку
        }
        Serial.println("Чтение завершено."); // Отладка
    }
}

// Обрабатываем данные о расстоянии
void parseData(String data) {
    Serial.print("Полученные данные: "); 
    Serial.println(data); // Выводим полученные данные для отладки
    // Будем искать символ '$' для разделения значений
    int startIndex = 0; // Начальный индекс для извлечения данных
    int endIndex = data.indexOf('$'); // Поиск первого '$'

    // Переменные для хранения чисел
    int numReceived[3] = {0, 0, 0}; // Массив для новых значений

    for (int i = 0; i < 3; i++) { // Ожидаем 3 числа
        if (endIndex == -1) {
            Serial.println("Ошибка: недостаточно данных.");
            return; // Если не хватает данных, выходим из функции
        }

        // Извлекаем значение между начальной и конечной позициями
        String numString = data.substring(startIndex, endIndex);
        int num = numString.toInt(); // Преобразуем строку в число
        Serial.print("Получено значение "); 
        Serial.print(i + 1); // Номер значения
        Serial.print(": ");
        Serial.println(num); // Выводим значение в последовательный монитор
        numReceived[i] = num; // Сохраняем значение в массив

        // Обновляем индексы для следующей итерации
        startIndex = endIndex + 1; // Переход к следующему значению
        endIndex = data.indexOf('$', startIndex); // Поиск следующего '$'
    }

    // Обновляем значения, сохраненные в переменных
    num1 = numReceived[0];
    num2 = numReceived[1];
    num3 = numReceived[2];

    // Выводим результаты для отладки
    Serial.print("Число 1: "); Serial.println(num1);
    Serial.print("Число 2: "); Serial.println(num2); 
    Serial.print("Число 3: "); Serial.println(num3);
}

