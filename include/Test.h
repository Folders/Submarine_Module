#include <common.h>
#ifdef TEST

#ifndef TEST_FILE_H
#define TEST_FILE_H

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


#endif // FILE_H
#endif // TEST