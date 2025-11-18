#include "spools.h"

Spools::Spools() {}

Displays displaySpools;

void Spools::addSpool(int spoolId)
{

	std::string spoolIdStr = std::to_string(spoolId);
	std::string spoolQuery = baseAPI_URL + "spool/" + spoolIdStr;

	// Serial.println(spoolQuery.c_str());

	http.useHTTP10(true);

	// Query spoolman to get the spool with the specified id
	http.begin(wifiClientHttp, spoolQuery.c_str());
	http.GET();

	JsonDocument doc;
	JsonDocument filter;
	DeserializationError error;

	// useFilters = false;

	if (!useFilters)// For debugging when you want to see the full json 
	{
		error = deserializeJson(doc, http.getStream());
	}
	else
	{

		
		filter["id"] = true;
		filter["remaining_weight"] = true;
		filter["location"] = true;
		filter["filament"]["name"] = true;
		filter["filament"]["material"] = true;

		error = deserializeJson(doc, http.getStream(), DeserializationOption::Filter(filter));
	}
	if (error)
	{
		Serial.print(F("deserializeJson() failed: "));
		Serial.println(error.c_str());
		return;
	}


	// serializeJsonPretty(doc, Serial);
	spoolsDocs.push_back(std::move(doc));
	Serial.print("\n\n");

	http.end();
	// delay(100);
}

std::vector<JsonObject> Spools::getSpools()
{

	return spoolsVector;
}

std::vector<JsonObject> Spools::getSpoolOrder()
{

	// Serial.println("getSpoolOrder");
	// Serial.println("Web Socket still connected? " + String(webSocket.isConnected()));
	// Serial.println("HTTP connected? " + String(http.connected()));

	spoolsVector.clear();
	spoolsDocs.clear();

	// bool emptyspoolsVector = spoolsVector.empty();

	http.useHTTP10(true);

	// Query spoolman to get only spools that are in the 'Drybox' location. Should be just4 spools.
	std::string spoolOrderQuery = baseAPI_URL + "setting/locations_spoolorders";

	http.begin(wifiClientHttp, spoolOrderQuery.c_str());
	http.GET();

	// Serial.println("Web Socket connected after GET? " + String(webSocket.isConnected()));
	// Serial.println("HTTP connected after GET? " + String(http.connected()));

	JsonDocument doc;

	JsonDocument filter;
	filter["value"] = true;
	// filter["value"]["Drybox"] = true;

	// DeserializationError error =  deserializeJson(doc, http.getStream(),DeserializationOption::Filter(filter));
	DeserializationError error = deserializeJson(doc, http.getStream());
	if (error)
	{
		Serial.print(F("deserializeJson() failed: "));
		Serial.println(error.c_str());
		// return;
	}

	Serial.println("Value");
	serializeJsonPretty(doc, Serial);
	Serial.println("\n");

	// Extract the inner JSON string
	const char *innerJsonStr = doc["value"];

	// Step 2: Parse the inner JSON string
	JsonDocument innerDoc;

	error = deserializeJson(innerDoc, innerJsonStr);
	if (error)
	{
		Serial.print(F("Inner JSON parse failed: "));
		Serial.println(error.f_str());
		// return;
	}

	// serializeJsonPretty(innerDoc, Serial);
	// Step 3: Access the "Drybox" array
	JsonArray drybox = innerDoc["Drybox"];

	// Serial.println("Drybox \n");
	// serializeJsonPretty(drybox, Serial);

	// Serial.println("Drybox:");
  	// for (int v : drybox) Serial.println(v);	

	// for (int v : innerDoc["Drybox"].as<JsonArray>()) {
    // 	Serial.println(v);
	// 	addSpool(v);
  	// }

	for (int i = 0; i < drybox.size(); i++)
	{
		int spoolId = drybox[i];

		// Serial.println("Push spool " + String(spoolId));
		addSpool(spoolId); // Send to addSpool which builds the vector of ordered spools
		
	}

	// int errorCount = 0;

	// // for (int i : drybox) {
	// for (int i = 0; i < slots; i++)
	// {

	// 	// for (int y : loose)
	// 	// {

	// 	// 	// std::string dryboxId =  std::to_string(i);
	// 	// 	std::string dryboxId = drybox[i];

	// 	// 	std::string loseId = std::to_string(y);
	// 	// 	std::string printMsg = "drybox Id: " + dryboxId + " loose Id: " + loseId;

	// 	// 	Serial.println(printMsg.c_str());

	// 	// 	if (drybox[i] == y)
	// 	// 	{
	// 	// 		errorCount++;
	// 	// 	}
	// 	// 	Serial.println("Error count: " + String(errorCount));
	// 	// 	Serial.println();
	// 	// 	delay(1);
	// 	// }
	// 	if (errorCount == 0)
	// 	{
	// 		int slot = i;
	// 		addSpool(i); // Send to addSpool which builds the vector of ordered spools
	// 		Serial.println("Push spool " + String(i));
	// 		// Serial.println("Error count next: " + errorCount);
	// 		Serial.println();
	// 	}
	// 	else
	// 	{
	// 		errorCount = 0;
	// 	}
	// }

	for (auto &d : spoolsDocs)
	{
		spoolsVector.push_back(d.as<JsonObject>());
		
	}

#ifdef DEBUG
		// --- Iterate safely ---
		// for (size_t i = 0; i < spoolsVector.size(); i++) {
		//   Serial.printf("Spool %u:\n", (unsigned)i);
		//   serializeJsonPretty(spoolsVector[i], Serial);
		//   Serial.println();
		// }

	// displaySpools.overviewDisplay(spoolsVector);
#endif

	http.end();

	return spoolsVector;
	// Serial.println("HTTP still connected after end? " + String(http.connected()));
}

