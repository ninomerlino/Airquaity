#include <Arduino.h>
#include <WiFi.h>
#include <MQTT.h>

/*namespace Debug {
	//debug stuff
	void Scan() {
		int16_t conn_count = WiFi.scanNetworks();
		switch (conn_count) {
		case WIFI_SCAN_FAILED:
			Serial.println("Failed to scan for networks");
			break;
		case 0:
			Serial.println("No networks found");
			break;
		case WIFI_SCAN_RUNNING:
			Serial.println("Scan is running in async mode");
			break;
		default:
			Serial.printf("Found %d networks:\n", conn_count);
			for (uint8_t i = 0; i < conn_count; i++)
				Serial.printf("\t(%d) : %s\n", i, WiFi.SSID(i).c_str());
			break;
		}
	}
}*/
//Global var
char* ssid = "lmao";
const char* password = "lmao";
const char* mqttId = "veryfastcarwroom";
const char* brokerAddr = "broker.emqx.io";
const int brokerPort = 1883;
WiFiClient socket;
MQTTClient mqtt;
//Network
void ConnectNetwork(int timeout = 60) {
	if (WiFi.status() == WL_CONNECTED)
		WiFi.disconnect();
	WiFi.begin(ssid, password);
	Serial.print("\n(WIFI)Connecting ");
	for (int i = 0; i < timeout * 2; i++) {
		delay(500);
		Serial.print("#");
		if (WiFi.status() == WL_CONNECTED){
			Serial.print('\n');
			return;
		}
	}
	Serial.printf("\nConnection failed with status %d\n", WiFi.status());
}
void ConnectMQTT(int timeout = 60){
	if(mqtt.connected())
		mqtt.disconnect();
	mqtt.begin(brokerAddr, brokerPort, socket);
	Serial.print("(MQTT)Connecting ");
	for (int i = 0; i < timeout * 2; i++) {
		delay(500);
		Serial.print("#");
		if (mqtt.connect(mqttId, mqttId)){
			Serial.print('\n');
			return;
		}
	}
	Serial.println("\nConnection to MQTT failed");
}

void setup() {
	//setup serial
	Serial.begin(9600);
	//setup wifi
	WiFi.mode(WIFI_STA);
	ConnectNetwork();
	//setup mqtt
	ConnectMQTT();
	Serial.println("Setup completed");
}
//TODO: test mqtt
//TODO: set up stream to telegraph
void loop() {
	if(WiFi.status() != WL_CONNECTED)Serial.println("No wifi connection");
	if(!mqtt.connected())Serial.println("No mqtt connection");
	if(!mqtt.publish("poopy/data/cose","it know where it is because it know where it isn't\n")){
		Serial.println("Error while publishing data");
	}
	delay(1000);
}