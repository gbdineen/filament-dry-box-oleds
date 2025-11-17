// #include <ArduinoJson.h>
// #include <iostream>
// #include <string>
// #include <vector>


#include "displays.h"
Displays displays;

#include "spools.h"
Spools spools;

#include "data.h"
Data data;

// TIMER STUFF
unsigned long previousMillis = 0;

void wsCallback(const char *payload)
{
	Serial.println(payload);

	if (payload == "[WSc] Connected") {
		// Serial.println("init Displays");
		displays.begin();

		displays.initDisplays();

		spools.getSpoolOrder();
		
	}
}


void setup()
{
	Serial.begin(115200);
	data.setWSCallback(wsCallback);
	data.begin();
}

void loop()
{
	data.loop();
}