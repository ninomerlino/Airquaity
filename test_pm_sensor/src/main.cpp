#include<Arduino.h>
#include<network.hpp>
#include<WiFi.h>

void setup() {
  //serial setup
  Serial.begin(9600);
  Serial.println("Serial setup completed");
  Serial.printf("Wifi lib has found %d networks\n",WiFi.scanNetworks());
  //wifi setup
  Network::Setup("wifiname","wifipassword");
  Network::PrintAvailableNetworks();
  Network::Connect();
}

void loop() {
  if(Network::GetStatus() == WL_CONNECTED){
    Network::PrintNetworkInfo();
  }else{
    Serial.println("Error no connection detected");
    delay(10000);
  }
}