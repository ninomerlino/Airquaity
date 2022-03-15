#include <Arduino.h>
#include <WiFi.h>
namespace NET {
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
	//usefull stuff
	void Connect(char* ssid, const char* pass, int timeout = 60) {
		WiFi.mode(WIFI_STA);
		if (WiFi.status() == WL_CONNECTED)
			WiFi.disconnect();
		WiFi.begin(ssid, pass);
		Serial.print("Connecting ");
		for (int i = 0; i < timeout * 2; i++) {
			delay(500);
			Serial.print("#");
			if (WiFi.status() == WL_CONNECTED)break;
		}
		if (WiFi.status() == WL_CONNECTED)
			Serial.println("\nConnection was successful");
		else
			Serial.printf("\nConnection failed with status %d\n", WiFi.status());
	}
}

void setup() {
	// serial setup
	Serial.begin(9600);
	Serial.println("");
	NET::Scan();
	NET::Connect("Uwu", "carsound");
}

void loop() {
	delay(10000);
}