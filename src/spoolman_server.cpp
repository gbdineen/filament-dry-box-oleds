// ─────────────────────────────────────────────
//  SpoolManager ESP32 – Web Server + LittleFS
//  Call setupSpoolServer() from setup() after WiFi connects.
// ─────────────────────────────────────────────
#include <ArduinoJson.h>
#include <ESPAsyncWebServer.h>
#include <AsyncTCP.h>
#include <LittleFS.h>
#include <HTTPClient.h>
#include <WiFi.h>

// ── Config ────────────────────────────────────
static const char* SPOOLMAN_BASE   = "http://10.0.0.226:7912/api/v1/";
static const char* SLOT_ORDER_FILE = "/slotorder.json";
static const int   WEB_SERVER_PORT = 80;
static const int   DRYBOX_SLOTS    = 4;   // must match the UI constant

static AsyncWebServer server(WEB_SERVER_PORT);
static AsyncWebSocket ws("/ws");

// ─────────────────────────────────────────────
//  LittleFS helpers
// ─────────────────────────────────────────────

bool initFS() {
    if (!LittleFS.begin(true)) {
        Serial.println("[FS] LittleFS mount failed");
        return false;
    }
    // Seed with empty slot array if file missing
    if (!LittleFS.exists(SLOT_ORDER_FILE)) {
        File f = LittleFS.open(SLOT_ORDER_FILE, "w");
        if (f) { f.print("[]"); f.close(); }
    }
    Serial.println("[FS] LittleFS ready");
    return true;
}

// Raw read – returns stored JSON string.
// New format: [{"slot":1,"spoolId":4},{"slot":2,"spoolId":null},...]
String readSlotOrder() {
    File f = LittleFS.open(SLOT_ORDER_FILE, "r");
    if (!f) return "[]";
    String s = f.readString();
    f.close();
    return s;
}

// Raw write – call with a serialised JSON string.
bool writeSlotOrder(const String& json) {
    File f = LittleFS.open(SLOT_ORDER_FILE, "w");
    if (!f) return false;
    f.print(json);
    f.close();
    return true;
}

// ─────────────────────────────────────────────
//  Slot query helpers  (call these from display code)
// ─────────────────────────────────────────────

// Returns the spool ID for a 1-based slot number, or -1 if empty / not found.
//
// Usage example:
//   int id = getSpoolIdForSlot(1);  // → 4
//   int id = getSpoolIdForSlot(3);  // → -1 (empty)
int getSpoolIdForSlot(int slotNum) {
    String json = readSlotOrder();
    JsonDocument doc;
    if (deserializeJson(doc, json) != DeserializationError::Ok) return -1;
    if (!doc.is<JsonArray>()) return -1;

    for (JsonObject entry : doc.as<JsonArray>()) {
        if (entry["slot"].as<int>() == slotNum) {
            if (entry["spoolId"].isNull()) return -1;
            return entry["spoolId"].as<int>();
        }
    }
    return -1;
}

// Returns true if the given slot is empty (no spool assigned).
bool isSlotEmpty(int slotNum) {
    return getSpoolIdForSlot(slotNum) == -1;
}

// Fills an int array with the spool ID for each slot (index 0 = slot 1).
// Empty slots are set to -1.  Caller must ensure arr has at least DRYBOX_SLOTS elements.
//
// Usage example:
//   int ids[DRYBOX_SLOTS];
//   getAllSlotIds(ids);
//   for (int i = 0; i < DRYBOX_SLOTS; i++) {
//       Serial.printf("Slot %d → spool %d\n", i+1, ids[i]);
//   }
void getAllSlotIds(int* arr) {
    for (int i = 0; i < DRYBOX_SLOTS; i++) arr[i] = -1;

    String json = readSlotOrder();
    JsonDocument doc;
    if (deserializeJson(doc, json) != DeserializationError::Ok) return;
    if (!doc.is<JsonArray>()) return;

    for (JsonObject entry : doc.as<JsonArray>()) {
        int slotNum = entry["slot"].as<int>();
        if (slotNum >= 1 && slotNum <= DRYBOX_SLOTS) {
            arr[slotNum - 1] = entry["spoolId"].isNull() ? -1 : entry["spoolId"].as<int>();
        }
    }
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
    JsonDocument payload;
    payload["location"] = newLocation;
    String bodyStr;
    serializeJson(payload, bodyStr);
    int code = http.PATCH(bodyStr);
    http.end();
    return (code == 200);
}

// ─────────────────────────────────────────────
//  WebSocket
// ─────────────────────────────────────────────

// Override this in your main file to react when the browser pushes a new slot order.
// e.g. trigger an LED update.
__attribute__((weak)) void onSlotOrderUpdated(const String& /*json*/) {}

