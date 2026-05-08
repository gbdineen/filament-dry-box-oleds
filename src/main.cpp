// #include <ArduinoJson.h>
// #include <iostream>
// #include <string>
// #include <vector>

#include "spools.h"
#include "displays.h"
#include "data.h"
#include "spoolman_server.h"
#include <WiFi.h>
// #include "spoolman_server.cpp"
Spools spools;
Displays displays(spools);
Data data (spools, displays);

// TIMER STUFF
unsigned long previousMillis = 0;

void onSlotOrderUpdated(const String& json) {
    // re-drive your display here
    // refreshDisplay();

	Serial.println("Slot Order Updated: " + json);
	
	JsonDocument doc;
    // if (deserializeJson(doc, newOrder) != DeserializationError::Ok) return;

	 DeserializationError error =  deserializeJson(doc, json);
	if (error)
	{
		Serial.print(F("deserializeJson() failed: "));
		Serial.println(error.c_str());
		return;
	}

	//  serializeJsonPretty(doc, Serial);
	
}

void wsCallback(const char *payload)
{
	Serial.println(payload);

	if (payload == "[WSc] Connected")
	{
		Serial.println("init Displays");
		displays.begin();
		// displays.initDisplays();
		spools.initSpools();

		// displays.setDisplayPaging(true);
		// displays.startPageDisplays();
	}
}

void setup()
{
	Serial.begin(230400);
	data.setWSCallback(wsCallback);
	data.begin();
}

void loop()
{
	data.loop();
	displays.loop();
}