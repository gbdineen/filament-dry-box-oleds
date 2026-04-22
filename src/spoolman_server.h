#pragma once

#include <Arduino.h>
#include <functional>
#include <vector>

// Call once after WiFi connects
void setupSpoolServer();

// Register a callback to be notified when the web UI sends a new slot order
// Callback receives the new order as a vector of spool IDs e.g. {1, 3}
void setOrderUpdateCallback(std::function<void(std::vector<int>)> cb);
