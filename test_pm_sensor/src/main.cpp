#include <Arduino.h>
#include <WiFi.h>
#include<utility/wifi_drv.h>
namespace NET {
  void listConnections() {
    uint8_t conn_count = WiFi.scanNetworks();
    switch (conn_count) {
    case WL_FAILURE:
      Serial.println("Failed to scan for networks");
      break;
    case 0:
      Serial.println("No networks found");
      break;
    default:
      Serial.printf("Found %d networks:\n");
      for (uint8_t i = 0; i < conn_count; i++)
        Serial.printf("\t(%d) : %s\n", i, WiFiDrv::getSSIDNetoworks(i));
      break;
    }
  }

  void startConnection(char* ssid, const char* pass) {
    if (WiFi.status() == WL_CONNECTED)
      WiFi.disconnect();
    if (WiFi.begin(ssid, pass) == WL_CONNECTED) {
      Serial.println("Connection was successful");
    } else {
      Serial.println("Connection failed");
    }
  }
}

void setup() {
  // serial setup
  Serial.begin(9600);
  Serial.printf("|%s|\n", WiFi.firmwareVersion());
  NET::listConnections();
  NET::startConnection("Uwu", "carsound");
}

void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("WoW very much connection so much internet");
  } else {
    Serial.println("Error no connection detected");
  }
  delay(10000);
}