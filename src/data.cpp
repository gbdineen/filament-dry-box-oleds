#include "data.h"
#include "spoolman_server.h"
#include "esp_wifi.h"


Data *Data::wsCallbackInstance = nullptr;
Data *Data::mqttCallbackInstance = nullptr;



Data::Data(Spools &spoolsRef, Displays& displaysRef)
    : spoolsRef(spoolsRef),
      displaysRef(displaysRef)
{
    wsCallbackInstance = this;
    mqttCallbackInstance = this;
    // mqttClient(wifiClient);
}


void Data::mqttCallbackStatic(char *topic, byte *payload, unsigned int length)
{

    Serial.println("MQTT Callback Static");
    if (mqttCallbackInstance)
    {
        mqttCallbackInstance->mqttCallbackStatic(topic, payload, length);
    }
}

void Data::mqttCallback(char *topic, byte *payload, unsigned int length)
{

    Serial.println("MQTT Callback");

    JsonDocument filter;
    JsonDocument doc;

    if (String(topic) == "octoPrint/event/plugin_Spoolman_spool_selected")
    {

        filter["spoolId"] = true;

        DeserializationError error = deserializeJson(doc, payload, DeserializationOption::Filter(filter));
        if (error)
        {
            Serial.print(F("deserializeJson() failed: "));
            Serial.println(error.c_str());
            return;
        }
#ifdef DEBUG
        serializeJsonPretty(doc, Serial);
#endif

        // Serial.println("===");

        // serializeJsonPretty(spoolsJson[1]["id"],Serial);

        // int displayId;
        // int responseCount = 0;

        // for (int i = 0; i < spoolsVector.size(); i++)
        // {

        //     // Serial.println("Spool Id: " + String(spoolsJson[i]["id"]) + "return Spool Id: " + String(doc["spoolId"]));

        //     std::string spoolIdStr = spoolsVector[i]["id"];
        //     std::string fullMessageStr = "Spool Id " + spoolIdStr + " Selected";
        //     // const char* fullMessageChar = fullMessageStr.c_str();

        //     if (doc["spoolId"].as<int>() == spoolsVector[i]["id"])
        //     {

        //         mqttClient.publish("mqttStatus", fullMessageStr.c_str());

        //         // displayId=i;
        //         displayArray[i].invertDisplay(true);
        //         responseCount = 1;
        //     }
        //     else
        //     {

        //         if (responseCount < 1)
        //         {
        //             responseCount++;
        //             mqttClient.publish("mqttStatus", "No active spool selected");
        //         }
        //         displayArray[i].invertDisplay(false);
        //     }
        // }
        // responseCount = 0;
    }
    else if (String(topic) == "octoPrint/event/plugin_Spoolman_spool_usage_committed")
    {
    }
}

void Data::webSocketEventStatic(WStype_t type, uint8_t *payload, size_t length)
{

    if (wsCallbackInstance)
    {
        wsCallbackInstance->webSocketEvent(type, payload, length);
    }
}

