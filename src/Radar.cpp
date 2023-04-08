#include <common.h>
#ifdef RADAR

#include <Arduino.h>

//////// Add new include library
#include <Adafruit_NeoPixel.h>
#include <Ticker.h>

////////  Define global constantes      (ALWAYS IN MAJ)
#define PIN 15
#define NUMPIXELS 104 // insert the total of pixels

Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

////////  Define global variables
int testInt = 0;
bool testBool = 0;

// settings
float fadding = (85 / 100.0);
float rotation_speed = 0.05;
int brightness = 250; // max 255

// numbers of circles leds
int Nb_Leds_Circle_Sml = 24;
int Nb_Leds_Circle_Med = 35;
float Nb_Leds_Circle_Big = 45; // need to be a float for the dividing !

// arrays for the brightness, modified inside the code
float bright_R[104]{};
float bright_G[104]{};
float bright_B[104]{};

// arrays for the color, modified by the UDP or Serial
float color_R[104]{};
float color_G[104]{};
float color_B[104]{};

int RotaLed = (NUMPIXELS - Nb_Leds_Circle_Big);

Ticker _Fadded;

bool light_on_off = true; // for turning off the leds

/////////////////////////////////////////////////////////////////////////////////////////////
//                                      User function                                      //
/////////////////////////////////////////////////////////////////////////////////////////////

/// @brief Si je tape ///, il me propose de mettre des commentaires à la fonction

void green(int i) // water
{
    color_R[i] = 0;
    color_G[i] = 1;
    color_B[i] = 0;
}

void blue(int i) // treasure
{
    color_R[i] = 0;
    color_G[i] = 0;
    color_B[i] = 1;
}

void red(int i) // enemy or torpeedo
{
    color_R[i] = 1;
    color_G[i] = 0;
    color_B[i] = 0;
}

void orange(int i) // land
{
    color_R[i] = 1.2;
    color_G[i] = 0.5;
    color_B[i] = 0;
}

/// @brief Light on all the leds
void light_leds()
{
    for (int i = 0; i < NUMPIXELS; i++)
    {
        pixels.setPixelColor(i, pixels.Color((((bright_R[i]) * color_R[i]) * light_on_off), (((bright_G[i]) * color_G[i]) * light_on_off), (((bright_B[i]) * color_B[i]) * light_on_off)));
    }
    pixels.show();
}


void faddingAll()
{
    int RotaLed_2 = (((RotaLed - (NUMPIXELS - Nb_Leds_Circle_Big)) / (Nb_Leds_Circle_Big / Nb_Leds_Circle_Med)) + Nb_Leds_Circle_Sml);
    int RotaLed_3 = (((RotaLed - (NUMPIXELS - Nb_Leds_Circle_Big)) / (Nb_Leds_Circle_Big / Nb_Leds_Circle_Sml)));

    for (int i = 0; i < NUMPIXELS; i++) // fadding R
    {
        bright_R[i] = (bright_R[i] * fadding); // decrement all leds values
        if (bright_R[i] < 5)
        {
            bright_R[i] = 0;
        }
    }

    for (int i = 0; i < NUMPIXELS; i++) // fadding G
    {
        bright_G[i] = ((bright_G[i]) * fadding); // decrement all leds values
        if (bright_G[i] < 5)
        {
            bright_G[i] = 0;
        }
    }

    for (int i = 0; i < NUMPIXELS; i++) // fadding B
    {
        bright_B[i] = (bright_B[i] * fadding); // decrement all leds values
        if (bright_B[i] < 5)
        {
            bright_B[i] = 0;
        }
    }

    bright_R[RotaLed] = 150.0; // set the rotating leds at full red brightness
    bright_R[RotaLed_2] = 150.0;
    bright_R[RotaLed_3] = 150.0;

    bright_G[RotaLed] = 150.0; // set the rotating leds at full green brightness
    bright_G[RotaLed_2] = 150.0;
    bright_G[RotaLed_3] = 150.0;

    bright_B[RotaLed] = 150.0; // set the rotating leds at full blue brightness
    bright_B[RotaLed_2] = 150.0;
    bright_B[RotaLed_3] = 150.0;

    light_leds(); // light on all leds

    RotaLed++;

    if (RotaLed > NUMPIXELS)
    {
        RotaLed = (NUMPIXELS - Nb_Leds_Circle_Big); // set the limits of the lasrt rings
    }
}

