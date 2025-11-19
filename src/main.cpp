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

	// Serial.println("Receiving spools");

	JsonDocument doc;
	JsonObject obj;

	// spoolsVector = spools.getSpools();

	// Just for testing 
	// for (size_t i = 0; i < localSpools.size(); i++)
	// {
	// 	// Serial.printf("Spool %u:\n", (unsigned)i);
	// 	serializeJsonPretty(localSpools[i], Serial);
	// 	Serial.println();
	// }



	// displays.overviewDisplay(std::move(localSpools));

}

void wsCallback(const char *payload)
{
	Serial.println(payload);

	if (payload == "[WSc] Connected")
	{
		// Serial.println("init Displays");
		displays.begin();
		displays.initDisplays();

		getSpools();

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