void Data::webSocketEvent(WStype_t type, uint8_t *payload, size_t length)
{

    switch (type){
        case WStype_DISCONNECTED:
            Serial.printf("[WSc] Disconnected!\n");
            break;

        case WStype_CONNECTED:

            if (firstStart)
            {
                firstStart = false;
                // Serial.printf("[WSc] Connected to url: %s\n", payload);
                wsCallback("[WSc] Connected");
            } else {
                Serial.print("[WSc] Re-connected");
            }

            // send message to server when Connected
            webSocket.sendTXT("Connected");

            // initDisplays();
            // getSpoolOrder();
            // patchSpoolOrder();

            break;

        case WStype_TEXT:

            // Serial.println("[WSc] incoming");
            // Serial.println(payload.as<String>());

            // break;

            JsonDocument doc;
            JsonDocument filter;

            filter["resource"]= true;
            filter["type"]= true;
            filter["payload"]["id"] = true;
            filter["payload"]["location"] = true;
            filter["payload"]["remaining_weight"] = true;
            filter["payload"]["filament"]["name"] = true;
            filter["payload"]["filament"]["material"] = true;
            filter["payload"]["filament"]["extra"]["friendly_color_name"] = true;

            // DeserializationError error = deserializeJson(doc, payload);
            DeserializationError error =  deserializeJson(doc, payload, DeserializationOption::Filter(filter));
            if (error)
            {
                Serial.print(F("deserializeJson() failed: "));
                Serial.println(error.c_str());
                return;
            }

            serializeJsonPretty(doc, Serial);

            
            
            if (doc["type"] == "updated")
            {
                // Serial.println("\n type - updated");
                if (doc["resource"] == "spool")
                {

                    
                    int spoolId = doc["payload"]["id"].as<int>();;
                    int remWeight = doc["payload"]["remaining_weight"];
                    const char* name = doc["payload"]["filament"]["name"];
                    const char* material = doc["payload"]["filament"]["material"];
                    String location = doc["payload"]["location"].as<String>();
                    
                    displaysRef.stopPageDisplays();
                    displaysRef.overviewDisplay(); 

                    std::vector<JsonDocument> spools = spoolsRef.getSpools();

                    int vSize = spools.size();
                    // Serial.println("Spools Size: " + String(vSize));
                    bool newSpool = false;

                    for (int i=0; i<vSize; i++)
                    {

                        int currId = spools[i]["id"].as<int>();

                        // Serial.print("currId: "); Serial.print(currId);
                        // Serial.print(" | spoolId: "); Serial.println(spoolId); 

                        if (currId == spoolId)
                        {
                            int displayId = i;
                            if (location == "Drybox")
                            {  
                                spoolsRef.updateSpool(spoolId, remWeight, material, name, &displayId);
                                const char* updateMsg = "UPDATED";
                                displaysRef.printMessage(displayId, updateMsg, true);
                                break;
                            }
                            else
                            {
                                spoolsRef.deleteSpool(spoolId);
                                const char* updateMsg = "REMOVED";
                                displaysRef.printMessage(displayId, updateMsg, true);

                            }

                        }
                        else
                        {
                            newSpool = true;
                        }
                    }
                    
                    if (newSpool) {
                        newSpool=false;
                        int addDisplayID = vSize;
                        // spoolsRef.addSpool(spoolId);
                        // const char* updateMsg = "ADDED";
                        // displaysRef.printMessage(addDisplayID, updateMsg, true);
                    }
                    
                }
                else if (doc["resource"] == "setting")
                {
                    Serial.println("Resource - Setting"); 
                }
                displaysRef.startPageDisplays();
            }
                
        break;
    }
}

void Data::setWSCallback(std::function<void(const char*)> cb) {
    this->wsCallback = cb;
}

std::string Data::getAPI_URL(std::string URL) {
    Serial.println(baseAPI_URL.c_str());
    return URL;
}

boolean Data::mqttReconnect() {
  if (mqttClient.connect(mqtt_client_id, mqttUN, mqttPW)) {
    // Serial.println("MQTT connected");
    // Once connected, publish an announcement...

    std::string clientIdStr = mqtt_client_id;
    std::string connectMsg = "MQTT client " + clientIdStr + " connected";

    mqttClient.publish("mqttStatus",connectMsg.c_str());
    // ... and resubscribe
    mqttClient.subscribe("octoPrint/event/plugin_Spoolman_spool_selected");
    mqttClient.subscribe("octoPrint/event/plugin_Spoolman_spool_usage_committed");

  }
  return mqttClient.connected();
}

void Data::begin()
{

   	// Serial.begin(115200);
    // while (!Serial)
    //     continue; 
	// Serial.print("Serial active");
   
    // WiFi.disconnect(true);
    // WiFi.mode(WIFI_OFF);
    // delay(500);  // longer delay
    // esp_wifi_stop();  // force stop the wifi driver
    // delay(500);
    WiFi.mode(WIFI_STA);
    WiFi.setHostname(hostname);

    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        // Serial.println("Connecting to WiFi...");
    }
    Serial.begin(115200);
    Serial.println("Connected to WiFi");
    Serial.println(WiFi.localIP()); 

    setupSpoolServer();

    webSocket.begin(apiHostIP, wsPort, "/api/v1/");
    webSocket.onEvent(webSocketEventStatic);
    webSocket.setReconnectInterval(1000);

    mqttClient.setClient(wifiClient);
    mqttClient.setServer(mqtt_broker, mqtt_port);
	mqttClient.setCallback(mqttCallbackStatic);


}

void Data::loop()
{
    // Serial.println("looping");
    if (!mqttClient.connected()) {
        // Serial.print("MQTT Disconnected");
        long now = millis();
    if (now - lastReconnectAttempt > 5000) {
        lastReconnectAttempt = now;
        // Attempt to reconnect
        if (mqttReconnect()) {
            lastReconnectAttempt = 0;
        }
    }
    } else {
        // Client connected
        mqttClient.loop();
    }
    // mqttClient.loop();
    webSocket.loop();
}

Data::~Data()
{
}