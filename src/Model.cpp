#include <common.h>
#ifdef MODEL

#include <Arduino.h>

//////// Add new include library
//#include <Adafruit_NeoPixel.h>

////////  Define global constantes (ALWAYS IN MAJ, use pin number and not name)
// const int TEST_IN = 10;
// const int TEST_OUT = 11;

////////  Define global variables
int _testInt = 0;
bool _testBool = 0;


/////////////////////////////////////////////////////////////////////////////////////////////
//                                      User function                                      //
/////////////////////////////////////////////////////////////////////////////////////////////

/// @brief Si je tape ///, il me propose de mettre des commentaires à la fonction
void my_function()
{
    // ...
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
}


///////////////////////////////  Reset all proprety of module  ////////////////////////////////

/// @brief Call after the config and when the module reset by the app
void ResetModule()
{
}


/////////////////////////////////  Write here the loop code  /////////////////////////////////

/// @brief Call at the end of the main loop function
void MyLoop()
{
    // To send datas to the server, use the send function
    comm.send("LED;R;1");

    // It's possible to send with more then on line
    comm.start();      // Open the buffer
    comm.add("LED");   // Write String
    comm.add(';');     // Add char
    comm.add(testInt); // Add from variable
    comm.send();       // Send concatened variable
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