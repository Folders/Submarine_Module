#include <common.h>
#ifdef ELECTRICITY

#ifndef ELECTRICITY_FILE_H
#define ELECTRICITY_FILE_H

#include <Arduino.h>

/// @brief Setup function of module
void MySetup();

/// @brief Loop function of module
void MyLoop();

/// @brief Reset function of module
void StartModule();

/// @brief Stop function of module
void StopModule();

/// @brief Function called when a message is received
void Received();

/// @brief Function called when a message is send in close loop
void ServerSimulation();


#endif // FILE_H
#endif // ELECTRICITY