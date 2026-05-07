# Drybox Spool Manager — API Reference

---

## ESP32 Spool Server
**Base URL:** `http://<ESP32_IP>`

| Method | Endpoint | Body | Returns | Description |
|--------|----------|------|---------|-------------|
| `GET` | `/` | — | HTML | Serves the web interface (index.html from LittleFS) |
| `GET` | `/slotorder` | — | `[{slot:1,spoolId:4},{slot:2,spoolId:null},...]` | Drybox slot assignments from LittleFS |
| `POST` | `/slotorder` | `[{slot:1,spoolId:4},...]` | `{"ok":true}` | Save slot assignments to LittleFS |
| `GET` | `/locations` | — | `{"value":"[\"Drybox\",\"Standby\"]",...}` | Location list proxied from Spoolman |
| `GET` | `/spools?location=X` | — | `[{...},{...}]` | Spools by location, proxied from Spoolman |
| `POST` | `/movespool` | `{"id":3,"location":"Standby"}` | `{"ok":true}` | Patches spool location in Spoolman |
| `WS` | `/ws` | — | — | WebSocket endpoint (inbound from web UI) |

### `/slotorder` — Slot Format

Slots are always a fixed-length array of `DRYBOX_SLOTS` (4) objects:

```json
[
  { "slot": 1, "spoolId": 4 },
  { "slot": 2, "spoolId": 2 },
  { "slot": 3, "spoolId": null },
  { "slot": 4, "spoolId": 1 }
]
```

`spoolId: null` indicates an empty slot. Index by `slot` number, not array position.

> **Migration note:** The old flat format `[4, 2, 1, 3]` is auto-detected by the web UI on first load and immediately rewritten to the new object format.

### `/ws` — WebSocket (ESP32 inbound)

The web UI opens `ws://<ESP32_IP>/ws` and pushes slot order updates after every change.

**Message format (browser → ESP32):**
```json
{
  "type": "slotorder",
  "payload": [
    { "slot": 1, "spoolId": 4 },
    { "slot": 2, "spoolId": 2 },
    { "slot": 3, "spoolId": null },
    { "slot": 4, "spoolId": 1 }
  ]
}
```

The ESP32 handles this via `AsyncWebSocket` on the existing `ESPAsyncWebServer` instance (separate from the outbound `WebSocketsClient` that connects to Spoolman).

### ESP32-side Helper Functions

```cpp
int  getSpoolIdForSlot(int slot);   // returns spoolId, or -1 if empty
bool isSlotEmpty(int slot);         // true if spoolId is null/-1
void getAllSlotIds(int ids[]);       // snapshot all 4 slots at once (ids[0] = slot 1, etc.)
```

**`onSlotOrderUpdated` callback hook** — override in `main.cpp` to react immediately when the browser saves a change:

```cpp
void onSlotOrderUpdated(const String& json) {
    refreshDisplay();
}
```

---

## Spoolman
**Base URL:** `http://10.0.0.110:7912/api/v1/`

| Method | Endpoint | Body | Description |
|--------|----------|------|-------------|
| `GET` | `/spool?location=X` | — | Get spools filtered by location |
| `PATCH` | `/spool/{id}` | `{"location":"Drybox"}` | Update a spool's location |
| `GET` | `/setting/locations` | — | Get all location names |
| `GET` | `/setting/locations_spoolorders` | — | Raw spool order (read-only — avoid) |

---

## WebSocket Architecture

```
┌─────────────────┐     WS /ws (inbound)      ┌──────────────┐
│   Web UI (browser)│ ─────────────────────────▶│   ESP32      │
│   index.html      │                           │   AsyncWS    │
└─────────────────┘                            └──────┬───────┘
                                                      │ WebSocketsClient (outbound)
                                                      ▼
                                              ┌──────────────┐
                                              │   Spoolman   │
                                              │   WS server  │
                                              └──────────────┘
```
