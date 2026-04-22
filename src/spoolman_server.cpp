// ─────────────────────────────────────────────
//  SpoolManager ESP32 – Web Server + LittleFS
// ─────────────────────────────────────────────
#include "spoolman_server.h"
#include <ArduinoJson.h>
#include <ESPAsyncWebServer.h>
#include <AsyncWebSocket.h>
#include <LittleFS.h>
#include <HTTPClient.h>
#include <WiFi.h>
#include <vector>
#include <functional>

// ── Config ────────────────────────────────────
const char* SPOOLMAN_BASE    = "http://10.0.0.110:7912/api/v1/";
const char* SLOT_ORDER_FILE  = "/slotorder.json";
const int   WEB_SERVER_PORT  = 80;

static AsyncWebServer    server(WEB_SERVER_PORT);
static AsyncWebSocket    ws("/ws");

static std::function<void(std::vector<int>)> orderUpdateCallback = nullptr;

// ─────────────────────────────────────────────
//  Public: register callback
// ─────────────────────────────────────────────
void setOrderUpdateCallback(std::function<void(std::vector<int>)> cb) {
    orderUpdateCallback = cb;
}

// ─────────────────────────────────────────────
//  LittleFS helpers
// ─────────────────────────────────────────────
bool initFS() {
    if (!LittleFS.begin(true)) {
        Serial.println("[FS] LittleFS mount failed");
        return false;
    }
    if (!LittleFS.exists(SLOT_ORDER_FILE)) {
        File f = LittleFS.open(SLOT_ORDER_FILE, "w");
        if (f) { f.print("[]"); f.close(); }
    }
    Serial.println("[FS] LittleFS ready");
    return true;
}

String readSlotOrder() {
    File f = LittleFS.open(SLOT_ORDER_FILE, "r");
    if (!f) return "[]";
    String s = f.readString();
    f.close();
    return s;
}

bool writeSlotOrder(const String& json) {
    File f = LittleFS.open(SLOT_ORDER_FILE, "w");
    if (!f) return false;
    f.print(json);
    f.close();
    return true;
}

// Parse a JSON array string into a vector<int> and fire the callback
void notifyOrderUpdate(const String& json) {
    if (!orderUpdateCallback) return;
    JsonDocument doc;
    if (deserializeJson(doc, json) != DeserializationError::Ok) return;
    std::vector<int> order;
    for (JsonVariant v : doc.as<JsonArray>()) {
        order.push_back(v.as<int>());
    }
    orderUpdateCallback(order);
}

// ─────────────────────────────────────────────
//  Spoolman proxy helpers
// ─────────────────────────────────────────────
String getLocations() {
    HTTPClient http;
    http.begin(String(SPOOLMAN_BASE) + "setting/locations");
    int code = http.GET();
    String body = (code == 200) ? http.getString() : "[]";
    http.end();
    return body;
}

String getSpoolsByLocation(const String& location) {
    HTTPClient http;
    http.begin(String(SPOOLMAN_BASE) + "spool?location=" + location);
    int code = http.GET();
    String body = (code == 200) ? http.getString() : "[]";
    http.end();
    return body;
}

bool patchSpoolLocation(int spoolId, const String& newLocation) {
    HTTPClient http;
    http.begin(String(SPOOLMAN_BASE) + "spool/" + String(spoolId));
    http.addHeader("Content-Type", "application/json");
    JsonDocument body;
    body["location"] = newLocation;
    String bodyStr;
    serializeJson(body, bodyStr);
    int code = http.PATCH(bodyStr);
    http.end();
    return (code == 200);
}

