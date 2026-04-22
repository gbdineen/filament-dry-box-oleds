
---

**ESP32 Spool Server** `http://<ESP32_IP>`

| Method | Endpoint | Body | Returns | Description |
|--------|----------|------|---------|-------------|
| `GET` | `/` | — | HTML | Serves the web interface |
| `GET` | `/slotorder` | — | `[1, 3]` | Drybox slot order from LittleFS |
| `POST` | `/slotorder` | `[3, 1]` | `{"ok":true}` | Save new Drybox slot order to LittleFS |
| `GET` | `/locations` | — | `{"value":"[\"Drybox\",\"Standby\"]",...}` | Proxied from Spoolman |
| `GET` | `/spools?location=X` | — | `[{...},{...}]` | Proxied from Spoolman |
| `POST` | `/movespool` | `{"id":3,"location":"Standby"}` | `{"ok":true}` | Patches spool location in Spoolman |

---

**Spoolman** `http://10.0.0.110:7912/api/v1/`

| Method | Endpoint | Body | Description |
|--------|----------|------|-------------|
| `GET` | `/spool?location=X` | — | Get spools by location |
| `PATCH` | `/spool/{id}` | `{"location":"Drybox"}` | Update spool location |
| `GET` | `/setting/locations` | — | Get all location names |
| `GET` | `/setting/locations_spoolorders` | — | Raw (dirty) spool order — avoid |