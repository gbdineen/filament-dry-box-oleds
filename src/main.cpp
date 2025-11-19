// #include <ArduinoJson.h>
// #include <iostream>
// #include <string>
// #include <vector>

#include "spools.h"
#include "displays.h"
#include "data.h"
Spools spools;
Displays displays(spools);
Data data;

// TIMER STUFF
unsigned long previousMillis = 0;

	// std::vector<JsonObject> &spoolsVector;

void getSpools()
{


}

void wsCallback(const char *payload)
{
	Serial.println(payload);

	if (payload == "[WSc] Connected")
	{
		// Serial.println("init Displays");
		// displays.begin();
		displays.initDisplays();
		spools.initSpools();

		displays.pageDisplays();

		// displays.overviewDisplay();
		// delay(3000);
		// displays.spoolWeightDisplay();

		// getSpools();

		// spools.getSpoolOrder();
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