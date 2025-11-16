#include "data.h"

Data *Data::wsCallbackInstance = nullptr;
Data *Data::mqttCallbackInstance = nullptr;



Data::Data()
{
    wsCallbackInstance = this;
    mqttCallbackInstance = this;
    // mqttClient(wifiClient);
}


void Data::mqttCallbackStatic(char *topic, byte *payload, unsigned int length)
{

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
                Serial.printf("[WSc] Connected to url: %s\n", payload);
            }

            // send message to server when Connected
            webSocket.sendTXT("Connected");

            // initDisplays();
            // getSpoolOrder();
            // patchSpoolOrder();

            break;

        case WStype_TEXT:

            Serial.println("[WSc] incoming");

            JsonDocument doc;
            JsonDocument filter;

            DeserializationError error = deserializeJson(doc, payload);
            if (error)
            {
                Serial.print(F("deserializeJson() failed: "));
                Serial.println(error.c_str());
                return;
            }

            serializeJsonPretty(doc, Serial);

            if (doc["type"] == "updated")
            {

                if (doc["resource"] == "spool")
                {
                    filter["payload"]["id"] = true;
                    filter["payload"]["remaining_weight"] = true;
                    filter["payload"]["filament"]["name"] = true;
                    filter["payload"]["filament"]["material"] = true;

                    error = deserializeJson(doc, payload, DeserializationOption::Filter(filter));
                    if (error)
                    {
                        Serial.print(F("deserializeJson() failed: "));
                        Serial.println(error.c_str());
                        return;
                    }
                    // serializeJsonPretty(doc,Serial);

                    int spoolId = doc["payload"]["id"];
                    int remWeight = doc["payload"]["remaining_weight"];

                    // updateSpool(spoolId, remWeight);
                    // delay(500);
                    // getSpoolOrder();
                }
                else if (doc["resource"] == "setting")
                {

                    filter["payload"] = true;

                    error = deserializeJson(doc, payload, DeserializationOption::Filter(filter));
                    if (error)
                    {
                        Serial.print(F("deserializeJson() failed: "));
                        Serial.println(error.c_str());
                        return;
                    }
                    // serializeJsonPretty(doc,Serial);

                    // getSpoolOrder();
                }
            }
            break;
    }
}

boolean Data::wsConnected() {
    return true;

}

boolean Data::mqttReconnect() {
  if (mqttClient.connect(mqtt_client_id, mqttUN, mqttPW)) {
    Serial.println("MQTT connected");
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


    WiFi.begin(ssid, password);
        while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.println("Connecting to WiFi...");
    }
    Serial.println("Connected to WiFi");

    webSocket.begin(hostIP, wsPort, "/api/v1/");
    webSocket.onEvent(webSocketEventStatic);
    webSocket.setReconnectInterval(5000);

    mqttClient.setClient(wifiClient);

    mqttClient.setServer(mqtt_broker, mqtt_port);
	mqttClient.setCallback(mqttCallbackStatic);
    // lastReconnectAttempt = 0;

    


    // std::string clientIdStr = mqtt_client_id;
    // std::string connectMsg = "MQTT client " + clientIdStr + " connected";
    // mqttClient.setServer(mqtt_broker, mqtt_port);
    // mqttClient.setCallback(mqttCallbackStatic);

    // if (mqttClient.connect(mqtt_client_id, mqttUN, mqttPW)) {
    //     mqttClient.publish("mqttStatus",connectMsg.c_str());
    //     // ... and resubscribe
    //     mqttClient.subscribe("octoPrint/event/plugin_Spoolman_spool_selected");
    //     mqttClient.subscribe("octoPrint/event/plugin_Spoolman_spool_usage_committed");
    // }
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