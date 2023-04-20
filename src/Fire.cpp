#include <common.h>
#ifdef FIRE

#include <Arduino.h>

//////// Add new include library
#include "FastLED.h"

////////  Define global constantes      (ALWAYS IN MAJ)
const int NUM_FIREPLACE = 3;
const int NUM_LEDS_PER_MATRIX = 64;

const int NUM_ROWS = sqrt(NUM_LEDS_PER_MATRIX);
const int NUM_COLS = sqrt(NUM_LEDS_PER_MATRIX);
const int NUM_LEDS = (NUM_FIREPLACE * NUM_LEDS_PER_MATRIX);

#define DATA_PIN 16    // Matrix data pin
#define BRIGHTNESS 155 // LED brightness

// Define the array of leds
CRGB leds[NUM_LEDS];

// define fire palette
DEFINE_GRADIENT_PALETTE(firepal){
    0, 0, 0, 0,        // black
    32, 255, 0, 0,     // red
    190, 255, 255, 0,  // yellow
    255, 255, 255, 255 // white
};

CRGBPalette16 myPal = firepal;

// set percent and row level depending on the number of fireplaces
int percent[NUM_FIREPLACE], row_level[NUM_FIREPLACE];

const int LED = 5; // indicator led when a IR signal is ON

// set Inputs for the IR receptors
// const int RECEPTORS_PINS[] = {14, 12, 13, 15, 4};
const int RECEPTORS_PINS[] = {14};
const int NUMBEROFINPUTS = sizeof(RECEPTORS_PINS) / sizeof(RECEPTORS_PINS[0]);

bool value[NUMBEROFINPUTS], old_value[NUMBEROFINPUTS], ir_comm[NUMBEROFINPUTS];

////////  Define global variables

/////////////////////////////////////////////////////////////////////////////////////////////
//                                      User function                                      //
/////////////////////////////////////////////////////////////////////////////////////////////

/// @brief Si je tape ///, il me propose de mettre des commentaires à la fonction
void my_function()
{
    // ...
}

uint8_t XY(uint8_t x, uint8_t y)
{
    return (y * NUM_COLS + x);
}

/// @brief make the fire effect with led with the fire lvl
/// @param fireplaceIndex
void runFire(int fireplaceIndex)
{
    int a = millis();
    for (int i = 0; i < NUM_COLS; i++)
    {
        for (int j = 0; j < NUM_ROWS; j++)
        {
            int temp2 = abs8(j - ((NUM_ROWS + row_level[fireplaceIndex]) - 1)) * 255 / ((NUM_ROWS + row_level[fireplaceIndex]) - 1);
            leds[XY(i, j + (fireplaceIndex * 8))] = ColorFromPalette(myPal, qsub8(inoise8(i * 60, j * 60 + a, a / 3), temp2), BRIGHTNESS);
        }
    }
}

/// @brief Turn on a LED if at least one IR receptor recieve something
void ir_indicator()
{
    int state = 0;
    for (int i = 0; i < NUMBEROFINPUTS; i++)
    {
        state += ir_comm[i];
    }

    digitalWrite(LED, state > 0 ? HIGH : LOW); // turn on led if at least 1 IR reciever is recepting
}

/////////////////////////////////////////////////////////////////////////////////////////////
//                                     Setup and reset                                     //
/////////////////////////////////////////////////////////////////////////////////////////////

/// @brief Setup function for the module
void MySetup()
{

    for (int i = 0; i < NUMBEROFINPUTS; i++)
    {
        pinMode(RECEPTORS_PINS[i], INPUT);
        value[i] = 0;
        old_value[i] = 0;
        ir_comm[i] = false;
    }

    pinMode(LED, OUTPUT);

    FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);
    FastLED.setBrightness(BRIGHTNESS);

    for (int i = 0; i < NUM_FIREPLACE; i++)
    {
        percent[i] = 0;
        row_level[i] = 0;
    }
}

///////////////////////////////  Reset all proprety of module  ////////////////////////////////

/// @brief Call after the config and when the module reset by the app
void ResetModule()
{
    for (int i = 0; i < NUM_FIREPLACE; i++)
    {
        percent[i] = 0;
        row_level[i] = 0;
    }

    for (int i = 0; i < NUMBEROFINPUTS; i++)
    {
        value[i] = 0;
        old_value[i] = 0;
        ir_comm[i] = false;
    }
}

/////////////////////////////////  Write here the loop code  /////////////////////////////////

/// @brief Call at the end of the main loop function
void MyLoop()
{
    /*
    // To send datas to the server, use the send function
    comm.send("LED;R;1");

    // It's possible to send with more then on line
    comm.start();    // Open the buffer
    comm.add("LED"); // Write String
    comm.add(';');   // Add char
     comm.add(testInt); // Add from variable
    comm.send(); // Send concatened variable
    */

    // changing the fire depending percent
    for (int i = 0; i < NUM_FIREPLACE; i++)
    {
        if (percent[i] == 0) // turn off the [i] matrix if the percent [i] = 0
        {
            fill_solid(&leds[NUM_LEDS_PER_MATRIX * i], NUM_LEDS_PER_MATRIX, CRGB::Black);
            FastLED.show();
        }
        if (percent[i] > 0) // start the fire effect on the matrix
        {
            EVERY_N_MILLISECONDS(5)
            {
                switch (percent[i]) // make a different fire lvl depending the percent
                {
                case 100:
                    row_level[i] = (pow((5 - 5), 2) - pow(((5 - 5) - 1), 2));
                    break;
                case 80:
                    row_level[i] = (pow((5 - 4), 2) - pow(((5 - 4) - 1), 2));
                    break;
                case 60:
                    row_level[i] = (pow((5 - 3), 2) - pow(((5 - 3) - 1), 2));
                    break;
                case 40:
                    row_level[i] = (pow((5 - 2), 2) - pow(((5 - 2) - 1), 2));
                    break;
                case 20:
                    row_level[i] = (pow((5 - 1), 2) - pow(((5 - 1) - 1), 2));
                    break;
                }
                runFire(i);
                FastLED.show();
            }
        }
    }

    // read the IR connection
    for (int i = 0; i < NUMBEROFINPUTS; i++)
    {
        value[i] = digitalRead(RECEPTORS_PINS[i]);

        if (value[i] && !old_value[i]) // IR connection
        {
            // send IR connection to server
            comm.start();
            comm.add("FIS;");
            comm.add(i);
            comm.send(";1");

#ifdef LOG
            Serial.println("FIS;");
            Serial.print(i);
            Serial.print(";1");
#endif

            ir_comm[i] = 1;
            ir_indicator();
            old_value[i] = 1;
        }
        else if (!value[i] && old_value[i]) // IR deconnection
        {
            // send IR deconnection to server
            comm.start();
            comm.add("FIS;");
            comm.add(i);
            comm.send(";0");

#ifdef LOG
            Serial.println("FIS;");
            Serial.print(i);
            Serial.print(";0");
#endif

            ir_comm[i] = 0;
            ir_indicator();
            old_value[i] = 0;
        }
    }
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

    if (comm.GetCode() == "FIR")
    {
        int i = comm.GetParameter(1).toInt();
        percent[i] = comm.GetParameter(2).toInt();
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