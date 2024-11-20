#include <Arduino.h>

#include <WiFi.h>
#include <WebServer.h>


const char* ssid = "ESP32_RX5808";
const char* password = "12345678";


#define CH1_PIN 32
#define CH2_PIN 33
#define CH3_PIN 25
#define RSSI_PIN 36


WebServer server(80);


void setChannel(int channel) {
  digitalWrite(CH1_PIN, channel & 0x01);
  digitalWrite(CH2_PIN, (channel >> 1) & 0x01);
  digitalWrite(CH3_PIN, (channel >> 2) & 0x01);
}

int getRSSI() {
  int rssi = analogRead(RSSI_PIN);
  return rssi;
}

// Обработчик главной страницы
void handleRoot() {
  int rssi = getRSSI(); // Получаем RSSI

  String html = R"rawliteral(
    <!DOCTYPE html>
    <html>
    <head>
      <title>RX5808 Channel Control</title>
    </head>
    <body>
      <h1>Управление каналами RX5808</h1>
      <p>Выберите канал:</p>
      <form action="/setChannel" method="get">
        <button name="channel" value="0">Канал 1</button>
        <button name="channel" value="1">Канал 2</button>
        <button name="channel" value="2">Канал 3</button>
        <button name="channel" value="3">Канал 4</button>
        <button name="channel" value="4">Канал 5</button>
        <button name="channel" value="5">Канал 6</button>
        <button name="channel" value="6">Канал 7</button>
        <button name="channel" value="7">Канал 8</button>
      </form>
      <h2>Текущий уровень сигнала RSSI: </h2>
      <p>RSSI: %d мВ</p> <!-- Здесь будет отображаться значение RSSI -->
    </body>
    </html>
  )rawliteral";
  
  html.replace("%d", String(rssi).c_str()); // Вставляем значение RSSI на страницу
  server.send(200, "text/html", html);
}


void handleSetChannel() {
  if (server.hasArg("channel")) {
    int channel = server.arg("channel").toInt();
    if (channel >= 0 && channel <= 7) {
      setChannel(channel);
      server.send(200, "text/plain", "Канал установлен: " + String(channel));
    } else {
      server.send(400, "text/plain", "Неверный канал");
    }
  } else {
    server.send(400, "text/plain", "Канал не выбран");
  }
}

void setup() {

  pinMode(CH1_PIN, OUTPUT);
  pinMode(CH2_PIN, OUTPUT);
  pinMode(CH3_PIN, OUTPUT);


  WiFi.softAP(ssid, password);
  Serial.begin(115200);
  Serial.println("Точка доступа запущена");
  Serial.println("IP-адрес: " + WiFi.softAPIP().toString());


  server.on("/", handleRoot);
  server.on("/setChannel", handleSetChannel);
  server.begin();
  Serial.println("Веб-сервер запущен");
}

void loop() {
  server.handleClient();
}
