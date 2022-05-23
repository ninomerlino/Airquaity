#include <Arduino.h>
#include <WiFi.h>
#include <Seeed_HM330X.h>
#include <MQTT.h>
#include <sensirion_common.h>
#include <sgp30.h>

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
//WIFI CONNECTION will be replaced by lora network
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
//MQTT my beloved
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
void PublishMQTT(uint16_t* data, uint16_t size){
	if (!mqtt.publish(mqttTopic, (char*)data, size*2)) {
		Serial.println("Error while publishing data");
		if (WiFi.status() != WL_CONNECTED)Serial.println("No wifi connection");
		if (!mqtt.connected())Serial.println("No mqtt connection");
	}
}
//Sensors PM and VOC
HM330XErrorCode ReadSensorPM(uint16_t values[7]) {
	uint8_t checksum = 0;
	uint8_t buffer[30];

	if (pmSensor.read_sensor_value(buffer, 29))
		return ERROR_COMM;
	//Compute checksum
	for (byte i = 0; i < 28; i++)
		checksum += buffer[i];
	//Validate checksum
	if (checksum != buffer[28])
		return ERROR_OTHERS;//invalid checksum
	//Store valid sensor readings
	for (uint8_t i = 2, j = 0; i < 16; i+=2,j++)
		values[j] = (uint16_t)(buffer[i] << 8 | buffer[i + 1]);
	return NO_ERROR;
}

s16 SetupTVOC() {
    s16 err;
    u16 scaled_ethanol_signal, scaled_h2_signal;

	err = sgp_probe();
    if(err != STATUS_OK)return err;

    err = sgp_measure_signals_blocking_read(&scaled_ethanol_signal, &scaled_h2_signal);
	Serial.printf("ethanol: %u, h2: %u\n", scaled_ethanol_signal, scaled_h2_signal);
    if(err != STATUS_OK)return err;
    
    err = sgp_iaq_init();
    return err;
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
	//setup sensor
	if (pmSensor.init()) {
		Serial.println("PM sensor init failed!");
	}
	if(SetupTVOC() != STATUS_OK){
		Serial.println("TVOC sensor init falied!");
	}
}

void loop() {
	uint16_t data[9];
	bool valid = true;
	if (ReadSensorPM(data)){
		Serial.println("Error reading from pm sensor");
		valid = false;
	}
	if(sgp_measure_iaq_blocking_read(data+7,data+8)){
		Serial.println("Error reading from tvoc sensor");
		valid = false;
	}
	if(valid){
		PublishMQTT(data, 9);
	}
	delay(5000);
}