void onWsEvent(AsyncWebSocket*, AsyncWebSocketClient* client,
               AwsEventType type, void* arg, uint8_t* data, size_t len) {
    if (type == WS_EVT_CONNECT) {
        Serial.printf("[WS] Client #%u connected\n", client->id());
    } else if (type == WS_EVT_DISCONNECT) {
        Serial.printf("[WS] Client #%u disconnected\n", client->id());
    } else if (type == WS_EVT_DATA) {
        AwsFrameInfo* info = (AwsFrameInfo*)arg;
        if (!info->final || info->index != 0 || info->len != len
            || info->opcode != WS_TEXT) return;

        String msg;
        msg.reserve(len);
        for (size_t i = 0; i < len; i++) msg += (char)data[i];

        // Expected: {"type":"slotorder","payload":[{"slot":1,"spoolId":4},...]}
        JsonDocument doc;
        if (deserializeJson(doc, msg) != DeserializationError::Ok) return;
        if (strcmp(doc["type"] | "", "slotorder") != 0) return;

        Serial.println("onWSEvent spoolman_server");
        String orderJson;
        serializeJson(doc["payload"], orderJson);
        if (writeSlotOrder(orderJson)) {
            onSlotOrderUpdated(orderJson);   // hook for your display code
            client->text("{\"ok\":true}");
        } else {
            client->text("{\"error\":\"write failed\"}");
        }
    }
}

// ─────────────────────────────────────────────
//  Web Server routes
// ─────────────────────────────────────────────

void initWebServer() {
    ws.onEvent(onWsEvent);
    server.addHandler(&ws);

    // Serve UI
    server.on("/", HTTP_GET, [](AsyncWebServerRequest* req) {
        req->send(LittleFS, "/index.html", "text/html");
    });

    // GET /slotorder  →  returns stored slot map as JSON
    server.on("/slotorder", HTTP_GET, [](AsyncWebServerRequest* req) {
        req->send(200, "application/json", readSlotOrder());
    });

    // POST /slotorder  →  save slot map sent by browser
    // Body: [{"slot":1,"spoolId":4},{"slot":2,"spoolId":2},
    //        {"slot":3,"spoolId":null},{"slot":4,"spoolId":1}]
    server.on("/slotorder", HTTP_POST,
        [](AsyncWebServerRequest* req) {},
        NULL,
        [](AsyncWebServerRequest* req, uint8_t* data, size_t len, size_t /*index*/, size_t /*total*/) {
            String body;
            body.reserve(len);
            for (size_t i = 0; i < len; i++) body += (char)data[i];

            JsonDocument doc;
            if (deserializeJson(doc, body) != DeserializationError::Ok) {
                req->send(400, "application/json", "{\"error\":\"bad json\"}");
                return;
            }
            String newOrder;
            serializeJson(doc, newOrder);
            if (writeSlotOrder(newOrder)) {
                onSlotOrderUpdated(newOrder);
                req->send(200, "application/json", "{\"ok\":true}");
            } else {
                req->send(500, "application/json", "{\"error\":\"write failed\"}");
            }
        }
    );

    // GET /locations  →  proxy to Spoolman
    server.on("/locations", HTTP_GET, [](AsyncWebServerRequest* req) {
        req->send(200, "application/json", getLocations());
    });

    // GET /spools?location=X  →  proxy to Spoolman
    server.on("/spools", HTTP_GET, [](AsyncWebServerRequest* req) {
        String loc = req->hasParam("location")
            ? req->getParam("location")->value()
            : "Drybox";
        req->send(200, "application/json", getSpoolsByLocation(loc));
    });

    // POST /movespool  →  proxy PATCH to Spoolman
    // Body: {"id": 3, "location": "Standby"}
    server.on("/movespool", HTTP_POST,
        [](AsyncWebServerRequest* req) {},
        NULL,
        [](AsyncWebServerRequest* req, uint8_t* data, size_t len, size_t /*index*/, size_t /*total*/) {
            String body;
            body.reserve(len);
            for (size_t i = 0; i < len; i++) body += (char)data[i];

            JsonDocument doc;
            if (deserializeJson(doc, body) != DeserializationError::Ok) {
                req->send(400, "application/json", "{\"error\":\"bad json\"}");
                return;
            }
            int    spoolId     = doc["id"].as<int>();
            String newLocation = doc["location"].as<String>();
            patchSpoolLocation(spoolId, newLocation)
                ? req->send(200, "application/json", "{\"ok\":true}")
                : req->send(500, "application/json", "{\"error\":\"patch failed\"}");
        }
    );

    server.begin();
    Serial.printf("[WS] Web server started on port %d\n", WEB_SERVER_PORT);
}

// ─────────────────────────────────────────────
//  Public entry point – call from setup()
// ─────────────────────────────────────────────
void setupSpoolServer() {
    initFS();
    initWebServer();
}