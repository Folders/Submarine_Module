#include <common.h>
#ifdef BREACH

#include <Arduino.h>

//////// Add new include library
// #include <Adafruit_NeoPixel.h>

////////  Define global constantes (ALWAYS IN MAJ, use pin number and not name)

const int SOUND_DETECTOR[] = {5};
const int NUMBEROFINPUTS = sizeof(SOUND_DETECTOR) / sizeof(SOUND_DETECTOR[0]);

const int LED = 4;

const unsigned long DELAY = 25; // soud detection delay

const int NUM_BREACH = 1;

////////  Define global variables

int breach_lvl[NUM_BREACH];

unsigned long lastDetectionTime = 0; // sound detection counter
bool isLedOn = false;                // state LED

/////////////////////////////////////////////////////////////////////////////////////////////
//                                      User function                                      //
/////////////////////////////////////////////////////////////////////////////////////////////

/// @brief Si je tape ///, il me propose de mettre des commentaires à la fonction
void my_function()
{
    // ...
}

/// @brief Detect sound
void detect_sound()
{

    for (int i = 0; i < NUMBEROFINPUTS; i++)
    {
        int output = digitalRead(SOUND_DETECTOR[i]);
        if (output == LOW) // sound has been detect
        {
            unsigned long now = millis();
            if (now - lastDetectionTime >= DELAY) // the delay has passed
            {
                lastDetectionTime = now;
                comm.start("BRE;");
                comm.add(i);
                comm.send();

                if (debug) // light on LED in DEBUG mode
                {
                    isLedOn = !isLedOn;
                    digitalWrite(LED, isLedOn); // change led state
                }

#ifdef LOG
                Serial.print("Son détecté dans le recepteur : ");
                Serial.print(i);
#endif
            }
        }
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////
//                                     Setup and reset                                     //
/////////////////////////////////////////////////////////////////////////////////////////////

/// @brief Setup function for the module
void MySetup()
{
// Suround every "Serial" order between "#ifdef LOG" and "#endif"
#ifdef LOG
    Serial.println("--- Model ---");
#endif
    for (int i = 0; i < NUMBEROFINPUTS; i++)
    {
        pinMode(SOUND_DETECTOR[i], INPUT);
    }

    for (int i = 0; i < NUM_BREACH; i++)
    {
        breach_lvl[i] = 0;
    }
}

///////////////////////////////  Reset all proprety of module  ////////////////////////////////

/// @brief Call after the config and when the module reset by the app
void ResetModule()
{
    for (int i = 0; i < NUM_BREACH; i++)
    {
        breach_lvl[i] = 0;
    }
}

/////////////////////////////////  Write here the loop code  /////////////////////////////////

/// @brief Call at the end of the main loop function
void MyLoop()
{
    detect_sound();
}

/////////////////////////////////////////////////////////////////////////////////////////////
//                                      Communication                                      //
/////////////////////////////////////////////////////////////////////////////////////////////

/// @brief Call when a message is received from server (or serial)
void Received()
{

    // If you want debug the received string, you can write this commande:
    comm.Info_Received();

    // The get the received code, use the function GetCode()
    if (comm.GetCode() == "TST")
    {
        // If you need to check the number of parameter available, use the function comm.GetSize()
        Serial.println("Number of received parameter :");
        Serial.println(comm.GetSize());

        // If you want to read one parameter, you can use the function comm.GetParameter(x)
        Serial.println("Value of parameter 1 :");
        Serial.println(comm.GetParameter(1));
    }

    if (comm.GetCode() == "BRE")
    {
        int i = comm.GetParameter(1).toInt();
        int y = comm.GetParameter(2).toInt();
        breach_lvl[i] = y;
    }
}

/// @brief When a message is send without server, the message will be received here. You can close the loop to test the module
void ServerSimulation()
{
    // Update navigation status
    if (comm.GetCode() == "NVC")
    {
        // DO something
        Serial.println("WOOW, j'ai reçu un NVC en local !");
    }
}

#endif