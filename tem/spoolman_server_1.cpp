// ─────────────────────────────────────────────
//  SpoolManager ESP32 – Web Server + LittleFS
//  Add to your main project as needed
// ─────────────────────────────────────────────
#include <ArduinoJson.h>
#include <ESPAsyncWebServer.h>
#include <LittleFS.h>
#include <HTTPClient.h>
#include <WiFi.h>
#include "spoolman_server.h"

// ── Config ────────────────────────────────────
const char* SPOOLMAN_BASE = "http://10.0.0.110:7912/api/v1/";
const char* SLOT_ORDER_FILE = "/slotorder.json";
const int   WEB_SERVER_PORT = 80;

AsyncWebServer server(WEB_SERVER_PORT);

// ─────────────────────────────────────────────
//  LittleFS helpers
// ─────────────────────────────────────────────

// Initialise LittleFS – call once in setup()
bool initFS() {
    if (!LittleFS.begin(true)) {  // true = format on fail
        Serial.println("[FS] LittleFS mount failed");
        return false;
    }
    // Create default slot order file if it doesn't exist yet
    if (!LittleFS.exists(SLOT_ORDER_FILE)) {
        File f = LittleFS.open(SLOT_ORDER_FILE, "w");
        if (f) { f.print("[]"); f.close(); }
    }
    Serial.println("[FS] LittleFS ready");
    return true;
}

// Read slot order from LittleFS → returns JSON string e.g. "[1,3]"
String readSlotOrder() {
    File f = LittleFS.open(SLOT_ORDER_FILE, "r");
    if (!f) return "[]";
    String s = f.readString();
    f.close();
    return s;
}

// Write slot order to LittleFS – expects a JSON array string e.g. "[3,1]"
bool writeSlotOrder(const String& json) {
    File f = LittleFS.open(SLOT_ORDER_FILE, "w");
    if (!f) return false;
    f.print(json);
    f.close();
    return true;
}

// ─────────────────────────────────────────────
//  Spoolman proxy helpers
// ─────────────────────────────────────────────

// GET /api/v1/setting/locations  →  forward to client as-is
String getLocations() {
    HTTPClient http;
    http.begin(String(SPOOLMAN_BASE) + "setting/locations");
    int code = http.GET();
    String body = (code == 200) ? http.getString() : "[]";
    http.end();
    return body;
}

// GET /api/v1/spool?location=X  →  forward to client as-is
String getSpoolsByLocation(const String& location) {
    HTTPClient http;
    http.begin(String(SPOOLMAN_BASE) + "spool?location=" + location);
    int code = http.GET();
    String body = (code == 200) ? http.getString() : "[]";
    http.end();
    return body;
}

// PATCH /api/v1/spool/{id}  with {"location": "X"}
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

void setOrderUpdateCallback(std::function<void(std::vector<int>)> cb)
{
    
}

// ─────────────────────────────────────────────
//  Web Server routes
// ─────────────────────────────────────────────
void initWebServer() {

    // Serve the web interface
    server.on("/", HTTP_GET, [](AsyncWebServerRequest* req) {
        req->send(LittleFS, "/index.html", "text/html");
    });

    // ── Slot order ──────────────────────────
    // GET /slotorder  →  return current Drybox slot order
    server.on("/slotorder", HTTP_GET, [](AsyncWebServerRequest* req) {
        req->send(200, "application/json", readSlotOrder());
    });

    // POST /slotorder  →  save new Drybox slot order
    // Body: [1, 3]
    server.on("/slotorder", HTTP_POST,
        [](AsyncWebServerRequest* req) {},
        NULL,
        [](AsyncWebServerRequest* req, uint8_t* data, size_t len, size_t index, size_t total) {
            String body = "";
            for (size_t i = 0; i < len; i++) body += (char)data[i];

            JsonDocument doc;
            DeserializationError err = deserializeJson(doc, body);
            if (err) {
                req->send(400, "application/json", "{\"error\":\"bad json\"}");
                return;
            }

            String newOrder;
            serializeJson(doc, newOrder);
            writeSlotOrder(newOrder)
                ? req->send(200, "application/json", "{\"ok\":true}")
                : req->send(500, "application/json", "{\"error\":\"write failed\"}");
        });

    // ── Spoolman proxies ────────────────────
    // GET /locations  →  proxy to Spoolman
    server.on("/locations", HTTP_GET, [](AsyncWebServerRequest* req) {
        req->send(200, "application/json", getLocations());
    });

    // GET /spools?location=X  →  proxy to Spoolman
    server.on("/spools", HTTP_GET, [](AsyncWebServerRequest* req) {
        String location = req->hasParam("location")
            ? req->getParam("location")->value()
            : "Drybox";
        req->send(200, "application/json", getSpoolsByLocation(location));
    });

    // POST /movespool  →  proxy PATCH to Spoolman
    // Body: {"id": 3, "location": "Standby"}
    server.on("/movespool", HTTP_POST,
        [](AsyncWebServerRequest* req) {},
        NULL,
        [](AsyncWebServerRequest* req, uint8_t* data, size_t len, size_t index, size_t total) {
            String body = "";
            for (size_t i = 0; i < len; i++) body += (char)data[i];

            JsonDocument doc;
            DeserializationError err = deserializeJson(doc, body);
            if (err) {
                req->send(400, "application/json", "{\"error\":\"bad json\"}");
                return;
            }

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
//  Call both of these from your setup()
// ─────────────────────────────────────────────
void setupSpoolServer() {
    initFS();
    initWebServer();
}
