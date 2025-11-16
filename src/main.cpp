// #include <ArduinoJson.h>
// #include <iostream>
// #include <string>
// #include <vector>


// #include "displays.h"
// Displays displays;

<<<<<<< HEAD
// #include "spools.h"
// Spools spools;

#include "data.h"
Data data;
=======
#include <ArduinoJson.h>
#include <iostream>
#include <string>
#include <vector>
// #include <functional>

// CUSTOM CLASSES AND HEADER
// #include "op_logo.h"
// #include "displays.h"
// #include "spools.h"
#include "data.h"
Data data;

// #define USE_SERIAL Serial
// #define WIFI_un "GL-SFT1200-887"
// #define WIFI_pw "goodlife"
// #define DEBUG // Uncomment to include verbose serial responses (json, etc)
// #define SCREEN_WIDTH 128 // OLED display width, in pixels
// #define SCREEN_HEIGHT 64 // OLED display height, in pixels
// #define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
// #define SCREEN_ADDRESS_0 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
// #define SCREEN_ADDRESS_1 0x3D
// #define SCREEN_ADDRESS_2 0x3C
// #define SCREEN_ADDRESS_3 0x3D
// #define I2C0_SDA 9 // &Wire Bus (default )
// #define I2C0_SCL 8
// #define I2C1_SDA 5  // &I2C_Bus1
// #define I2C1_SCL 4
// #define WIRE Wire
// #define LOGO_HEIGHT   16
// #define LOGO_WIDTH    16
// #define HOST_IP "192.168.8.228"
// #define WS_PORT 7912
// #define MQTT_PORT 1883

// const char * hostIP = "192.168.8.228";
// std::string  baseAPI_URL = "http://192.168.8.228:7912/api/v1/";
// int wsPort = 7912;

// // TwoWire  I2C_Bus0 = TwoWire(0);
// // TwoWire  I2C_Bus1 = TwoWire(1);

// const char* ssid = "GL-SFT1200-887";
// const char* password = "goodlife";
// // bool setMaxPacketSize(3000);

// // MQTT broker details
// const char* mqtt_broker = hostIP;
// const int mqtt_port = 1883; // Or 8883 for SSL/TLS
// const char* mqtt_client_id = "drybox_oleds";
// const char* mqttUN = "gbdineen";
// const char* mqttPW = "N1mbl3Sh@rk";
// int lastReconnectAttempt = 0;
// bool firstStart = true;

// // JSON STUFF
// JsonDocument spoolsJson;
// bool useFilters = true;
// // std::vector<JsonObject> spoolsVector;
// std::vector<JsonDocument> spoolsDocs;   // holds real storage
// std::vector<JsonObject> spoolsVector;   // holds lightweight views

// WiFiClient wifiClient;
// WiFiClient wifiClientHttp;
// // WiFiClient wifiMQTT;
// // WiFiClient wifiWS;
// HTTPClient http;
// WebSocketsClient webSocket;
// PubSubClient mqttClient(wifiClient);

// DISPLAY STUFF.
// DisplayControl displayControl;
// Spools spools;

// int slots = 4;  // Number of spool slots, also happens to be the number of displays
// std::string screenMode = "overview";
// uint8_t disp_w = 128;
// uint8_t disp_h = 64;
// uint8_t disp_center_x = disp_w/2;
// uint8_t disp_center_y = disp_h/2;
// uint8_t font_height = 10;
// uint8_t padding_screen_top = 10;
// uint8_t padding_screen_left = 10;
// uint8_t padding_font_bottom = 7;
// uint8_t character_height = font_height + padding_font_bottom;
// bool pageDisplays = false; // Rotate through screen views
// void overviewDisplay();
>>>>>>> 9b14b09 (data flow up and running. moving on to displays)

// TIMER STUFF
unsigned long previousMillis = 0;


void setup()
{
	Serial.begin(115200);

	data.begin();

	// I2C_Bus0.begin(I2C0_SDA, I2C0_SCL, 100000);
	// I2C_Bus1.begin(I2C1_SDA, I2C1_SCL, 100000);

	// WiFi.begin(ssid, password);
	// while (WiFi.status() != WL_CONNECTED) {
	//   delay(1000);
	//   Serial.println("Connecting to WiFi...");
	// }
	// Serial.println("Connected to WiFi");

	// webSocket.begin(hostIP, wsPort, "/api/v1/");
	// webSocket.onEvent(webSocketEvent);
	// webSocket.setReconnectInterval(5000);

	// // Start up MQTT.

	// // Set these first so they don't get missed before actual connection
	// std::string clientIdStr = mqtt_client_id;
	// std::string connectMsg = "MQTT client " + clientIdStr + " connected";
	// mqttClient.setServer(mqtt_broker, mqtt_port);
	// mqttClient.setCallback(mqttCallback);

	// if (mqttClient.connect(mqtt_client_id, mqttUN, mqttPW)) {
	//   mqttClient.publish("mqttStatus",connectMsg.c_str());
	//   // ... and resubscribe
	//   mqttClient.subscribe("octoPrint/event/plugin_Spoolman_spool_selected");
	//   mqttClient.subscribe("octoPrint/event/plugin_Spoolman_spool_usage_committed");
	// }

	// mqttReconnect();
	// while(!mqttClient.connected()){
	//   mqttClient.connect(mqtt_client_id, mqttUN, mqttPW);
	// }

	// Set initial subs and post a heatlh message
	// mqttReconnect();

	// Start up web socket.
	// There seems to be no way to pause for the WS connection so I hope it has
	// some sort of blocking to hold the rest before it's reasdy

	// initDisplays();

	// First let's get the order to put the spools in
	// getSpoolOrder();

	// Get initial spool data from spoolman
	// getSpools();
}

void loop()
{
	data.loop();
	// if (!mqttClient.connected()) {
	//   Serial.print("MQTT Disconnected");
	//   long now = millis();
	//   if (now - lastReconnectAttempt > 5000) {
	//     lastReconnectAttempt = now;
	//     // Attempt to reconnect
	//     if (mqttReconnect()) {
	//       lastReconnectAttempt = 0;
	//     }
	//   }
	// } else {
	//   // Client connected
	//   mqttClient.loop();
	// }
	// // mqttClient.loop();
	// webSocket.loop();
}