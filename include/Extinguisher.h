#include <common.h>
#ifdef EXTINGUISHER

#ifndef EXTINGUISHER_FILE_H
#define EXTINGUISHER_FILE_H

#include <Arduino.h>

/// @brief Setup function of module
void MySetup();

/// @brief Reset function of module
void ResetModule();

/// @brief Loop function of module
void MyLoop();

/// @brief Function called when a message is received
void Received();

/// @brief Function called when a message is send in close loop
void ServerSimulation();

Ticker _Clign;
Ticker _Animation;

Ticker _Clign_1;
Ticker _Animation_1;


#endif // FILE_H
#endif // EXTINGUISHER