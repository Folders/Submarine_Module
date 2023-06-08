#include <common.h>
#ifdef OXYGEN

#include <Arduino.h>

//////// Add new include library
#include <Adafruit_NeoPixel.h>

////////  Define global constantes (ALWAYS IN MAJ, use pin number and not name)

const int POTPIN = A0;          // Analog pin used to read the potentiometer
const int UPPERTHRESHOLD = 900; // Upper threshold to detect upward movement
const int LOWERTHRESHOLD = 10;  // Lower threshold to detect downward movement

const int INCREMENTATION = 10; // number of Back-and-Forth neeeded to add oxygen

const int NUM_LEDS = 10;
#define LED_PIN 13
#define BRIGHTNESS 200
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUM_LEDS, LED_PIN, NEO_GRBW + NEO_KHZ800);

#define WORKALONE // add oxygen automaticly

////////  Define global variables

int potValue;           // Variable to store the read value from the potentiometer
bool previousState = 0; // Variable to store the previous state of the potentiometer
int counter = 0;        // Counter for the number of back-and-forth movements
int percent = 50;       // Oxygen percent

/////////////////////////////////////////////////////////////////////////////////////////////
//                                      User function                                      //
/////////////////////////////////////////////////////////////////////////////////////////////


/// @brief Read Back-and-Forth from the potentiometer
void read_moves()
{
    potValue = analogRead(POTPIN);

    // Detect upward movement
    if (potValue > UPPERTHRESHOLD && previousState == 0)
    {
        counter++;
        previousState = 1;

#ifdef LOG
        Serial.print("Back-and-Forth: ");
        Serial.println(counter);
#endif
    }
    // Detect downward movement
    else if (potValue < LOWERTHRESHOLD && previousState == 1)
    {
        previousState = 0;
    }
}

/// @brief Send to server when the counter reach the right number for adding oxygen
void send_server()
{
    if (counter >= INCREMENTATION)
    {
        comm.send("OXY;1");
        counter = 0;

#ifdef WORKALONE

        if (percent <= 90)
        {
            percent = percent + 10;
#ifdef LOG
            Serial.print("Augmentation du pourcentage. Nouveau pourcentage : ");
            Serial.println(percent);
#endif
        }
        else if (percent < 100 && percent > 90)
        {
            percent = 100;
#ifdef LOG
            Serial.print("Augmentation du pourcentage. Nouveau pourcentage : ");
            Serial.println(percent);
#endif
        }

#endif

    }
}

/// @brief display oxygen percent with leds and red to green effect
void display_percent()
{
    for (int i = 0; i < NUM_LEDS; i++)
    {
        pixels.setPixelColor(i, pixels.Color(((10 - i) * 25), (i * 25), 0, 0));
    }

    /// @brief Turn off the leds depending percent
    int ledpercent = percent / NUM_LEDS;
    for (int j = (ledpercent); j < NUM_LEDS; j++)
    {
        pixels.setPixelColor(j, pixels.Color(0, 0, 0, 0));
    }

    pixels.show();
}

/////////////////////////////////////////////////////////////////////////////////////////////
//                                     Setup and reset                                     //
/////////////////////////////////////////////////////////////////////////////////////////////


/// @brief Setup function for the module
void MySetup()
{
    pixels.begin();
    pixels.setBrightness(BRIGHTNESS);
    pixels.clear();
    pixels.show();
}

///////////////////////////////  Reset all proprety of module  ////////////////////////////////

/// @brief Call after the config and when the module reset by the app
void ResetModule()
{
    percent = 50 ;
}

/////////////////////////////////  Write here the loop code  /////////////////////////////////

/// @brief Call at the end of the main loop function
void MyLoop()
{
    read_moves();
    send_server();
    display_percent();
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

    // get oxygen percent
    if (comm.GetCode() == "OXP")
    {
        percent = comm.GetParameter(1).toInt();
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