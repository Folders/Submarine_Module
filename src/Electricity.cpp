#include <common.h>
#ifdef ELECTRICITY

#include <Arduino.h>

//////// Add new include library
#include <Adafruit_NeoPixel.h>
#include <Bounce2.h>
#include <Ticker.h>

////////  Define global constantes      (ALWAYS IN MAJ)

// Set inputs
const int INPUTPINS[] = {2, 14, 12, 13, 4, 5}; // if there is 6 interrupts
const int NB_INPUT = sizeof(INPUTPINS) / sizeof(int);

Bounce buttons[NB_INPUT]; // using Bounce2 librairy

// Neopixels setup
#define PIN 0
#define NB_PIXELS NB_INPUT + 1 // insert the total of pixels
Adafruit_NeoPixel pixels(NB_PIXELS, PIN, NEO_GRB + NEO_KHZ800);

/// @brief number of clign for succes or fail
const int NUM_CLIGN = 4;

////////  Define global variables
// State tickers

/// @brief ticker for succes
Ticker _Succes;

/// @brief ticker for fail
Ticker _Fail;

/// @brief Electricity is working
bool _working;

bool clign_succes;
bool clign_fail;

int victory_counter;
int fail_counter;

/////////////////////////////////////////////////////////////////////////////////////////////
//                                      User function                                      //
/////////////////////////////////////////////////////////////////////////////////////////////

/// @brief Update working status
/// @param status New status
void SetWorking(bool status)
{
    // Check if update
    if (_working != status)
    {
        // Save value
        _working = status;

#ifdef LOG
        if (_working)
            Serial.println("Electricity status: Working");
        else
            Serial.println("Electricity status: Shutdown");
#endif

        // Update status led
        if (_working)
            pixels.setPixelColor(NB_INPUT, pixels.Color(0, 255, 0));
        else
            pixels.setPixelColor(NB_INPUT, pixels.Color(255, 0, 0));
            
        // Update pixels
        pixels.show();
    }
}

/// @brief Update working status
/// @param status New status
void SetWorking(String status)
{
    if (status == "1")
        SetWorking(true);
    else if (status == "0")
        SetWorking(false);
}

/// @brief Read interrupts and send to server if pressed
void interrupts_read()
{
    for (int i = 0; i < NB_INPUT; i++)
    {

        buttons[i].update();

        if (buttons[i].fell()) // inetrrupt has been pressed
        {
            // if there is a breakdown or debug, send button state
            if (!_working || debug)
            {
                // send information to server
                comm.start("BTN;");
                comm.add(i + 1);
                comm.send(";1");
            }

#ifdef LOG
            Serial.print("Boutton ");
            Serial.print(i + 1);
            Serial.print(" : Appuyé");
            Serial.println();
#endif

            if (!_working) // turn green the matching led if there is a breakdown
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
                comm.add(i + 1);
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

    for (int i = 0; i < NB_INPUT; i++)
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
        for (int i = 0; i < NB_INPUT; i++)
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

    for (int i = 0; i < NB_INPUT; i++)
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
        for (int i = 0; i < NB_INPUT; i++)
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
#ifdef LOG
    Serial.println();
    Serial.print("Number of button :");
    Serial.println(NB_INPUT);
#endif

    // set inputs
    for (int i = 0; i < NB_INPUT; i++)
    {
        buttons[i].attach(INPUTPINS[i], INPUT_PULLUP);
        buttons[i].interval(5);
    }

    // Neo pixels
    pixels.begin();
    pixels.setBrightness(50);
    pixels.clear();
    pixels.show();
}

///////////////////////////////  Reset all proprety of module  ////////////////////////////////

/// @brief Call after the config and when the module reset by the app
void ResetModule()
{
    /// Turn off all leds
    pixels.clear();
    pixels.show();

    SetWorking(true);
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
    if (comm.GetCode() == "LED")
    {
        // Get index of light
        int i = comm.GetParameter(1).toInt() - 1;

        // Check if index is in range
        if (i < 0 || i >= NB_INPUT)
            return;

        // If we receive juste an other parameter, apply the color depending of the char
        if (comm.GetSize() == 2)
        {
            switch (comm.GetParameter(2)[0])
            {
            case '0':
                pixels.setPixelColor(i, pixels.Color(0, 0, 0)); // turn led OFF
                break;

            case 'R':
                pixels.setPixelColor(i, pixels.Color(255, 0, 0)); // turn led red
                break;

            case 'G':
                pixels.setPixelColor(i, pixels.Color(0, 255, 0)); // turn led green
                break;

            case 'B':
                pixels.setPixelColor(i, pixels.Color(0, 0, 255)); // turn led blue
                break;
            }
        }

        // If we receive 3 parameter, apply the received color
        if (comm.GetSize() == 4)
        {
            // Load color
            int r = comm.GetParameter(2).toInt();
            int g = comm.GetParameter(3).toInt();
            int b = comm.GetParameter(4).toInt();

            // Set received color
            pixels.setPixelColor(i, pixels.Color(r,g,b)); // turn led blue
        }

        // Update pixels
        pixels.show();
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
        SetWorking(comm.GetParameter(1));
    }
}

/// @brief When a message is send without server, the message will be received here. You can close the loop to test the module
void ServerSimulation()
{

    // If i receive a button message
    if (comm.GetCode() == "BTN")
    {
        // Get index of light
        int i = comm.GetParameter(1).toInt() - 1;

        // Check if index is in range
        if (i < 0 || i >= NB_INPUT)
            return;

        // Check if we need to turn on
        if (comm.GetParameterInChar(2)[0] != '1')
            return;

        // Update the led
        pixels.setPixelColor(i, pixels.Color(0, 255, 0)); // turn led green
        pixels.show();
    }
}

#endif