// ─────────────────────────────────────────────
//  WebSocket handler
// ─────────────────────────────────────────────
void onWsEvent(AsyncWebSocket* server, AsyncWebSocketClient* client,
               AwsEventType type, void* arg, uint8_t* data, size_t len) {
    if (type == WS_EVT_CONNECT) {
        Serial.printf("[WS] Client #%u connected\n", client->id());
    } else if (type == WS_EVT_DISCONNECT) {
        Serial.printf("[WS] Client #%u disconnected\n", client->id());
    } else if (type == WS_EVT_DATA) {
        AwsFrameInfo* info = (AwsFrameInfo*)arg;
        if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
            String msg = "";
            for (size_t i = 0; i < len; i++) msg += (char)data[i];

            // Expect: {"type":"slotorder","payload":[1,3]}
            JsonDocument doc;
            if (deserializeJson(doc, msg) == DeserializationError::Ok) {
                if (strcmp(doc["type"], "slotorder") == 0) {
                    String orderJson;
                    serializeJson(doc["payload"], orderJson);
                    writeSlotOrder(orderJson);
                    notifyOrderUpdate(orderJson);
                    client->text("{\"ok\":true}");
                }
            }
        }
    }
}

// ─────────────────────────────────────────────
//  Web Server routes
// ─────────────────────────────────────────────
void initWebServer() {

    // WebSocket
    ws.onEvent(onWsEvent);
    server.addHandler(&ws);

    // Serve web interface
    server.on("/", HTTP_GET, [](AsyncWebServerRequest* req) {
        req->send(LittleFS, "/index.html", "text/html");
    });

    // GET /slotorder
    server.on("/slotorder", HTTP_GET, [](AsyncWebServerRequest* req) {
        req->send(200, "application/json", readSlotOrder());
    });

    // POST /slotorder
    server.on("/slotorder", HTTP_POST,
        [](AsyncWebServerRequest* req) {},
        NULL,
        [](AsyncWebServerRequest* req, uint8_t* data, size_t len, size_t index, size_t total) {
            String body = "";
            for (size_t i = 0; i < len; i++) body += (char)data[i];
            JsonDocument doc;
            DeserializationError err = deserializeJson(doc, body);
            if (err) { req->send(400, "application/json", "{\"error\":\"bad json\"}"); return; }
            String newOrder;
            serializeJson(doc, newOrder);
            if (writeSlotOrder(newOrder)) {
                notifyOrderUpdate(newOrder);
                req->send(200, "application/json", "{\"ok\":true}");
            } else {
                req->send(500, "application/json", "{\"error\":\"write failed\"}");
            }
        });

    // GET /locations
    server.on("/locations", HTTP_GET, [](AsyncWebServerRequest* req) {
        req->send(200, "application/json", getLocations());
    });

    // GET /spools?location=X
    server.on("/spools", HTTP_GET, [](AsyncWebServerRequest* req) {
        String location = req->hasParam("location")
            ? req->getParam("location")->value()
            : "Drybox";
        req->send(200, "application/json", getSpoolsByLocation(location));
    });

    // POST /movespool
    server.on("/movespool", HTTP_POST,
        [](AsyncWebServerRequest* req) {},
        NULL,
        [](AsyncWebServerRequest* req, uint8_t* data, size_t len, size_t index, size_t total) {
            String body = "";
            for (size_t i = 0; i < len; i++) body += (char)data[i];
            JsonDocument doc;
            DeserializationError err = deserializeJson(doc, body);
            if (err) { req->send(400, "application/json", "{\"error\":\"bad json\"}"); return; }
            int spoolId        = doc["id"].as<int>();
            String newLocation = doc["location"].as<String>();
            patchSpoolLocation(spoolId, newLocation)
                ? req->send(200, "application/json", "{\"ok\":true}")
                : req->send(500, "application/json", "{\"error\":\"patch failed\"}");
        });

    server.begin();
    Serial.println("[WS] Web server started on port " + String(WEB_SERVER_PORT));
}

// ─────────────────────────────────────────────
//  Call from setup() after WiFi connects
// ─────────────────────────────────────────────
void setupSpoolServer() {
    initFS();
    initWebServer();
}