void Spools::updateSpool(int spoolId, int remWeight)
{

	// int vectorSize = spoolsVector.size();

	// for (size_t i = 0; i < vectorSize; i++)
	// {

	// 	if (spoolsVector[i]["id"] == spoolId)
	// 	{

	// 		spoolsVector[i]["remaining_weight"] = remWeight;
	// 		// serializeJsonPretty(spoolsJson, Serial);

	// 		// updateSpoolsJson();

	// 		const char *material = spoolsVector[i]["filament"]["material"];
	// 		const char *name = spoolsVector[i]["filament"]["name"];

	// 		displayArray[i].clearDisplay();
	// 		u8g2_for_adafruit_gfx.begin(displayArray[i]);
	// 		u8g2_for_adafruit_gfx.setFont(u8g2_font_crox2hb_tr); // select u8g2 font from here: https://github.com/olikraus/u8g2/wiki/fntlistall
	// 		u8g2_for_adafruit_gfx.setFontMode(0);				 // use u8g2 transparent mode (this is default)
	// 		u8g2_for_adafruit_gfx.setFontDirection(0);
	// 		// u8g2_for_adafruit_gfx.setForegroundColor(WHITE);

	// 		const char *updateMsg = "UPDATED";

	// 		int16_t text_width = u8g2_for_adafruit_gfx.getUTF8Width(updateMsg);

	// 		int16_t x1, y1;
	// 		uint16_t w, h;
	// 		displayArray[i].getTextBounds(updateMsg, 0, 0, &x1, &y1, &w, &h);

	// 		int16_t text_center_x = disp_center_x - (text_width / 2);
	// 		int16_t text_center_y = disp_center_y + (h / 2);

	// 		u8g2_for_adafruit_gfx.setCursor(text_center_x, text_center_y); // Start at top-left corner
	// 		displayArray[i].invertDisplay(true);
	// 		u8g2_for_adafruit_gfx.print(updateMsg);

	// 		// u8g2_for_adafruit_gfx.print(updateMsg);
	// 		displayArray[i].display();

	// 		delay(3000);

	// 		// displayArray[i].clearDisplay();
	// 		displayArray[i].invertDisplay(false);

	// 		// screenMode = "overview";

	// 		if (screenMode == "spool_weight")
	// 		{

	// 			spoolWeightDisplay();
	// 		}
	// 		else if (screenMode == "overview")
	// 		{

	// 			overviewDisplay();
	// 		}
	// 	}
	// }
	// std::string updateMessage = "Spool " + std::to_string(spoolId) + " updated";
	// // const char * updateMessagePtr
	// mqttClient.publish("mqttStatus", updateMessage.c_str());
}

void Spools::begin()
{
}

Spools::~Spools() {}