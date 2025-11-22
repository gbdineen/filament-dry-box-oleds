#include "spools.h"



Spools::Spools() 
	// : spoolsOrder{}
{}

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


	serializeJsonPretty(doc, Serial);

	const char* location = doc["location"];
	if (doc["location"] == "Drybox") {
		spoolsDocs.push_back(std::move(doc));
	}
	// Serial.print("\n\n");

	http.end();
	// delay(100);
}

std::vector<JsonDocument>& Spools::getSpools()
{
	return spoolsVector;

}

void Spools::getSpool(int spoolId)
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
	// DeserializationError error;

	filter["id"] = true;
	filter["remaining_weight"] = true;
	filter["location"] = true;
	filter["filament"]["name"] = true;
	filter["filament"]["material"] = true;

	DeserializationError error = deserializeJson(doc, http.getStream(), DeserializationOption::Filter(filter));
	if (error)
	{
		Serial.print(F("deserializeJson() failed: "));
		Serial.println(error.c_str());
		return;
	}

	http.end();

	// JsonDocument output;
  	// // // serializeJson(doc, output);
	// JsonDocument *ptr = nullptr;
	// *ptr = &doc;

	// return ptr;
}

void Spools::initSpools() {

	getSpoolsOrder();

	spoolsVector.clear();
	spoolsDocs.clear();


	// const std::vector<int> orderVector = getSpoolsOrder();

	for (int d : spoolsOrderVector) {
	// for (int i=0; i<orderArray.size(); i++) {

		// int oId = orderArray[i];
		std::string oStr = std::to_string(d);
		std::string msg = "Spools in order array: " + oStr;

		Serial.println(msg.c_str());

		http.useHTTP10(true);

		// Query spoolman to get only spools that are in the 'Drybox' location. Should be just4 spools.
		
		std::string spoolOrderQuery = baseAPI_URL + "spool/" + oStr;

		http.begin(wifiClientHttp, spoolOrderQuery.c_str());
		http.GET();

		JsonDocument doc;
		JsonDocument filter;

		filter["id"] = true;
		filter["remaining_weight"] = true;
		filter["location"] = true;
		filter["filament"]["name"] = true;
		filter["filament"]["material"] = true;

		DeserializationError error = deserializeJson(doc, http.getStream(), DeserializationOption::Filter(filter));
		if (error)
		{
			Serial.print(F("deserializeJson() failed: "));
			Serial.println(error.c_str());
			return;
		}

		// serializeJsonPretty(doc, Serial);

		if (doc["id"] == d && doc["location"] == "Drybox") {

			spoolsVector.push_back(std::move(doc));

		}  else {

			spoolsOrderVector.erase(std::find(spoolsOrderVector.begin(), spoolsOrderVector.end(), d)); 

		}

	}
	http.end();

	for (size_t i = 0; i < spoolsVector.size(); i++) {
		Serial.printf("Spool %u:\n", (unsigned)i);
		serializeJsonPretty(spoolsVector[i], Serial);
		Serial.println();
	}

	for (size_t t = 0; t < spoolsOrderVector.size(); t++) {
		int vid = spoolsOrderVector[t];
		Serial.printf("Spool %u:\n", vid);
		Serial.println();
	}

	
	std::string spoolOrderPost = baseAPI_URL + "setting/locations_spoolorders";
	http.begin(wifiClientHttp, spoolOrderPost.c_str());
	http.addHeader("Content-Type", "application/json");



	String payload =
    "\"{\\\"Loose\\\":[5,6,3],"
    "\\\"Drybox\\\":[2,8,4],"
    "\\\"Archived\\\":[7,1]}\"";

	int httpCode = http.POST(payload);

	Serial.printf("HTTP POST returned code: %d\n", httpCode);

	if (httpCode > 0) {
		String response = http.getString();
		Serial.println("Response:");
		Serial.println(response);
	} else {
		Serial.printf("POST failed: %s\n", http.errorToString(httpCode).c_str());
	}



}

