#ifndef DATA_H
#define DATA_H

#include "spools.h"
#include "displays.h"

#include <ArduinoJson.h>
#include <HTTPClient.h>
#include <WebSocketsClient.h>
#include <PubSubClient.h> // For MQTT
#include <WiFi.h>
#include <functional>
#include <iostream>

class Data
{
public:
    Data(Spools& spoolsRef, Displays& displaysRef);
    ~Data();
    void loop();
    void begin();
    void webSocketEvent(WStype_t type, uint8_t *payload, size_t length);
    void mqttCallback(char *topic, byte *payload, unsigned int length);
    boolean mqttReconnect();
    boolean wsConnected();
    void setWSCallback(std::function<void(const char*)> cb);
    WiFiClient wifiClient;
    WiFiClient wifiClientHttp;
    HTTPClient http;

private:

    Spools& spoolsRef;
    Displays& displaysRef;

     int slots = 4;

    std::function<void(const char*)> wsCallback;

    WebSocketsClient webSocket;
    static void webSocketEventStatic(WStype_t type, uint8_t *payload, size_t length);
    static Data *wsCallbackInstance;

    PubSubClient mqttClient;
    static void mqttCallbackStatic(char *topic, byte *payload, unsigned int length);
    static Data *mqttCallbackInstance;

    const char *apiHostIP = "10.0.0.110";
    std::string baseAPI_URL = "http://10.0.0.110:7912/api/v1/";
    int wsPort = 7912;

    const char *ssid = "Cedar Basecamp";
    const char *password = "N1mbl3Sh@rk";

    // MQTT broker details
    const char *mqttHostIP = "10.0.0.1";
    const char *mqtt_broker = mqttHostIP;
    const int mqtt_port = 1883; // Or 8883 for SSL/TLS
    const char *mqtt_client_id = "drybox_oleds";
    const char *mqttUN = "cedar_mqtt";
    const char *mqttPW = "basecamp";
    int lastReconnectAttempt = 0;
    bool firstStart = true;

    // boolean mqttReconnect();
};

#endif