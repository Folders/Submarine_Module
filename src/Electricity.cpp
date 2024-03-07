#include <common.h>
#ifdef ELECTRICITY

#include <Arduino.h>

//////// Add new include library
#include <Bounce2.h>
#include <Ticker.h>

////////  Define global constantes      (ALWAYS IN MAJ)

// Set inputs
const int INPUTPINS[] = {2, 14, 12, 13, 4, 5}; // if there is 6 interrupts
const int NB_INPUT = sizeof(INPUTPINS) / sizeof(int);

Bounce buttons[NB_INPUT]; // using Bounce2 librairy

// Neopixels setup
#define NB_PIXELS NB_INPUT + 1 // insert the total of pixels

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

/// @brief During animation, lock new input
bool _inAnimation = false;
bool _clign;  // For animation
int _counter; // For animation

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
            // Set status led to GREEN
            pixels.setPixelColor(NB_INPUT, CRGB::Lime);
        else
        {
            // Set status led to RED
            pixels.setPixelColor(NB_INPUT, CRGB::Red);

            // Turn off every switch light
            for (int i = 0; i < NB_INPUT; i++)
            {
                pixels.setPixelColor(i, CRGB::Black); // turn off the led
            }
        }

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
    // For each button
    for (int i = 0; i < NB_INPUT; i++)
    {
        // Update button object
        buttons[i].update();

        if (buttons[i].fell()) // inetrrupt has been pressed
        {
            // if there is a breakdown or debug, send button state
            if ( (!_working || debug) && !_inAnimation)
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

            if (!_working && !_inAnimation) // turn green the matching led if there is a breakdown
            {
                pixels.setPixelColor(i, CRGB::Lime);
                pixels.show();
            }
        }

        // For debug
        if (debug && !_inAnimation)
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

/// @brief Victory animation (Call by ticker)
void Succes_Anim()
{
    // Update animation effect
    _clign = !_clign;
    _counter++;

    // Check the last clign effect
    if (_counter == (NUM_CLIGN * 2))
    {
        // Set color to every light
        for (int i = 0; i < NB_INPUT; i++)
        {
            pixels.setPixelColor(i, CRGB::Lime);
        }

        // Stop the ticker
        _Succes.detach();
        _inAnimation = false;
    }
    else
    {
        if (_clign == true)
        {
            // Turn off every light green
            for (int i = 0; i < NB_INPUT; i++)
            {
                pixels.setPixelColor(i, CRGB::Lime);
            }
        }

        if (_clign == false)
        {
            // Turn off every light
            for (int i = 0; i < NB_INPUT; i++)
            {
                pixels.setPixelColor(i, CRGB::Black); // turn off the led
            }
        }
    }

    // Update pixel
    pixels.show();
}

/// @brief The sequence is sucessful
void Succes()
{
#ifdef LOG
    // Log info
    Serial.println("Sequence: Succes");
#endif

    // Update module status
    SetWorking(true);

    // Init animation
    _clign = false;
    _counter = 0;
    _inAnimation = true;

    // Call first animation
    Succes_Anim();

    // Create the ticker for the reste of the animation
    _Succes.attach(0.5, Succes_Anim);
}

/// @brief Make the leds lighting red (fail)
void Fail_Anim()
{
    // Update animation effect
    _clign = !_clign;
    _counter++;

    // Check the last clign effect
    if (_counter == (NUM_CLIGN * 2))
    {
        // Set color to every light
        for (int i = 0; i < NB_INPUT; i++)
        {
            pixels.setPixelColor(i, CRGB::Black);
        }

        // Stop the ticker
        _Fail.detach();
        _inAnimation = false;
    }
    else
    {
        if (_clign == true)
        {
            // Turn off every light green
            for (int i = 0; i < NB_INPUT; i++)
            {
                pixels.setPixelColor(i, CRGB::Red);
            }
        }

        if (_clign == false)
        {
            // Turn off every light
            for (int i = 0; i < NB_INPUT; i++)
            {
                pixels.setPixelColor(i, CRGB::Black); // turn off the led
            }
        }
    }
    
    // Update pixel
    pixels.show();
}

/// @brief The sequence is faulty
void Fail()
{
#ifdef LOG
    // Log info
    Serial.println("Sequence: Fail");
#endif

    // Init animation
    _clign = false;
    _counter = 0;
    _inAnimation = true;

    // Call first animation
    Fail_Anim();

    // Create the ticker for the reste of the animation
    _Fail.attach(0.5, Fail_Anim);
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

    // Define pixels property
    pixels.addLeds(NB_PIXELS);
    //pixels.useInfoPixel();
}

///////////////////////////////  Reset all proprety of module  ////////////////////////////////

/// @brief Call after the config and when the module reset by the app
void ResetModule()
{
    /// Turn off all leds
    pixels.clear();

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
                pixels.setPixelColor(i, CRGB::Black); // turn led OFF
                break;

            case 'R':
                pixels.setPixelColor(i, Pixel(255, 0, 0)); // turn led red
                break;

            case 'G':
                pixels.setPixelColor(i, Pixel(0, 255, 0)); // turn led green
                break;

            case 'B':
                pixels.setPixelColor(i, Pixel(0, 0, 255)); // turn led blue
                break;

            case 'r':
                pixels.addVariator(i, CRGB::Red, CRGB::Black);
                break;

            case 'g':
                pixels.addVariator(i, CRGB::Green, CRGB::Black);
                break;

            case 'b':
                pixels.addVariator(i, CRGB::Blue, CRGB::Black);
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
            pixels.setPixelColor(i, Pixel(r, g, b)); // turn led blue
        }

        // Update pixels
        pixels.show();
    }

    if (comm.GetCode() == "POW") // recieve information from the server if it's a succes or a fail
    {
        switch (comm.GetParameter(1)[0])
        {
        case 'F': // fail
            Fail();
            break;

        case 'S': // succes
            Succes();
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
        pixels.setPixelColor(i, CRGB::Lime); // turn led green
        pixels.show();
    }
}

#endif