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

////////  Define specific class

bool light_on_off = true; // for turning off the leds

/// @brief Pixel management
class Pixel
{
public:
    /// @brief Red componant of pixel, between 0..1
    float Red;

    /// @brief Green componant of pixel, between 0..1
    float Green;

    /// @brief Blue componant of pixel, between 0..1
    float Blue;

    /// @brief Current brigntness of the pixel, between 0..255
    float Brightness = 150;

    /// @brief Return current color of the pixel
    /// @return Curret red value (0..255)
    uint8_t GetRed()
    {
        if (light_on_off)
            return static_cast<uint8_t>(Red * Brightness);
        else
            return 0;
    }

    /// @brief Return current color of the pixel
    /// @return Curret green value (0..255)
    uint8_t GetGreen()
    {
        if (light_on_off)
            return static_cast<uint8_t>(Green * Brightness);
        else
            return 0;
    }

    /// @brief Return current color of the pixel
    /// @return Curret blue value (0..255)
    uint8_t GetBlue()
    {
        if (light_on_off)
            return static_cast<uint8_t>(Blue * Brightness);
        else
            return 0;
    }

    /// @brief Update color of the pixel
    /// @param newColor New pixel color
    void SetColor(Pixel newColor)
    {
        this->Red = newColor.Red;
        this->Green = newColor.Green;
        this->Blue = newColor.Blue;
    }

    Pixel()
    {
        this->Red = 0.0;
        this->Green = 0.0;
        this->Blue = 0.0;
    }

    Pixel(float r, float g, float b)
    {
        this->Red = r;
        this->Green = g;
        this->Blue = b;
    }
};

////////  Define global variables
int testInt = 0;
bool testBool = 0;

Pixel pWater(0.0, 1.0, 1.0);
Pixel pEarth(1.2, 0.5, 0.0);
Pixel pChest(1.0, 0.0, 0.0);
Pixel pTorpedo(1.5, 0.0, 0.0);
Pixel pBorder(1.0, 1.0, 1.0);

// settings
float fadding = (85 / 100.0);
float rotation_speed = 0.05;
int brightness = 250; // max 255

// numbers of circles leds
int Nb_Leds_Circle_Sml = 24;
int Nb_Leds_Circle_Med = 35;
float Nb_Leds_Circle_Big = 45; // need to be a float for the dividing !

// arrays for the color, modified by the UDP or Serial
Pixel color[104]{};

int RotaLed = (NUMPIXELS - Nb_Leds_Circle_Big);

Ticker _Fadded;

/////////////////////////////////////////////////////////////////////////////////////////////
//                                      User function                                      //
/////////////////////////////////////////////////////////////////////////////////////////////

/// @brief Light on all the leds
void light_leds()
{
    for (int i = 0; i < NUMPIXELS; i++)
    {
        pixels.setPixelColor(i, pixels.Color(color[i].GetRed(), color[i].GetGreen(), color[i].GetBlue()));
    }
    pixels.show();
}

void faddingAll()
{
    int RotaLed_2 = (((RotaLed - (NUMPIXELS - Nb_Leds_Circle_Big)) / (Nb_Leds_Circle_Big / Nb_Leds_Circle_Med)) + Nb_Leds_Circle_Sml);
    int RotaLed_3 = (((RotaLed - (NUMPIXELS - Nb_Leds_Circle_Big)) / (Nb_Leds_Circle_Big / Nb_Leds_Circle_Sml)));

    // Manage fading
    for (int i = 0; i < NUMPIXELS; i++) // fadding R
    {
        // No computation if brightness is null
        if(color[i].Brightness == 0)
            continue;

        // Decrease current brightness
        color[i].Brightness = (color[i].Brightness * fadding);

        // Force to 0 at low value
        if (color[i].Brightness < 5)
        {
            color[i].Brightness = 0;
        }
    }
    
    // Set the rotating leds at full red brightness
    color[RotaLed].Brightness = 150;
    color[RotaLed_2].Brightness = 150;
    color[RotaLed_3].Brightness = 150;

    // light on all leds
    light_leds(); 

    // Increase rotationg leds position
    RotaLed++;

    // 
    if (RotaLed > NUMPIXELS)
    {
        RotaLed = (NUMPIXELS - Nb_Leds_Circle_Big); // set the limits of the lasrt rings
    }
}

/// @brief Change the colors of the specifics led of small circle
/// @param i Element of circle to update
/// @param pixel Pixel value of element
void light_smallcircle_leds(int i, Pixel pixel)
{
    switch (i)
    {
    case 0:
        for (int i = 0; i < 3; i++)
        {
            color[i].SetColor(pixel);
        }
        color[23].SetColor(pixel);
        break;

    case 1:
        for (int i = 3; i < 7; i++)
        {
            color[i].SetColor(pixel);
        }
        break;

    case 2:
        for (int i = 7; i < 11; i++)
        {
            color[i].SetColor(pixel);
        }
        break;

    case 3:
        for (int i = 11; i < 15; i++)
        {
            color[i].SetColor(pixel);
        }
        break;

    case 4:
        for (int i = 15; i < 19; i++)
        {
            color[i].SetColor(pixel);
        }
        break;

    case 5:
        for (int i = 19; i < 23; i++)
        {
            color[i].SetColor(pixel);
        }
        break;
    }
}

