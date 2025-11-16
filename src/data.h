#ifndef DATA_H
#define DATA_H

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
    void loop();
    void begin();
    void webSocketEvent(WStype_t type, uint8_t *payload, size_t length);
    void mqttCallback(char *topic, byte *payload, unsigned int length);
    boolean mqttReconnect();
    boolean wsConnected();

    
    WiFiClient wifiClientHttp;
    HTTPClient http;

    Data();
    ~Data();

private:
    WiFiClient wifiClient;

    WebSocketsClient webSocket;
    static void webSocketEventStatic(WStype_t type, uint8_t *payload, size_t length);
    static Data *wsCallbackInstance;

    PubSubClient mqttClient;
    static void mqttCallbackStatic(char *topic, byte *payload, unsigned int length);
    static Data *mqttCallbackInstance;

    const char *hostIP = "192.168.8.228";
    std::string baseAPI_URL = "http://192.168.8.228:7912/api/v1/";
    int wsPort = 7912;

    const char *ssid = "GL-SFT1200-887";
    const char *password = "goodlife";

    // MQTT broker details
    const char *mqtt_broker = hostIP;
    const int mqtt_port = 1883; // Or 8883 for SSL/TLS
    const char *mqtt_client_id = "drybox_oleds";
    const char *mqttUN = "gbdineen";
    const char *mqttPW = "N1mbl3Sh@rk";
    int lastReconnectAttempt = 0;
    bool firstStart = true;

    boolean mqttReconnect();
};

#endif