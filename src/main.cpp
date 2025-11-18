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

void getSpools()
{

	// Serial.println("Receiving spools");

	JsonDocument doc;
	JsonObject obj;

	std::vector<JsonObject> localSpools = spools.getSpoolOrder();

	for (size_t i = 0; i < localSpools.size(); i++)
	{
		Serial.printf("Spool %u:\n", (unsigned)i);
		serializeJsonPretty(localSpools[i], Serial);
		Serial.println();
	}

	// DeserializationError error = deserializeJson(doc, localSpools);
	// if (error)
	// {
	// 	Serial.print(F("deserializeJson() failed: "));
	// 	Serial.println(error.c_str());
	// 	return;
	// }

	// serializeJsonPretty(doc,Serial);
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