// change the colors of the specifics small circle leds
void light_smallcircle_leds(int i, float r, float g, float b)
{
    switch (i)
    {
    case 0:
        for (int i = 0; i < 3; i++)
        {
            color_R[i] = r;
            color_G[i] = g;
            color_B[i] = b;
        }

        color_R[23] = r;
        color_G[23] = g;
        color_B[23] = b;

        break;

    case 1:
        for (int i = 3; i < 7; i++)
        {
            color_R[i] = r;
            color_G[i] = g;
            color_B[i] = b;
        }

        break;

    case 2:
        for (int i = 7; i < 11; i++)
        {
            color_R[i] = r;
            color_G[i] = g;
            color_B[i] = b;
        }

        break;

    case 3:
        for (int i = 11; i < 15; i++)
        {
            color_R[i] = r;
            color_G[i] = g;
            color_B[i] = b;
        }

        break;

    case 4:
        for (int i = 15; i < 19; i++)
        {
            color_R[i] = r;
            color_G[i] = g;
            color_B[i] = b;
        }

        break;

    case 5:
        for (int i = 19; i < 23; i++)
        {
            color_R[i] = r;
            color_G[i] = g;
            color_B[i] = b;
        }

        break;
    }
}

// change the colors of the specifics medium circle leds
void light_mediumcircle_leds(int i, float r, float g, float b)
{
    switch (i)
    {
    case 0:
        for (int i = 24; i < 26; i++)
        {
            color_R[i] = r;
            color_G[i] = g;
            color_B[i] = b;
        }

        color_R[58] = r;
        color_G[58] = g;
        color_B[58] = b;

        break;

    case 1:
        for (int i = 26; i < 29; i++)
        {
            color_R[i] = r;
            color_G[i] = g;
            color_B[i] = b;
        }

        break;

    case 2:
        for (int i = 29; i < 32; i++)
        {
            color_R[i] = r;
            color_G[i] = g;
            color_B[i] = b;
        }

        break;

    case 3:
        for (int i = 32; i < 35; i++)
        {
            color_R[i] = r;
            color_G[i] = g;
            color_B[i] = b;
        }

        break;

    case 4:
        for (int i = 35; i < 38; i++)
        {
            color_R[i] = r;
            color_G[i] = g;
            color_B[i] = b;
        }

        break;

    case 5:
        for (int i = 38; i < 41; i++)
        {
            color_R[i] = r;
            color_G[i] = g;
            color_B[i] = b;
        }

        break;

    case 6:
        for (int i = 41; i < 44; i++)
        {
            color_R[i] = r;
            color_G[i] = g;
            color_B[i] = b;
        }

        break;

    case 7:
        for (int i = 44; i < 47; i++)
        {
            color_R[i] = r;
            color_G[i] = g;
            color_B[i] = b;
        }

        break;

    case 8:
        for (int i = 47; i < 50; i++)
        {
            color_R[i] = r;
            color_G[i] = g;
            color_B[i] = b;
        }

        break;

    case 9:
        for (int i = 50; i < 53; i++)
        {
            color_R[i] = r;
            color_G[i] = g;
            color_B[i] = b;
        }

        break;

    case 10:
        for (int i = 53; i < 56; i++)
        {
            color_R[i] = r;
            color_G[i] = g;
            color_B[i] = b;
        }

        break;

    case 11:
        for (int i = 56; i < 58; i++)
        {
            color_R[i] = r;
            color_G[i] = g;
            color_B[i] = b;
        }

        break;
    }
}

