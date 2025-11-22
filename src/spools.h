#ifndef SPOOLS_H
#define SPOOLS_H

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
        void addSpool(int spoolId);
        void getSpoolInfo(int *sid, int *w, const char* *m, const char* *n);
        void initSpools();
        void initSpoolsDEP();
        void initSpoolsDEP2();
        void getSpoolsOrder();
        std::vector<JsonDocument>& getSpools();
        void getSpool(int spoolId);
        int getSpoolsCount();
        // std::vector<JsonArray> getSpoolOrder();
        void updateSpool(int& sid, int& w, const char* &m, const char* &n, int* d);
        void begin();
        
    private:

        bool debug = false;

        // JSON STUFF
        JsonDocument spoolsJson;
        bool useFilters = true;
        std::vector<JsonDocument> spoolsDocs;   // holds real storage
        std::vector<JsonDocument> spoolsVector;
        std::vector<int> spoolsOrderVector;

        int slots = 4;

        WiFiClient wifiClientHttp;
        HTTPClient http; 
        
        const char *hostIP = "192.168.8.228";
        std::string baseAPI_URL = "http://192.168.8.228:7912/api/v1/";
        int wsPort = 7912;

};

// class Displays : public Spools {
//     public:
//         void overviewDisplay();

// };


#endif