void Spools::initSpoolsDEP2()
{

	spoolsVector.clear();
	spoolsDocs.clear();


	// const std::vector<int> orderArray = getSpoolsOrder();

	// for (auto &v : orderArray) {
	// 	Serial.println("Spools in order array: " + String(v));
	// }

	http.useHTTP10(true);

	// Query spoolman to get only spools that are in the 'Drybox' location. Should be just4 spools.
	std::string spoolOrderQuery = baseAPI_URL + "spool?location=Drybox";

	http.begin(wifiClientHttp, spoolOrderQuery.c_str());
	http.GET();

	JsonDocument docGET;
		// DeserializationError error =  deserializeJson(obj, innerDoc,DeserializationOption::Filter(filter));
	DeserializationError error = deserializeJson(docGET, http.getStream());
	if (error)
	{
		Serial.print(F("deserializeJson() failed: "));
		Serial.println(error.c_str());
		// return;
	}

	// serializeJsonPretty(docGET, Serial);
	// Serial.println("\n");	

	JsonArray docAry = docGET.as<JsonArray>();

	for (JsonObject obj : docAry) {

		String innerStr; 
		serializeJson(obj,innerStr);

		JsonDocument innerDoc;
		JsonDocument filter;
		filter["id"] = true;
		filter["remaining_weight"] = true;
		filter["location"] = true;
		filter["filament"]["name"] = true;
		filter["filament"]["material"] = true;

		DeserializationError error =  deserializeJson(innerDoc, innerStr,DeserializationOption::Filter(filter));
		// DeserializationError error = deserializeJson(docGET, http.getStream());
		if (error)
		{
			Serial.print(F("deserializeJson() failed: "));
			Serial.println(error.c_str());
			// return;
		}

		// serializeJsonPretty(innerDoc,Serial);
		// Serial.println("\n\n");

		// int o = orderArray.size();
		// int s = docAry.size();

		// if (o > s ) {

		// 	for (auto &j : orderArray) 
		// 	{
		// 		spoolsVector.push_back(std::move(innerDoc));
		// 		// if (orderArray[i] == innerDoc["id"]) {

		// 		// 	int id = innerDoc["id"];

		// 		// 	Serial.print(orderArray[i]); Serial.println(id);

		// 		// 	// spoolsVector.push_back(std::move(innerDoc));

		// 		// 	// Serial.print(innerDoc["id"]);

		// 		// }
		// 	}
		// } 
		// else if (s > 0 ){

		// 	for (auto d : obj) {
				
		// 		if (innerDoc["id"] == d) {

		// 			spoolsVector.push_back(std::move(innerDoc));

		// 		}
		// 	}

		// }


		/*-------------------------
		if (innerDoc["location"] == "Drybox"){

			// spoolsVector.push_back(innerDoc);
			// serializeJsonPretty(innerDoc,Serial);
			// Serial.println("\n");

			int orderSize = order.size();
			int spoolsSize = spoolsVector.size();

			if (orderSize > spoolsSize){

				Serial.println("Order Array is greater");

				for (int i=0; i<orderSize; i++) 
				{
					int o = order[i];
					int s = spoolsVector.size();

					std::string oS = std::to_string(o);
					std::string oMsg = "Order Id: " + oS;

					std::string sS = std::to_string(s);
					std::string sMsg = "Spools Id: " + sS + " " + oMsg;

					
					Serial.println(sMsg.c_str());
						
				}
				Serial.println("\n\n");

			} else if (orderSize<spoolsSize) {

				Serial.println("Spools Array is greater");

				for (int j=0; j<spoolsSize; j++) 
				{

					int o = order[j];
					int s = spoolsVector.size();

					Serial.print("Order Id: " + o); Serial.println(" Spool Id is: " + s);;
				}

			} else {
				Serial.println("Arrays are equal");
			}

			// for (int i=0; i<order.size(); i++) {
			// 	if (order[i] == innerDoc["location"]["Drybox"][i]) {

			// 		spoolsVector.push_back(innerDoc[i]);
			// 		serializeJsonPretty(spoolsVector[i],Serial);
			// 	}
			// }
		}
			*/

	}

	http.end();

	for (size_t i = 0; i < spoolsVector.size(); i++) {
		Serial.printf("Spool %u:\n", (unsigned)i);
		serializeJsonPretty(spoolsVector[i], Serial);
		Serial.println();
	}




	// for (int i=0; i<innerDoc.size(); i++) {

	// 	if (innerDoc[i]["location"] == "Drybox") {

	// 		spoolsVector.push_back(std::move(innerDoc[i]));

	// 	}

	// }
	// serializeJsonPretty(docGET, Serial);
	// Serial.println("\n");



	// http.end();



	std::string spoolOrderPost = baseAPI_URL + "setting/locations_spoolorders";
	http.begin(wifiClientHttp, spoolOrderPost.c_str());
	http.addHeader("Content-Type", "application/json");
	

	String payload =
    "\"{\\\"Loose\\\":[5,6,3],"
    "\\\"Drybox\\\":[2,8,4],"
    "\\\"Archived\\\":[7,1]}\"";

	int httpCode = http.POST(payload);

	Serial.printf("HTTP POST returned code: %d\n", httpCode);

	if (httpCode > 0) {
		String response = http.getString();
		Serial.println("Response:");
		Serial.println(response);
	} else {
		Serial.printf("POST failed: %s\n", http.errorToString(httpCode).c_str());
	}


	http.end();


}