// change the colors of the specifics big circle leds
void light_bigcircle_leds(int i, float r, float g, float b)
{
    switch (i)
    {
    case 0:
        for (int i = 59; i < 61; i++)
        {
            color_R[i] = r;
            color_G[i] = g;
            color_B[i] = b;
        }

        color_R[103] = r;
        color_G[103] = g;
        color_B[103] = b;

        break;

    case 1:
        for (int i = 61; i < 63; i++)
        {
            color_R[i] = r;
            color_G[i] = g;
            color_B[i] = b;
        }

        break;

    case 2:
        for (int i = 63; i < 66; i++)
        {
            color_R[i] = r;
            color_G[i] = g;
            color_B[i] = b;
        }

        break;

    case 3:
        for (int i = 66; i < 68; i++)
        {
            color_R[i] = r;
            color_G[i] = g;
            color_B[i] = b;
        }

        break;

    case 4:
        for (int i = 68; i < 71; i++)
        {
            color_R[i] = r;
            color_G[i] = g;
            color_B[i] = b;
        }

        break;

    case 5:
        for (int i = 71; i < 73; i++)
        {
            color_R[i] = r;
            color_G[i] = g;
            color_B[i] = b;
        }

        break;

    case 6:
        for (int i = 73; i < 76; i++)
        {
            color_R[i] = r;
            color_G[i] = g;
            color_B[i] = b;
        }

        break;

    case 7:
        for (int i = 76; i < 78; i++)
        {
            color_R[i] = r;
            color_G[i] = g;
            color_B[i] = b;
        }

        break;

    case 8:
        for (int i = 78; i < 81; i++)
        {
            color_R[i] = r;
            color_G[i] = g;
            color_B[i] = b;
        }

        break;

    case 9:
        for (int i = 81; i < 83; i++)
        {
            color_R[i] = r;
            color_G[i] = g;
            color_B[i] = b;
        }

        break;

    case 10:
        for (int i = 83; i < 86; i++)
        {
            color_R[i] = r;
            color_G[i] = g;
            color_B[i] = b;
        }

        break;

    case 11:
        for (int i = 86; i < 88; i++)
        {
            color_R[i] = r;
            color_G[i] = g;
            color_B[i] = b;
        }

        break;

    case 12:
        for (int i = 88; i < 91; i++)
        {
            color_R[i] = r;
            color_G[i] = g;
            color_B[i] = b;
        }

        break;

    case 13:
        for (int i = 91; i < 93; i++)
        {
            color_R[i] = r;
            color_G[i] = g;
            color_B[i] = b;
        }

        break;

    case 14:
        for (int i = 93; i < 96; i++)
        {
            color_R[i] = r;
            color_G[i] = g;
            color_B[i] = b;
        }

        break;

    case 15:
        for (int i = 96; i < 98; i++)
        {
            color_R[i] = r;
            color_G[i] = g;
            color_B[i] = b;
        }

        break;

    case 16:
        for (int i = 98; i < 101; i++)
        {
            color_R[i] = r;
            color_G[i] = g;
            color_B[i] = b;
        }

        break;

    case 17:
        for (int i = 101; i < 103; i++)
        {
            color_R[i] = r;
            color_G[i] = g;
            color_B[i] = b;
        }

        break;
    }
}


/////////////////////////////////////////////////////////////////////////////////////////////
//                                     Setup and reset                                     //
/////////////////////////////////////////////////////////////////////////////////////////////

/// @brief Setup function for the module
void MySetup()
{
    pixels.begin();
    pixels.setBrightness(brightness);

    _Fadded.attach(rotation_speed, faddingAll); // start the fading ticker
}

///////////////////////////////  Reset all proprety of module  ////////////////////////////////

/// @brief Call after the config and when the module reset by the app
void ResetModule()
{
    // settings
    float fadding = (85 / 100.0);
    float rotation_speed = 0.05;
    int brightness = 250;

    for (int i = 0; i < NUMPIXELS; i++)
    {
        green(i);
    }
}

/////////////////////////////////  Write here the loop code  /////////////////////////////////
//
// To send datas to the server, use the send function "Send("COD;xxx;yyy");"
void MyLoop()
{
}

/////////////////////////////////////////////////////////////////////////////////////////////
//                                      Communication                                      //
/////////////////////////////////////////////////////////////////////////////////////////////

