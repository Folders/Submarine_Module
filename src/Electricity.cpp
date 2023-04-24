#include <common.h>
#ifdef ELECTRICITY

#include <Arduino.h>

//////// Add new include library
#include <Adafruit_NeoPixel.h>
#include <Bounce2.h>
#include <Ticker.h>

////////  Define global constantes      (ALWAYS IN MAJ)

// Set inputs
// const int INPUTPINS[] = {15, 14, 12, 13, 4, 5};      // if there is 6 interrupts
const int INPUTPINS[] = {15, 14, 12, 13}; // if there is 4 interrupts
const int NUMBEROFINPUTS = sizeof(INPUTPINS) / sizeof(INPUTPINS[0]);

Bounce buttons[NUMBEROFINPUTS]; // using Bounce2 librairy

// Neopixels setup
#define PIN 0
#define NUMPIXELS 6 // insert the total of pixels
Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRBW + NEO_KHZ800);

/// @brief number of clign for succes or fail
const int NUM_CLIGN = 4;

////////  Define global variables
// State tickers

/// @brief ticker for succes
Ticker _Succes;

/// @brief ticker for fail
Ticker _Fail;

/// @brief If there is a breakdown or not
bool breackdown;

bool clign_succes;
bool clign_fail;


int victory_counter;
int fail_counter;

/////////////////////////////////////////////////////////////////////////////////////////////
//                                      User function                                      //
/////////////////////////////////////////////////////////////////////////////////////////////

/// @brief Read interrupts and send to server if pressed
void interrupts_read()
{
    for (int i = 0; i < NUMBEROFINPUTS; i++)
    {
        
        buttons[i].update();

        if (buttons[i].fell()) // inetrrupt has been pressed
        {
            // if there is a breakdown or debug, send button state
            if (breackdown || debug)
            {
                // send information to server
                comm.start("BTN;");
                comm.add(i);
                comm.send(";1");
            }

#ifdef LOG
            Serial.print("Boutton ");
            Serial.print(i);
            Serial.print(" : Appuyé");
            Serial.println();
#endif

            if (breackdown == true) // turn green the matching led if there is a breakdown
            {
                pixels.setPixelColor(i, pixels.Color(0, 255, 0, 0));
                pixels.show();
            }
        }

        // For debug
        if (debug)
        {
            if (buttons[i].rose()) // inetrrupt has been released
            {
                // send information to server
                comm.start("BTN;");
                comm.add(i);
                comm.send(";0");
            }
        }
    }
}

/// @brief Make the leds lighting green (succes)
void victory()
{
    clign_succes = !clign_succes; // for making a clign effect

    victory_counter++;

    for (int i = 0; i < NUMPIXELS; i++)
    {
        if (clign_succes == true)
        {
            pixels.setPixelColor(i, pixels.Color(0, 255, 0, 0)); // turn green the led
        }

        if (clign_succes == false)
        {
            pixels.setPixelColor(i, pixels.Color(0, 0, 0, 0)); // turn off the led
        }

        pixels.show();
    }

    if (victory_counter == (NUM_CLIGN * 2)) // the last clign effect
    {
        for (int i = 0; i < NUMPIXELS; i++)
        {
            pixels.setPixelColor(i, pixels.Color(0, 0, 0, 0)); // turn off the led
            pixels.show();
        }

        _Succes.detach();    // stop the ticker
        victory_counter = 0; // reset counter
    }
}

/// @brief Make the leds lighting red (fail)
void fail()
{
    clign_fail = !clign_fail; // for making a clign effect
    fail_counter++;

    for (int i = 0; i < NUMPIXELS; i++)
    {
        if (clign_fail == true)
        {
            pixels.setPixelColor(i, pixels.Color(255, 0, 0, 0)); // turn red the led
        }

        if (clign_fail == false)
        {
            pixels.setPixelColor(i, pixels.Color(0, 0, 0, 0)); // turn off the led
        }

        pixels.show();
    }

    if (fail_counter == (NUM_CLIGN * 2)) // the last clign effect
    {
        for (int i = 0; i < NUMPIXELS; i++)
        {
            pixels.setPixelColor(i, pixels.Color(0, 0, 0, 0)); // turn off the led
            pixels.show();
        }

        _Fail.detach();   // stop the ticker
        fail_counter = 0; // reset the counter
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////
//                                     Setup and reset                                     //
/////////////////////////////////////////////////////////////////////////////////////////////

/// @brief Setup function for the module
void MySetup()
{
    // set inputs
    for (int i = 0; i < NUMBEROFINPUTS; i++)
    {
        pinMode(INPUTPINS[i], INPUT);
        buttons[i].attach(INPUTPINS[i], INPUT);
        buttons[i].interval(5);
    }

    // Neo pixels
    pixels.begin();
    pixels.setBrightness(50);
    pixels.clear();
    pixels.show();

    breackdown = false;
}

///////////////////////////////  Reset all proprety of module  ////////////////////////////////

/// @brief Call after the config and when the module reset by the app
void ResetModule()
{
    /// Turn off all leds
    pixels.clear();
    pixels.show();

    breackdown = false; // no breackdown
}

/////////////////////////////////  Write here the loop code  /////////////////////////////////
/// @brief Call at the end of the main loop function
void MyLoop()
{
    interrupts_read(); // read button's states
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

    if (comm.GetCode() == "POW") // recieve information from the server if it's a succes or a fail
    {
        switch (comm.GetParameter(1)[0])
        {
        case 'F': // fail

#ifdef LOG
            Serial.println("Fail");
#endif

            _Fail.attach(0.7, fail);
            break;

        case 'S': // succes

#ifdef LOG
            Serial.println("Succes");
#endif

            _Succes.attach(0.7, victory);
            break;
        }
    }

    if (comm.GetCode() == "BRN") // recieve the information of a breackdown
    {
        breackdown = comm.GetParameter(1).toInt();
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