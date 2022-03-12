#include "network.hpp"

char* Network::ssid = NULL;
char* Network::password = NULL;

void Network::Setup(const char *SSID, const char *PASS)
{
  Network::SetSSID(SSID);
  Network::SetPassword(PASS);
  WiFiDrv::wifiDriverInit();
}
void Network::SetSSID(const char *SSID)
{
  if(ssid)
    delete ssid;
  ssid = new char[strlen(SSID) + 1];
  strcpy(ssid, SSID);
}
// set network password
void Network::SetPassword(const char *PASS)
{
  if(password)
    delete password;
  password = new char[strlen(PASS) + 1];
  strcpy(password, PASS);
}
uint8_t Network::GetStatus()
{
  return WiFiDrv::getConnectionStatus();
}
// connect with WPA
void Network::Connect(int attempts, unsigned long timeout)
{
  unsigned long start;
  uint8_t status = WL_IDLE_STATUS;

  Serial.printf("Connecting to %s with %d attempts\n", ssid, attempts);
  for (int i = 0; i < attempts; i++)
  {
    Serial.printf("Attempt n°%d Connecting...",i);
    start = millis();
    // set password and ssid
    if (WiFiDrv::wifiSetPassphrase(ssid, strlen(ssid), password, strlen(password)) == WL_FAILURE)
    {
      Serial.printf("\nX\tAttempt failed on setpassphase");
      status = WL_CONNECT_FAILED; // failed to set password
    }
    else
    {
      while (status != WL_CONNECTED || (millis() - start >= timeout)) // connection loop
      {
        Serial.print(".");
        status = WiFiDrv::getConnectionStatus();
        delay(500);
      }
    }
    if (status == WL_CONNECTED){// connection loop was successfull
      Serial.print("\n✓\tAttempt was successful\n");
      return;
    }
    Serial.print("\n");
  }
  return;
}
void Network::PrintNetworkInfo()
{
  IPAddress ip, gt, ds;
  WiFiDrv::getIpAddress(ip);
  Serial.printf("Network SSID(%s) IP(%s)\n", WiFiDrv::getCurrentSSID(), ip.toString().c_str());
  Serial.printf("\tsignal strenght %d dBm\n", WiFiDrv::getCurrentRSSI());
  Serial.printf("\tEncryption type %d\n", WiFiDrv::getCurrentEncryptionType());
  WiFiDrv::getGatewayIP(gt);
  WiFiDrv::getGatewayIP(ds);
  Serial.printf("\tGateway(%s) DNS(%s)\n", gt.toString().c_str(), ds.toString().c_str());
}
void Network::PrintAvailableNetworks(){
  if(WiFiDrv::startScanNetworks() == WL_FAILURE){
    Serial.print("Cannot scan for networks");
    return;
  }
  uint8_t netitem = 0;
  for (uint8_t i = 0; i < 10; i++){//makes 10 attempts at scanning for networks
    delay(2000);
    netitem = WiFiDrv::getScanNetworks();
    if(netitem == 0)break;
  }
  netitem = WiFiDrv::getScanNetworks();
  Serial.printf("Found %d networks\n", netitem);
  for (uint8_t i = 0; i < netitem; i++){//print all networks info
      Serial.printf("\tNew Network %s\n",WiFiDrv::getSSIDNetoworks(netitem));
  }
  return;
}
void Network::Disconnect(){
  WiFiDrv::disconnect();
}