// Call when a message is received from server
void Received()
{

    // Get info of received string
    comm.Info_Received();

    // stop the radar
    if (comm.GetCode() == "RDS") 
    {
        switch (comm.GetParameter(1)[0])
        {
        case '1': // turn off the leds
            light_on_off = 0;
            break;
        case '0': // turn on the leds
            light_on_off = 1;
            break;
        }
    }

    // modifing the speed or reminicence
    if (comm.GetCode() == "RDP") 
    {
        if (rotation_speed != comm.GetParameter(1).toFloat())
        {
            _Fadded.detach();
            rotation_speed = comm.GetParameter(1).toFloat();
            _Fadded.attach(rotation_speed, faddingAll);
        }

        fadding = comm.GetParameter(2).toFloat();
    }

    if (comm.GetCode() == "RDR") // send the map information
    {
        // change colors of the small circle
        for (int i = 0; i < 8; i++)
        {
            switch (comm.GetParameter(1)[i])
            {
            case 'W': // water
                // allumer les leds de la case en vert
                {
                    float c_r = 0;
                    float c_g = 1;
                    float c_b = 0;
                    light_smallcircle_leds(i, c_r, c_g, c_b);
                }

                break;

            case 'E': // earth
                      // allumer les leds de la case en orange
            {
                float c_r = 0;
                float c_g = 1;
                float c_b = 0;
                light_smallcircle_leds(i, c_r, c_g, c_b);
            }

            break;

            case 'T': // torpeedo
                // allumer en rouge foncé

                {
                    float c_r = 1.5;
                    float c_g = 0;
                    float c_b = 0;
                    light_smallcircle_leds(i, c_r, c_g, c_b);
                }

                break;

            case 'C': // chest
                // allumer en bleu

                {
                    float c_r = 0;
                    float c_g = 0;
                    float c_b = 1;
                    light_smallcircle_leds(i, c_r, c_g, c_b);
                }

                break;

            case 'B': // board
                // allumer en blanc

                {
                    float c_r = 1;
                    float c_g = 1;
                    float c_b = 1;
                    light_smallcircle_leds(i, c_r, c_g, c_b);
                }

                break;
            }
        }

        // change colors of the medium circle
        for (int i = 0; i < 12; i++)
        {
            switch (comm.GetParameter(2)[i])
            {

            case 'W': // water
                // allumer les leds de la case en vert
                {
                    float c_r = 0;
                    float c_g = 1;
                    float c_b = 0;
                    light_mediumcircle_leds(i, c_r, c_g, c_b);
                }

                break;

            case 'E': // earth
                      // allumer les leds de la case en orange
            {
                float c_r = 0;
                float c_g = 1;
                float c_b = 0;
                light_mediumcircle_leds(i, c_r, c_g, c_b);
            }

            break;

            case 'T': // torpeedo
                // allumer en rouge foncé

                {
                    float c_r = 1.5;
                    float c_g = 0;
                    float c_b = 0;
                    light_mediumcircle_leds(i, c_r, c_g, c_b);
                }

                break;

            case 'C': // chest
                // allumer en bleu

                {
                    float c_r = 0;
                    float c_g = 0;
                    float c_b = 1;
                    light_mediumcircle_leds(i, c_r, c_g, c_b);
                }

                break;

            case 'B': // board
                // allumer en blanc

                {
                    float c_r = 1;
                    float c_g = 1;
                    float c_b = 1;
                    light_mediumcircle_leds(i, c_r, c_g, c_b);
                }

                break;
            }
        }

        // change colors of the big circle
        for (int i = 0; i < 18; i++)
        {
            switch (comm.GetParameter(3)[i])
            {

            case 'W': // water
                // allumer les leds de la case en vert
                {
                    float c_r = 0;
                    float c_g = 1;
                    float c_b = 0;
                    light_bigcircle_leds(i, c_r, c_g, c_b);
                }

                break;

            case 'E': // earth
                      // allumer les leds de la case en orange
            {
                float c_r = 0;
                float c_g = 1;
                float c_b = 0;
                light_bigcircle_leds(i, c_r, c_g, c_b);
            }

            break;

            case 'T': // torpeedo
                // allumer en rouge foncé

                {
                    float c_r = 1.5;
                    float c_g = 0;
                    float c_b = 0;
                    light_bigcircle_leds(i, c_r, c_g, c_b);
                }

                break;

            case 'C': // chest
                // allumer en bleu

                {
                    float c_r = 0;
                    float c_g = 0;
                    float c_b = 1;
                    light_bigcircle_leds(i, c_r, c_g, c_b);
                }

                break;

            case 'B': // board
                // allumer en blanc

                {
                    float c_r = 1;
                    float c_g = 1;
                    float c_b = 1;
                    light_bigcircle_leds(i, c_r, c_g, c_b);
                }

                break;
            }
        }
    }
}

// Call when a message is received to "close the loop" of the server.
// Every send fonction will call this in standalone
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