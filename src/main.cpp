// #include <ArduinoJson.h>
// #include <iostream>
// #include <string>
// #include <vector>


// #include "displays.h"
// Displays displays;

// #include "spools.h"
// Spools spools;

#include "data.h"
Data data;

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