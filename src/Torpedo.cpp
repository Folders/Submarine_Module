#include <common.h>
#ifdef TORPEDO

#include <Arduino.h>

//////// Add new include library
#include <Adafruit_NeoPixel.h>
#include <Bounce2.h>

////////  Define global constantes      (ALWAYS IN MAJ)

// set interrupts Outputs
const int OUTPUTPINS[] = {26, 25, 33, 32, 17, 16};
const int NUMBEROFOUTPUTS = sizeof(OUTPUTPINS) / sizeof(OUTPUTPINS[0]);

// set Inputs for the interrupts
const int INPUTPINS[] = {22, 21, 19, 18};
const int NUMBEROFINPUTS = sizeof(INPUTPINS) / sizeof(INPUTPINS[0]);

// Torpeedo launch button
const int BUTTONPIN = 23;
Bounce button;

const int PIN = 27;
#define NUMPIXELS 4    // insert the total of pixels
#define BRIGHTNESS 150 // leds brightness
Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

////  Define global variables

///@brief to save the position of interrupts
int btn_state[NUMBEROFINPUTS]{0};

///@brief to compare btn position and see if there is a changing position
int old_btn_state[NUMBEROFINPUTS]{0}; //

/// @brief direction which is reading
int read_direction = 0;

/// @brief torpedo range
int torpeedo_range = 0;

/////////////////////////////////////////////////////////////////////////////////////////////
//                                      User function                                      //
/////////////////////////////////////////////////////////////////////////////////////////////

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

    // Set Neo pixels
    pixels.begin();
    pixels.setBrightness(BRIGHTNESS);
    pixels.clear();
    pixels.show();
}

///////////////////////////////  Reset all proprety of module  ////////////////////////////////

/// @brief Call after the config and when the module reset by the app
void ResetModule()
{
    pixels.clear();
    pixels.show();
}

/////////////////////////////////  Write here the loop code  /////////////////////////////////

/// @brief Call at the end of the main loop function
void MyLoop()
{
    button.update(); // read button'state

    // button has been pressed
    if (button.rose())
    {
        // send interrupts directions to the server
        comm.start("TLN");

        // write each btn state
        for (int i = 0; i < NUMBEROFINPUTS; i++)
        {
            comm.add(";");
            comm.add(btn_state[i]);
        }
        comm.send();

#ifdef LOG
        Serial.print("TLN");
        for (int i = 0; i < NUMBEROFINPUTS; i++)
        {
            Serial.print(";");
            Serial.print(btn_state[i]);
        }
#endif
    }

    if (debug)
    {   
        // send that the button has been released
        if (button.fell())
        {
            comm.send("TLN;0");
        }
    }

    // turn on each output of all the interrupts (from 1 to 6)
    for (int j = 0; j < NUMBEROFOUTPUTS; j++)
    {
        digitalWrite(OUTPUTPINS[j], HIGH); // turn on the i output
        read_direction = j;

        // read each intput's interrupts
        for (int i = 0; i < NUMBEROFINPUTS; i++)
        {
            if (digitalRead(INPUTPINS[i])) // there is a connection (interrupt is on a position)
            {
                btn_state[i] = (read_direction); // save the direction in the matrix
            }
        }
        digitalWrite(OUTPUTPINS[read_direction], LOW); // turn off the output
    }

    ///@brief send to server if a button is changing direction
    if (debug)
    {
        for (int i = 0; i < NUMBEROFINPUTS; i++)
        {
            if (old_btn_state[i] != btn_state[i])
            {
                comm.start("TLN");
                for (int i = 0; i < NUMBEROFINPUTS; i++)
                {
                    comm.add(";");
                    comm.add(btn_state[i]);
                }
                comm.send();

                old_btn_state[i] = btn_state[i];
            }
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

    // recieve torpeedo range from server and turn on lights
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
        pixels.clear();
        pixels.show();

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