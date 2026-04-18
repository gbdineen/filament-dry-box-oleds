#include "spools.h"



Spools::Spools() 
	// : spoolsOrder{}
{}

void Spools::getSpool(int &spoolId)
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


void Spools::addSpool(int &spoolId)
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
	Serial.print("\n\n");

	http.end();
	// delay(100);
}

void Spools::deleteSpool(int &spoolId) {

	spoolsVector.erase(std::find(spoolsVector.begin(), spoolsVector.end(), spoolId));
	// spoolsOrderVector.erase(std::find(spoolsOrderVector.begin(), spoolsOrderVector.end(), spoolId)); 
	// pushUpdatedSpoolsOrder();
}

std::vector<JsonDocument>& Spools::getSpools()
{

	// std::cout << "Spools Address of spoolsVector: " << &spoolsVector << std::endl;

	return spoolsVector;

}

void Spools::getDryboxSpools() {

	spoolsVector.clear();

	http.useHTTP10(true);

	std::string spoolQuery = baseAPI_URL + "spool";

	http.begin(wifiClientHttp, spoolQuery.c_str());
	http.GET();

	JsonDocument outerDoc;
	// JsonDocument filter;
	// filter["Drybox"] = true;

	// DeserializationError error =  deserializeJson(outerDoc, http.getStream(),DeserializationOption::Filter(filter));
	DeserializationError error = deserializeJson(outerDoc, http.getStream());
	if (error)
	{
		Serial.print(F("deserializeJson() failed: "));
		Serial.println(error.c_str());
		// return;
	}

	// serializeJsonPretty(outerDoc, Serial);
	// Serial.println("\n\n\n");

	// JsonArray dryboxSpools = outerDoc.as<JsonArray>();

	for (int i=0; i < outerDoc.size(); i++) {

		// Serial.println("Item" + i);
		if (outerDoc[i]["location"] == "Drybox") {
			serializeJsonPretty(outerDoc[i], Serial);
			spoolsVector.push_back(outerDoc[i]);
		}

	}
}

void Spools::initSpools() {

	Serial.print("init spools");

	getDryboxSpools();

}

void Spools::getSpoolsOrder() {

	spoolsOrderVector.clear();

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
    	Serial.println(v);
		spoolsOrderVector.push_back(std::move(v));
  	}

	// std::vector<int> *ptr;
	// ptr = &spoolsOrder;

	// std::cout << "Address of myVariable (using &spoolsOrder): " << &spoolsOrder << std::endl;

	// return spoolsOrderVector;

}

void Spools::refactorSpoolsOrder() {

	int spoolsVectorSize = spoolsVector.size();
	int spoolsOrderVectorSize = spoolsOrderVector.size();
	
	for (int d=0; d<spoolsVectorSize; d++) {

		for (int i=0; i<spoolsOrderVectorSize; i++) {

			if(spoolsOrderVector[i] != spoolsVector[i]["id"]) {

				spoolsOrderVector.erase(std::find(spoolsOrderVector.begin(), spoolsOrderVector.end(), spoolsOrderVector[i])); 
			
			}

		}

	}

	pushUpdatedSpoolsOrder();

}

void Spools::pushUpdatedSpoolsOrder() {   // Likely deprecated after new Refine locations mamangement app    

	
	std::string ordersStr = "";

	ordersStr += std::to_string(spoolsOrderVector[0]);

	for (int i=1; i<spoolsOrderVector.size(); i++) {

		ordersStr += "," + std::to_string(spoolsOrderVector[i]);

	}

	// Serial.println(ordersStr.c_str());

	std::string spoolOrderPost = baseAPI_URL + "setting/locations_spoolorders";
	http.begin(wifiClientHttp, spoolOrderPost.c_str());
	http.addHeader("Content-Type", "application/json");

	std::string payload = "\"{\\\"Loose\\\":[5,6,3],"
    "\\\"Drybox\\\":[" + ordersStr + "],"
    "\\\"Archived\\\":[7,1]}\"";

	int httpCode = http.POST(payload.c_str());

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
			
		}
	}
	// std::string updateMessage = "Spool " + std::to_string(sid) + " updated";
	// const char * updateMessagePtr
	// mqttClient.publish("mqttStatus", updateMessage.c_str());
}

void Spools::begin()
{
}

Spools::~Spools() {}