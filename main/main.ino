#include <WiFi.h>
#include "SPIFFS.h"
#include "ESPAsyncWebServer.h"

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HELGTH 64
#define OLE_RESET -1
#define SCREEN_ADDRESS 0x3C

const byte MotorA1 = 32;
const byte MotorA2 = 33;
const byte MotorA3 = 25;
const byte MotorA4 = 26;

const char *ssid = "ShibaInu's Esp32 WebServer";
const char *password = "0000000000";
IPAddress local_IP(192, 168, 4, 1);
IPAddress gatway(192, 168, 4, 1);
IPAddress subnet(255, 255, 255, 0);

AsyncWebServer server(80);

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HELGTH, &Wire, OLE_RESET);
bool oledStatus = true;

void HttpGetHome(AsyncWebServerRequest *request) {
  request->send(SPIFFS, "/index.html", "text/html");
}

void HttpPostForward(AsyncWebServerRequest *request) {
  request->send(200);
  Serial.println("forward");
  ledcWrite(0, 255);
  ledcWrite(1, 0);
  ledcWrite(2, 255);
  ledcWrite(3, 0);
  delay(1000);
  ledcWrite(0, 0);
  ledcWrite(1, 0);
  ledcWrite(2, 0);
  ledcWrite(3, 0);
}

void HttpPostBackward(AsyncWebServerRequest *request) {
  request->send(200);
  ledcWrite(0, 0);
  ledcWrite(1, 255);
  ledcWrite(2, 0);
  ledcWrite(3, 255);
  delay(1000);
  ledcWrite(0, 0);
  ledcWrite(1, 0);
  ledcWrite(2, 0);
  ledcWrite(3, 0);
}

void HttpPostStop(AsyncWebServerRequest *request) {
  request->send(200);
  ledcWrite(0, 0);
  ledcWrite(1, 0);
  ledcWrite(2, 0);
  ledcWrite(3, 0);
}

void HttpPostLeft(AsyncWebServerRequest *request) {
  request->send(200);
  ledcWrite(0, 255);
  ledcWrite(1, 0);
  ledcWrite(2, 0);
  ledcWrite(3, 255);
  delay(1000);
  ledcWrite(0, 0);
  ledcWrite(1, 0);
  ledcWrite(2, 0);
  ledcWrite(3, 0);
}

void HttpPostRight(AsyncWebServerRequest *request) {
  request->send(200);
  ledcWrite(0, 0);
  ledcWrite(1, 255);
  ledcWrite(2, 255);
  ledcWrite(3, 0);
  delay(1000);
  ledcWrite(0, 0);
  ledcWrite(1, 0);
  ledcWrite(2, 0);
  ledcWrite(3, 0);
}

void setup() {
  Serial.begin(9600);

  pinMode(MotorA1, OUTPUT);
  pinMode(MotorA2, OUTPUT);
  pinMode(MotorA3, OUTPUT);
  pinMode(MotorA4, OUTPUT);

  ledcSetup(0, 5000, 8);
  ledcAttachPin(MotorA1, 0);
  ledcSetup(1, 5000, 8);
  ledcAttachPin(MotorA2, 1);
  ledcSetup(2, 5000, 8);
  ledcAttachPin(MotorA3, 2);
  ledcSetup(3, 5000, 8);
  ledcAttachPin(MotorA4, 3);

  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    oledStatus = false;
  }

  if(!SPIFFS.begin()) {
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }

  WiFi.softAPConfig(local_IP, gatway, subnet);
  while (!WiFi.softAP(ssid, password)) {}
  Serial.println("softAP Opened.");
  Serial.print("softAPIP: ");
  Serial.println(WiFi.softAPIP());
  if (oledStatus) {
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(WHITE, BLACK);
    display.setCursor(0, 0);
    display.print("softAPIP:");
    display.setCursor(53, 0);
    display.print(WiFi.softAPIP());
    display.display();
  }

  server.on("/", HTTP_GET, HttpGetHome);
  server.on("/forward", HTTP_POST, HttpPostForward);
  server.on("/backward", HTTP_POST, HttpPostBackward);
  server.on("/stop", HTTP_POST, HttpPostStop);
  server.on("/left", HTTP_POST, HttpPostLeft);
  server.on("/right", HTTP_POST, HttpPostRight);

  server.begin();
}

void loop() {}