/// @brief Change the colors of the specifics led medium circle
/// @param i Element of circle to update
/// @param pixel Pixel value of element
void light_mediumcircle_leds(int i, Pixel pixel)
{
    switch (i)
    {
    case 0:
        for (int i = 24; i < 26; i++)
        {
            color[i].SetColor(pixel);
        }
        color[58] = pixel;
        break;

    case 1:
        for (int i = 26; i < 29; i++)
        {
            color[i].SetColor(pixel);
        }
        break;

    case 2:
        for (int i = 29; i < 32; i++)
        {
            color[i].SetColor(pixel);
        }
        break;

    case 3:
        for (int i = 32; i < 35; i++)
        {
            color[i].SetColor(pixel);
        }
        break;

    case 4:
        for (int i = 35; i < 38; i++)
        {
            color[i].SetColor(pixel);
        }
        break;

    case 5:
        for (int i = 38; i < 41; i++)
        {
            color[i].SetColor(pixel);
        }
        break;

    case 6:
        for (int i = 41; i < 44; i++)
        {
            color[i].SetColor(pixel);
        }
        break;

    case 7:
        for (int i = 44; i < 47; i++)
        {
            color[i].SetColor(pixel);
        }
        break;

    case 8:
        for (int i = 47; i < 50; i++)
        {
            color[i] = pixel;
        }
        break;

    case 9:
        for (int i = 50; i < 53; i++)
        {
            color[i].SetColor(pixel);
        }
        break;

    case 10:
        for (int i = 53; i < 56; i++)
        {
            color[i].SetColor(pixel);
        }
        break;

    case 11:
        for (int i = 56; i < 58; i++)
        {
            color[i].SetColor(pixel);
        }
        break;
    }
}

/// @brief Change the colors of the specifics led large circle
/// @param i Element of circle to update
/// @param pixel Pixel value of element
void light_bigcircle_leds(int i, Pixel pixel)
{
    switch (i)
    {
    case 0:
        for (int i = 59; i < 61; i++)
        {
            color[i].SetColor(pixel);
        }
        color[103].SetColor(pixel);
        break;

    case 1:
        for (int i = 61; i < 63; i++)
        {
            color[i].SetColor(pixel);
        }
        break;

    case 2:
        for (int i = 63; i < 66; i++)
        {
            color[i].SetColor(pixel);
        }
        break;

    case 3:
        for (int i = 66; i < 68; i++)
        {
            color[i].SetColor(pixel);
        }
        break;

    case 4:
        for (int i = 68; i < 71; i++)
        {
            color[i].SetColor(pixel);
        }
        break;

    case 5:
        for (int i = 71; i < 73; i++)
        {
            color[i].SetColor(pixel);
        }
        break;

    case 6:
        for (int i = 73; i < 76; i++)
        {
            color[i].SetColor(pixel);
        }
        break;

    case 7:
        for (int i = 76; i < 78; i++)
        {
            color[i].SetColor(pixel);
        }
        break;

    case 8:
        for (int i = 78; i < 81; i++)
        {
            color[i].SetColor(pixel);
        }
        break;

    case 9:
        for (int i = 81; i < 83; i++)
        {
            color[i].SetColor(pixel);
        }
        break;

    case 10:
        for (int i = 83; i < 86; i++)
        {
            color[i].SetColor(pixel);
        }
        break;

    case 11:
        for (int i = 86; i < 88; i++)
        {
            color[i].SetColor(pixel);
        }
        break;

    case 12:
        for (int i = 88; i < 91; i++)
        {
            color[i].SetColor(pixel);
        }
        break;

    case 13:
        for (int i = 91; i < 93; i++)
        {
            color[i].SetColor(pixel);
        }
        break;

    case 14:
        for (int i = 93; i < 96; i++)
        {
            color[i].SetColor(pixel);
        }
        break;

    case 15:
        for (int i = 96; i < 98; i++)
        {
            color[i].SetColor(pixel);
        }
        break;

    case 16:
        for (int i = 98; i < 101; i++)
        {
            color[i].SetColor(pixel);
        }
        break;

    case 17:
        for (int i = 101; i < 103; i++)
        {
            color[i].SetColor(pixel);
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
        color[i].SetColor(pWater);
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
                {
                    light_smallcircle_leds(i, pWater);
                }
                break;

                case 'E': // earth
                {
                    light_smallcircle_leds(i, pEarth);
                }
                break;

                case 'T': // torpeedo
                {
                    light_smallcircle_leds(i, pTorpedo);
                }
                break;

                case 'C': // chest
                {
                    light_smallcircle_leds(i, pChest);
                }
                break;

                case 'B': // board
                {
                    light_smallcircle_leds(i, pBorder);
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
                {
                    light_smallcircle_leds(i, pWater);
                }
                break;

                case 'E': // earth
                {
                    light_smallcircle_leds(i, pEarth);
                }
                break;

                case 'T': // torpeedo
                {
                    light_smallcircle_leds(i, pTorpedo);
                }
                break;

                case 'C': // chest
                {
                    light_smallcircle_leds(i, pChest);
                }
                break;

                case 'B': // board
                {
                    light_smallcircle_leds(i, pBorder);
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
                {
                    light_smallcircle_leds(i, pWater);
                }
                break;

                case 'E': // earth
                {
                    light_smallcircle_leds(i, pEarth);
                }
                break;

                case 'T': // torpeedo
                {
                    light_smallcircle_leds(i, pTorpedo);
                }
                break;

                case 'C': // chest
                {
                    light_smallcircle_leds(i, pChest);
                }
                break;

                case 'B': // board
                {
                    light_smallcircle_leds(i, pBorder);
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