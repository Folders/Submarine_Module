#include <common.h>
#ifdef TORPEEDO

#include <Arduino.h>

//////// Add new include library
#include <Adafruit_NeoPixel.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Bounce2.h>
#include <FastLED.h>
#include <MFRC522.h>
#include <Ticker.h>

////////  Define global constantes      (ALWAYS IN MAJ)

// set interrupts Outputs
const int OUTPUTPINS[] = {26, 25, 33, 32, 17, 16};
const int NUMBEROFOUTPUTS = sizeof(OUTPUTPINS) / sizeof(OUTPUTPINS[0]);

// set Inputs for the interrupts
const int INPUTPINS[] = {18, 19, 21, 22};
const int NUMBEROFINPUTS = sizeof(INPUTPINS) / sizeof(INPUTPINS[0]);

// Torpeedo launch button
const int BUTTONPIN = 23;
Bounce button;

const int PIN = 27;
#define NUMPIXELS 4 // insert the total of pixels
Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

////  Define global variables

int btn_state[NUMBEROFINPUTS]{0}; // to save the position of interrupts
int read_direction = 0;           // the position which's reading

int torpeedo_range = 0;

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
// Suround every "Serial" order between "#ifdef DEBUG" and "#endif"
#ifdef DEBUG
    Serial.println("--- Model ---");
#endif

    // set interrupts outputs
    for (int i = 0; i < NUMBEROFOUTPUTS; i++)
    {
        pinMode(OUTPUTPINS[i], OUTPUT);
    }

    // set interrupts inputs
    for (int i = 0; i < NUMBEROFINPUTS; i++)
    {
        pinMode(INPUTPINS[i], INPUT);
    }

    // set Torpeedo btn
    pinMode(BUTTONPIN, INPUT);
    button.attach(BUTTONPIN, INPUT);
    button.interval(5);

    // Neo pixels
    pixels.begin();
    pixels.setBrightness(250);
    pixels.clear();
    pixels.show();
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
    /*
    // To send datas to the server, use the send function
    comm.send("LED;R;1");

    // It's possible to send with more then on line
    comm.start();      // Open the buffer
    comm.add("LED");   // Write String
    comm.add(';');     // Add char
    comm.add(testInt); // Add from variable
    comm.send();       // Send concatened variable
    */

    button.update();        //read button'state

    // button has been pressed
    if (button.rose()) 
    {
        // send interrupts directions to the server
        comm.start("TLN");

        for (int i = 0; i < NUMBEROFINPUTS; i++)
        {
            comm.add(";");
            comm.add(btn_state[i]);
        }
        comm.send();

#ifdef DEBUG
        Serial.println("TLN");
        for (int i = 0; i < NUMBEROFINPUTS; i++)
        {
            Serial.print(";");
            Serial.print(btn_state[i]);
        }
#endif
    }

    // turn on each output of all the interrupts (from 1 to 6)
    for (int j = 0; j < NUMBEROFOUTPUTS; j++)
    {
        digitalWrite(OUTPUTPINS[j], HIGH); // turn on the i output
        read_direction = j;

        // read each intput's interrupts
        for (int i = 0; i < NUMBEROFINPUTS; i++)
        {
            if (digitalRead(INPUTPINS[i]))      //there is a connection (interrupt is on a position)     
            {
                btn_state[i] = (read_direction + 1); // save the direction in the matrix
            }
        }
        digitalWrite(OUTPUTPINS[read_direction], LOW); // turn off the output
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

    //recieve torpeedo range from server and turn on lights
    if (comm.GetCode() == "TLV") 
    {
        // change torpeedo range
        switch (comm.GetParameter(1)[0])
        {
        case '0': 
            torpeedo_range = 0;
            break;
        case '1': 
            torpeedo_range = 1;
            break;
        case '2': 
            torpeedo_range = 2;
            break;
        case '3': 
            torpeedo_range = 3;
            break;
        case '4': 
            torpeedo_range = 4;
            break;
        }

        // turn off the Range LEDS
        for (int i = 0; i < NUMPIXELS; i++)
        {
            pixels.setPixelColor(i, pixels.Color(0, 0, 0));
            pixels.show();
        }

        // turn on the RANGE LEDS
        for (int i = 0; i < torpeedo_range; i++)
        {
            pixels.setPixelColor(i, pixels.Color(0, 255, 0));
            pixels.show();
        }
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