void Spools::getSpoolsOrder() {

	http.useHTTP10(true);

	// // Query spoolman to get only spools that are in the 'Drybox' location. Should be just4 spools.
	std::string spoolOrderQuery = baseAPI_URL + "setting/locations_spoolorders";

	http.begin(wifiClientHttp, spoolOrderQuery.c_str());
	http.GET();

	JsonDocument docGET;

	JsonDocument filter;
	filter["value"] = true;
	// filter["value"]["Drybox"] = true;

	DeserializationError error =  deserializeJson(docGET, http.getStream(),DeserializationOption::Filter(filter));
	// DeserializationError error = deserializeJson(docGET, http.getStream());
	if (error)
	{
		Serial.print(F("deserializeJson() failed: "));
		Serial.println(error.c_str());
		// return;
	}


	// Extract the inner JSON string
	const char *innerJsonStr = docGET["value"];

	// Step 2: Parse the inner JSON string
	JsonDocument innerDoc;
	JsonDocument innerFilter;

	filter["Drybox"] = true;

	error = deserializeJson(innerDoc, innerJsonStr,DeserializationOption::Filter(filter));
	if (error)
	{
		Serial.print(F("Inner JSON parse failed: "));
		Serial.println(error.f_str());
		// return;
	}

	serializeJsonPretty(innerDoc, Serial);
	Serial.println("\n");
	// Step 3: Access the "Drybox" array
	JsonArray drybox = innerDoc["Drybox"];

	// std::vector<int> spoolOrder;

	for (int v : drybox) {
    	// Serial.println(v);
		spoolsOrderVector.push_back(std::move(v));
  	}

	// std::vector<int> *ptr;
	// ptr = &spoolsOrder;

	// std::cout << "Address of myVariable (using &spoolsOrder): " << &spoolsOrder << std::endl;

	// return spoolsOrderVector;

}

