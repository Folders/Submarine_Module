#include <common.h>
#ifdef DIALOGUE

#include <Arduino.h>

//////// Add new include library
#include <Wire.h>
#include <Adafruit_MCP23X17.h>
#include <Bounce2.h>

////////  Define global constantes (ALWAYS IN MAJ, use pin number and not name)

/// MCP23017 setup///

// MCP23017 I2C Adresses
#define MCP23017_ADDRESS1 0x20
#define MCP23017_ADDRESS2 0x21
#define MCP23017_ADDRESS3 0x22
#define MCP23017_ADDRESS4 0x23

Adafruit_MCP23X17 mcp1;
Adafruit_MCP23X17 mcp2;
Adafruit_MCP23X17 mcp3;
Adafruit_MCP23X17 mcp4;

/// button setup///

// Create Bounce objects for each button
Bounce button1 = Bounce();
Bounce button2 = Bounce();
Bounce button3 = Bounce();
Bounce button4 = Bounce();

#define BUTTON_SKIP D5
Bounce buttonSKIP = Bounce();

////////  Define global variables

bool choice_made ; 

/////////////////////////////////////////////////////////////////////////////////////////////
//                                      User function                                      //
/////////////////////////////////////////////////////////////////////////////////////////////

/// @brief Si je tape ///, il me propose de mettre des commentaires à la fonction
void my_function()
{
    // ...
}

/// @brief Read interrupts and send comm to server
void interrupts_read()
{
    button1.update();
    button2.update();
    button3.update();
    button4.update();
    buttonSKIP.update();

    if (button1.fell())
    {
        comm.send("BTN;0;1");
        choice_made == true; 
    }

    if (button2.fell())
    {
        comm.send("BTN;1;1");
         choice_made == true; 
    }

    if (button3.fell())
    {
        comm.send("BTN;2;1");
         choice_made == true; 
    }

    if (button4.fell())
    {
        comm.send("BTN;3;1");
         choice_made == true; 
    }

    if (buttonSKIP.fell())
    {
        comm.send("BTN;4;1");
         choice_made == true; 
    }

    // important de continuer à lire jusqu'à ce qu'un bouton soit pressé !
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

    // Initialize I2C communication
    Wire.begin();

    // Initialize MCP23017s
    mcp1.begin_I2C(MCP23017_ADDRESS1);
    mcp2.begin_I2C(MCP23017_ADDRESS2);
    mcp3.begin_I2C(MCP23017_ADDRESS3);
    mcp4.begin_I2C(MCP23017_ADDRESS4);

    // Configure GPA0 pins of MCP23017s as inputs
    mcp1.pinMode(0, INPUT);
    mcp2.pinMode(0, INPUT);
    mcp3.pinMode(0, INPUT);
    mcp4.pinMode(0, INPUT);

    button1.attach(mcp1.readGPIOAB(), 0);
    button1.interval(5);
    button2.attach(mcp2.readGPIOAB(), 0);
    button2.interval(5);
    button3.attach(mcp3.readGPIOAB(), 0);
    button3.interval(5);
    button4.attach(mcp4.readGPIOAB(), 0);
    button4.interval(5);

    // initialise SKIP button
    pinMode(BUTTON_SKIP, INPUT_PULLUP);
    buttonSKIP.attach(BUTTON_SKIP);
    buttonSKIP.interval(5);
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