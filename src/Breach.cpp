#include <common.h>
#ifdef BREACH

#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

//////// Add new include library


////////  Define global constantes (ALWAYS IN MAJ, use pin number and not name)

const int SOUND_DETECTOR = 5;

const int LED = 4; // indicator led for debug mode (turn on and off when a sound is hear)

const unsigned long DELAY = 25; // soud detection delay

// pixels band for water effect
const float NUM_LEDS = 60; // i'v put a float here for a dividing after that
#define LED_PIN 13
#define BRIGHTNESS 200
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUM_LEDS, LED_PIN, NEO_GRBW + NEO_KHZ800);


const long INTERVAL = 1000; // small intervall for water effect

////////  Define global variables

int breach_lvl = 0; // breach lvl

unsigned long lastDetectionTime = 0; // sound detection counter

bool isLedOn = false; // state LED

unsigned long previousMillis = 0; // time marker for water leds

/////////////////////////////////////////////////////////////////////////////////////////////
//                                      User function                                      //
///////////////////////////////////////////////////////////////////////////////////////////

/// @brief Detect sound
void detect_sound()
{
    int output = digitalRead(SOUND_DETECTOR);

    if (output == LOW) // sound has been detect
    {
        unsigned long now = millis();
        if (now - lastDetectionTime >= DELAY) // the delay has passed
        {
            lastDetectionTime = now;
            comm.send("BRE;1");

            if (debug) // light on LED in DEBUG mode
            {
                isLedOn = !isLedOn;
                digitalWrite(LED, isLedOn); // change led state
            }

#ifdef LOG
            Serial.print("Son détecté dans le recepteur");
#endif
        }
    }
}


/// @brief Make a water effect on led's band depnding breach_lvl
void water_effect()
{
    unsigned long currentMillis = millis(); // Get the current time in milliseconds
    if (currentMillis - previousMillis >= INTERVAL)
    {
        previousMillis = currentMillis; // Update the previous time

        int new_num_leds = ((NUM_LEDS / 100) * breach_lvl);     //divide leds with breach lvl

        if (new_num_leds > 0)
        {
            ///@brief make wave effect in breachlvl function
            for (int i = 0; i <= new_num_leds; i++)
            {
                int blueValue = random(40, 256); // Randomly change the value of blue for each pixel
                int greenValue;
                if (blueValue < 80)
                {
                    greenValue = 0;
                }
                else
                {
                    greenValue = random(0, 50); // Randomly change the value of green for each pixel
                }

                pixels.setPixelColor(i, pixels.Color(0, greenValue, blueValue, 0)); // Set the color of each pixel
            }
        }

        /// @brief turn off the first led when new_num_leds is 0
        if (new_num_leds == 0)
        {
            pixels.clear();
            pixels.show();
        }
        else
        {
            ///@brief turn off leds depending breach lvl
            for (int j = (new_num_leds + 1); j < NUM_LEDS; j++)
            {
                pixels.setPixelColor(j, pixels.Color(0, 0, 0, 0));
            }
            pixels.show(); // Update the display
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

    pinMode(SOUND_DETECTOR, INPUT);

    breach_lvl = 0;
    pixels.begin();
    pixels.setBrightness(BRIGHTNESS);
    pixels.clear();
    pixels.show();
}

///////////////////////////////  Reset all proprety of module  ////////////////////////////////

/// @brief Call after the config and when the module reset by the app
void ResetModule()
{

        breach_lvl = 0;
}

/////////////////////////////////  Write here the loop code  /////////////////////////////////

/// @brief Call at the end of the main loop function
void MyLoop()
{
    detect_sound(); 
    water_effect();
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
        breach_lvl = i;
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