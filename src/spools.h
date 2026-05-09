#ifndef SPOOLS_H
#define SPOOLS_H

// #include "displays.h"

#include <ArduinoJson.h>
#include <iostream>
#include <string> 
#include <vector>
#include <algorithm>
#include <HTTPClient.h>
#include <WebSocketsClient.h>
#include <PubSubClient.h> // For MQTT

#define DEBUG

class Spools 
{
     public:
        Spools();
        ~Spools();
        void getSpoolInfo(int *sid, int *w, const char* *m, const char* *n);
        void initSpools();
        void getSpoolsOrder();
        std::vector<int>&  getSpoolsOrderVector();
        const std::vector<JsonDocument>& getSpools();
        void setSpoolsOrder(const String& newOrder);
        void loadSpools();
        void addSpool(int &spoolId);
        JsonDocument getSpool(int &spoolId);
        void deleteSpool(int &spoolId);
        int getSpoolsCount();
        void loopSpoolsVector();
        // std::vector<JsonArray> getSpoolOrder();
        void updateSpool(int& sid, int& w, const char* &m, const char* &n, int* d);
        void refactorSpoolsOrder();
        void getSlots();
        void begin();
        
    private:

       
        bool debug = false;

        // JSON STUFF
        JsonDocument spoolsJson;
        bool useFilters = true;
        std::vector<JsonDocument> spoolsDocs;   // holds real storage
        std::vector<JsonDocument> spoolsVector;
        std::vector<JsonDocument> unorderedSpools;
        std::vector<int> spoolsOrderVector;

        int slots = 4;

        WiFiClient wifiClientHttp;
        HTTPClient http; 
        
        // Spoolman API
        const char *hostIP = "10.0.0.226";
        std::string baseAPI_URL = "http://10.0.0.226:7912/api/v1/";
        int wsPort = 7912;

        // Spools Order Server API
        const char *spoolsOrderServerIP = "10.0.0.84";
        std::string spoolsOrderServer_URL = "http://10.0.0.84";
        // int wsPort = 7912;

};

// class Displays : public Spools {
//     public:
//         void overviewDisplay();

// };


#endif

