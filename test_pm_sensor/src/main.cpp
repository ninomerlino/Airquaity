#include <Arduino.h>
#include <WiFi.h>
#include <Seeed_HM330X.h>
#include <MQTT.h>

#define ssid "Uwu"
#define password "carsound"
#define mqttId "veryfastcarwroom"
#define mqttTopic "poopy/data/cose"
#define brokerAddr "broker.emqx.io"
#define brokerPort 1883

//Global var
WiFiClient socket;
MQTTClient mqtt;
HM330X pmSensor;
//Network
void ConnectNetwork(int timeout = 60) {
	if (WiFi.status() == WL_CONNECTED)
		return;
	WiFi.begin(ssid, password);
	Serial.print("\n(WIFI)Connecting ");
	for (int i = 0; i < timeout * 2; i++) {
		delay(500);
		Serial.print("#");
		if (WiFi.status() == WL_CONNECTED) {
			Serial.print('\n');
			return;
		}
	}
	Serial.printf("\nConnection failed with status %d\n", WiFi.status());
}
void ConnectMQTT(int timeout = 60) {
	if (mqtt.connected())
		return;
	mqtt.begin(brokerAddr, brokerPort, socket);
	Serial.print("(MQTT)Connecting ");
	for (int i = 0; i < timeout * 2; i++) {
		delay(500);
		Serial.print("#");
		if (mqtt.connect(mqttId, mqttId)) {
			Serial.print('\n');
			return;
		}
	}
	Serial.println("\nConnection to MQTT failed");
}
HM330XErrorCode ReadSensorPM(uint16_t values[13]) {
	uint8_t checksum = 0;
	uint8_t buffer[30];

	if (pmSensor.read_sensor_value(buffer, 29))return ERROR_COMM;
	//debug read buffer
	//for (byte i = 0; i < 30; i++)printf("(%u)%u\n", i, buffer[i]);
	for (byte i = 0; i < 28; i++)checksum += buffer[i];
	if (checksum != buffer[28])return ERROR_OTHERS;
	/*			   0  2      4     6    8    10    12    14   16  18  20  22  24  26  28      29 30
	Data structure |XX|Sen N°|PM1.0|PM2.5|PM10|PM1.0|PM2.5|PM10|0.3|0.5|1.0|2.5|5.0|10|Checksum|X|
							 |CF=1 Standar    |Atmospheric     |Number of particle    |
							 |partilculate    |enviroment      |with diameter above   |
							 |matter		  |                |Xum in a liter of air |
							 |				ug/m^3			   |                      |
	*/
	for (uint8_t i = 1; i < 13; i++)values[i - 1] = (uint16_t)(buffer[i * 2] << 8 | buffer[(i * 2) + 1]);
	return NO_ERROR;
}
void PublishMQTT(uint16_t* data){
	if (!mqtt.publish(mqttTopic, (char*)data, 14)) {
		Serial.println("Error while publishing data");
		if (WiFi.status() != WL_CONNECTED)Serial.println("No wifi connection");
		if (!mqtt.connected())Serial.println("No mqtt connection");
	}
}
//fromat data into string message
void setup() {
	//setup serial
	Serial.begin(9600);
	//setup wifi
	WiFi.mode(WIFI_STA);
	ConnectNetwork();
	//setup mqtt
	ConnectMQTT();
	Serial.println("Setup completed");
	//setup sensor
	if (pmSensor.init()) {
		Serial.printf("PM sensor init failed!");
	}
}
//TODO: test pm sensor connection to the db
void loop() {
	uint16_t PMdata[7];
	if (ReadSensorPM(PMdata))
		Serial.println("Error reading from pm sensor");
	else
		PublishMQTT(PMdata);
	delay(5000);
}
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
/*void DebugPrintData(uint16_t d[13]){
	Serial.printf("Sensor N°%u\n"
	"PM1.0 concentration (SP)%uug/m^3 (AT)%uug/m^3\n"
	"PM2.5 concentration (SP)%uug/m^3 (AT)%uug/m^3\n"
	"PM10  concentration (SP)%uug/m^3 (AT)%uug/m^3\n"
	" %u\t0.3um < x < 0.5um\n"
	" %u\t0.5um < x < 1.0um\n"
	" %u\t1.0um < x < 2.5um\n"
	" %u\t2.5um < x < 5.0um\n"
	" %u\t5.0um < x < 10um\n"
	" %u\t10um < x\n",
	d[0],d[1],d[4],d[2],d[5],d[3],d[6],
	d[7],d[8],d[9],d[10],d[11],d[12]
	);
}*/