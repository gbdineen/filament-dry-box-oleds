#include "spools.h"



Spools::Spools() 
	// : spoolsOrder{}
{}

void Spools::initSpools() {

	getSpoolsOrder();
	loadSpools();
}	


void Spools::getSpoolsOrder() {

	spoolsOrderVector.clear();

	http.useHTTP10(true);

	// Query spool manager to get spool IDs in slot order
	std::string spoolOrderQuery = spoolsOrderServer_URL + "/slotorder";

	http.begin(wifiClientHttp, spoolOrderQuery.c_str());
	http.GET();

	JsonDocument doc;

	// DeserializationError error =  deserializeJson(docGET, http.getStream(),DeserializationOption::Filter(filter));
	DeserializationError error = deserializeJson(doc, http.getStream());
	if (error)
	{
		Serial.print(F("deserializeJson() failed: "));
		Serial.println(error.c_str());
		// return;
	}

	// serializeJsonPretty(doc,Serial);

	JsonArray spoolsOrderArray = doc.as<JsonArray>();

	for (int v : spoolsOrderArray) {
		spoolsOrderVector.push_back(v);
		std::cout << "Initial Spools Order: " << v << std::endl;
	}

}

void Spools::setSpoolsOrder(const std::vector<int>& newOrder) {
    spoolsOrderVector = newOrder;  // copies into the member

	for (int v : spoolsOrderVector) {
		// spoolsOrderVector.push_back(v);
		std::cout << "New Spools Order: " << v << std::endl;
	}gc

}



void Spools::loadSpools() {

	spoolsVector.clear();

	http.useHTTP10(true);

	std::string spoolQuery = baseAPI_URL + "spool?location=Drybox";

	http.begin(wifiClientHttp, spoolQuery.c_str());
	// http.GET();
	int httpCode = http.GET();
	
	String payload = http.getString();

	JsonDocument doc;
	JsonDocument filter;

	filter[0]["id"] = true;
	filter[0]["location"] = true;
	filter[0]["remaining_weight"] = true;
	filter[0]["filament"]["name"] = true;
	filter[0]["filament"]["material"] = true;

	DeserializationError error =  deserializeJson(doc, payload, DeserializationOption::Filter(filter));
	// DeserializationError error = deserializeJson(doc, http.getStream());
	if (error)
	{
		Serial.print(F("deserializeJson() failed: "));
		Serial.println(error.c_str());
		return;
	}

	// serializeJsonPretty(doc, Serial);


	JsonArray spoolsArray = doc.as<JsonArray>();

	
	for (int i=0; i < spoolsOrderVector.size(); i++)
	{

		for (int x=0; x < spoolsArray.size(); x++)
		{
			 JsonDocument spoolDoc;
       		 spoolDoc.set(spoolsArray[x]);  
			
			if (spoolsArray[x]["id"] == spoolsOrderVector[i])
			{
				// std::cout << "spoolsArray X: " << spoolsArray[x]["id"] << std::endl;
				// std::cout << "Slot ID: " << spoolsOrderVector[i] << std::endl;
				spoolsVector.push_back(std::move(spoolDoc));
				// serializeJsonPretty(spoolsArray[x], Serial);
				std::cout << "\n\n" << std::endl;

			}

		}

	}

	// for (JsonDocument v : spoolsVector) 
	// {
	// 	std::cout << v << std::endl;
	// }
	
	
	// for (JsonObject spool : spoolsArray) {
    //     JsonDocument spoolDoc;
    //     spoolDoc.set(spool);              // Deep copy each object
		
	// 	std::cout << "spool[id]: " << spool["id"] << "\n" <<std::endl;

	// 	// for (int i = 0; i < spoolsOrderVector.size(); i++) {

	// 	// 	std::cout << "spoolDoc[id]: " << spoolDoc["id"] << std::endl;
	// 	// 	std::cout << "spoolsOrderVector[i]: " << spoolsOrderVector[i] << std::endl;
			
	// 	// 	if (spoolsOrderVector[i] == spoolDoc["id"]) {
	// 	// 		spoolsVector.push_back(std::move(spoolDoc)); // Move into vector
	// 	// 		serializeJsonPretty(spool, Serial);
	// 	// 	}
	// 	// }
		
	// 	// for (int v : spoolsOrderVector)
	// 	// {
	// 	// 	std::cout << "spoolDoc[\"id\"]: " << spoolDoc["id"] << std::endl;
	// 	// 	if (spoolDoc["id"] == v)
	// 	// 	{
	// 	// 		spoolsVector.push_back(std::move(spoolDoc)); // Move into vector
	// 	// 		serializeJsonPretty(spool, Serial);
	// 	// 	}

	// 	// }

    // }


	// std::string innerJsonStr = doc[0];

	// error = deserializeJson(spoolsDoc, innerJsonStr);
	// if (error)
	// {
	// 	Serial.print(F("deserializeJson() failed: "));
	// 	Serial.println(error.c_str());
	// 	return;
	// }

	// serializeJsonPretty(spoolsDoc, Serial);

	// JsonArray spoolsArray = spoolsDoc[0];

	// for (int v : spoolsArray) {

	// 	spoolsVector.push_back(std::move(v));
	// }



	// serializeJsonPretty(doc, Serial);

	// JsonArray dryboxSpools = doc[0];

	// for (int v : dryboxSpools) {
    // 	Serial.println(v);
	// 	spoolsVector.push_back(std::move(v));
  	// }
	// for (int i=0; i < dryboxSpools.size(); i++) {

	// 	// Serial.println("Item" + i);
	// 	// if (outerDoc[i]["location"] == "Drybox") {
	// 		serializeJsonPretty(dryboxSpools[i], Serial);
	// 		spoolsVector.push_back(std::move(dryboxSpools[i]));
	// 	// }
	// }
}


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

	Serial.println("addSpool");
	std::string spoolIdStr = std::to_string(spoolId);
	std::string spoolQuery = baseAPI_URL + "spool/" + spoolIdStr;

	// Serial.println(spoolQuery.c_str());

	http.useHTTP10(true);

	// Query spoolman to get the spool with the specified id
	http.begin(wifiClientHttp, spoolQuery.c_str());
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

	serializeJsonPretty(doc, Serial);

	JsonDocument spoolDoc;
	spoolDoc.set(doc);
	spoolsVector.push_back(std::move(spoolDoc)); // Move into vector
	// serializeJsonPretty(spoolDoc, Serial);

	// const char* location = doc["location"];
	// if (doc["location"] == "Drybox") {
	// 	spoolsVector.push_back(std::move(doc));
	// }
	// Serial.print("\n\n");

	http.end();
	// delay(100);
}

void Spools::deleteSpool(int &spoolId) {

	spoolsVector.erase(std::find(spoolsVector.begin(), spoolsVector.end(), spoolId));
	// spoolsOrderVector.erase(std::find(spoolsOrderVector.begin(), spoolsOrderVector.end(), spoolId)); 
	// pushUpdatedSpoolsOrder();
}

const std::vector<JsonDocument>& Spools::getSpools()
{
	return spoolsVector;
}





std::vector<int>& Spools::getSpoolsOrderVector() {
	
	for (int v : spoolsOrderVector)
	{
		std::cout << "Spool Orders: " << v << std::endl;
	}

    return spoolsOrderVector;
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