void Spools::initSpoolsDEP()
{

	// Serial.println("getSpoolOrder");
	// Serial.println("Web Socket still connected? " + String(webSocket.isConnected()));
	// Serial.println("HTTP connected? " + String(http.connected()));

	spoolsVector.clear();
	spoolsDocs.clear();

	// bool emptyspoolsVector = spoolsVector.empty();

	// http.useHTTP10(true);

	// // Query spoolman to get only spools that are in the 'Drybox' location. Should be just4 spools.
	// std::string spoolOrderQuery = baseAPI_URL + "setting/locations_spoolorders";

	// http.begin(wifiClientHttp, spoolOrderQuery.c_str());
	// http.GET();

	// // Serial.println("Web Socket connected after GET? " + String(webSocket.isConnected()));
	// // Serial.println("HTTP connected after GET? " + String(http.connected()));

	// JsonDocument docGET;

	// JsonDocument filter;
	// filter["value"] = true;
	// // filter["value"]["Drybox"] = true;

	// // DeserializationError error =  deserializeJson(doc, http.getStream(),DeserializationOption::Filter(filter));
	// DeserializationError error = deserializeJson(docGET, http.getStream());
	// if (error)
	// {
	// 	Serial.print(F("deserializeJson() failed: "));
	// 	Serial.println(error.c_str());
	// 	// return;
	// }

	// // Serial.println("Value");
	// serializeJsonPretty(docGET, Serial);
	// Serial.println("\n");

	// http.end();

	// JsonDocument doc;

	// doc["key"] = "locations_spoolorders";

	// JsonObject setting = doc["setting"].to<JsonObject>();

	// String valueStr = "{\"Loose\":[5,7,6,3],\"Drybox\":[2,8,4],\"Archived\":[4,7,8]}";

	// JsonObject setting_value = setting["value"].to<JsonObject>();

	// setting["value"] = valueStr;



	// JsonArray setting_value_Loose = setting_value["Loose"].to<JsonArray>();
	// setting_value_Loose.add(5);
	// setting_value_Loose.add(7);
	// setting_value_Loose.add(6);
	// setting_value_Loose.add(3);

	// JsonArray setting_value_Drybox = setting_value["Drybox"].to<JsonArray>();
	// setting_value_Drybox.add(4);
	// setting_value_Drybox.add(8);
	// setting_value_Drybox.add(2);

	// JsonArray setting_value_Archived = setting_value["Archived"].to<JsonArray>();
	// setting_value_Archived.add(4);
	// setting_value_Archived.add(7);
	// setting_value_Archived.add(8);
	// setting["is_set"] = true;
	// setting["type"] = "object";

	std::string spoolOrderPost = baseAPI_URL + "setting/locations_spoolorders";
	http.begin(wifiClientHttp, spoolOrderPost.c_str());
	http.addHeader("Content-Type", "application/json");

	String payload =
    "\"{\\\"Loose\\\":[5,6,3],"
    "\\\"Drybox\\\":[2,8,4],"
    "\\\"Archived\\\":[7,1]}\"";

	int httpCode = http.POST(payload);

	Serial.printf("HTTP POST returned code: %d\n", httpCode);

	if (httpCode > 0) {
		String response = http.getString();
		Serial.println("Response:");
		Serial.println(response);
	} else {
		Serial.printf("POST failed: %s\n", http.errorToString(httpCode).c_str());
	}


	// String json;
	// serializeJson(doc, json);
	// http.POST(json);

	// Serial.print(http.getString());


	http.end();


	/*---------------------------/

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

	*/

	// Serial.println("Drybox \n");
	// serializeJsonPretty(drybox, Serial);

	// Serial.println("Drybox:");
  	// for (int v : drybox) Serial.println(v);	

	// for (int v : innerDoc["Drybox"].as<JsonArray>()) {
    // 	Serial.println(v);
	// 	addSpool(v);
  	// }

	/*

	for (int i = 0; i < drybox.size(); i++)
	{
		int spoolId = drybox[i];

		// Serial.println("Push spool " + String(spoolId));
		addSpool(spoolId); // Send to addSpool which builds the vector of ordered spools
		
	}

	for (auto &d : spoolsDocs)
	{
		spoolsVector.push_back(d);
		
	}

	if(debug){
		// --- Iterate safely ---
		for (size_t i = 0; i < spoolsVector.size(); i++) {
		  Serial.printf("Spool %u:\n", (unsigned)i);
		  serializeJsonPretty(spoolsVector[i], Serial);
		  Serial.println();
		}
	}

	*/



	// return spoolsVector;
	// Serial.println("HTTP still connected after end? " + String(http.connected()));
}

/****************************************/
/*!
	@brief SpoolId, weight, material, name
	@param sid	SpoolId
	@param w 	Spool weight
	@param m	Spool material
	@param n 	Spool name
*/
/*****************************************/
void Spools::getSpoolInfo(int *sid, int *w, const char* *m, const char* *n) { 


}

int Spools::getSpoolsCount() {
	return spoolsVector.size();
}

/****************************************/
/*!
	@brief SpoolId, weight
	@param sid	SpoolId
	@param w 	Spool weight
	@param m	Spool material
	@param n 	Spool name
	@param d 	Id of display to update

*/
/*****************************************/
void Spools::updateSpool(int& sid, int& w, const char* &m, const char* &n, int* d)
{

	int vectorSize = spoolsVector.size();

	for (int i = 0; i < vectorSize; i++)
	{

		if (spoolsVector[i]["id"] == sid)
		{


			spoolsVector[i]["remaining_weight"] = w;
			spoolsVector[i]["material"] = m;
			spoolsVector[i]["name"] = n;
			*d = i;
			// // serializeJsonPretty(spoolsJson, Serial);
		}
	}
	// std::string updateMessage = "Spool " + std::to_string(spoolId) + " updated";
	// // const char * updateMessagePtr
	// mqttClient.publish("mqttStatus", updateMessage.c_str());
}

void Spools::begin()
{
}

Spools::~Spools() {}