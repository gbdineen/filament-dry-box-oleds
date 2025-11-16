#ifndef SPOOLS_H
#define SPOOLS_H


#include <ArduinoJson.h>
#include <iostream>
#include <string> 
#include <vector>
// #include <SPI.h>
#include <HTTPClient.h>
#include <WebSocketsClient.h>
#include <PubSubClient.h> // For MQTT

class Spools 
{
     public:
        Spools();
        ~Spools();
        void addSpool(int spoolId);
        void getSpools();
        void getSpoolOrder();
        void updateSpool(int spoolId, int remWeight);
        void begin();
        
        
    private:
        // JSON STUFF
        JsonDocument spoolsJson;
        bool useFilters = true;
        std::vector<JsonDocument> spoolsDocs;   // holds real storage
        std::vector<JsonObject> spoolsVector;   //

        WiFiClient wifiClientHttp;
        HTTPClient http; 
        
        const char *hostIP = "192.168.8.228";
        std::string baseAPI_URL = "http://192.168.8.228:7912/api/v1/";
        int wsPort = 7912;
       

   

};






#endif

