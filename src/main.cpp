// #include <ArduinoJson.h>
// #include <iostream>
// #include <string>
// #include <vector>

#include "spoolman_server.h"
#include "spools.h"
#include "displays.h"
#include "data.h"
#include <WiFi.h>
// #include "spoolman_server.cpp"
Spools spools;
Displays displays(spools);
Data data (spools, displays);

// TIMER STUFF
unsigned long previousMillis = 0;


void onSlotOrderUpdated(const String& json) {

	displays.stopPageDisplays();
	displays.overviewDisplay();
	// displays.setScreenMode("overview");
	spools.getSlots();
	// displays.resetTimer();
	// displays.setScreenMode("overview");
	
	displays.startPageDisplays();

}

void wsCallback(const char *payload)
{
	Serial.println(payload);

	if (payload == "[WSc] Connected")
	{
		Serial.println("init Displays");
		displays.begin();
		displays.initDisplays();
		spools.initSpools();

		// displays.overviewDisplay();

		// displays.setDisplayPaging(false);
		displays.overviewDisplay();
		displays.startPageDisplays();
	}
}

void setup()
{
	// Serial.begin(230400); 
	data.setWSCallback(wsCallback);
	data.begin();
}

void loop()
{
	data.loop();
	displays.loop();
}