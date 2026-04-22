#pragma once

#include <functional>
#include <vector>

void setupSpoolServer();
void setOrderUpdateCallback(std::function<void(std::vector<int>)> cb);