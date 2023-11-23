#include <common.h>
#ifdef DIALOGUE

#include <Arduino.h>

//////// Add new include library
//#include <LOLIN_EPD.h>
#include <Adafruit_GFX.h>
#include <Wire.h>
#include <LiquidTWI2.h>             // Library for I2C LCD screen
//#include <Adafruit_NeoPixel.h>      // Library for NeoPixel
#include<FastLED.h> // header file

////////  Define global constantes (ALWAYS IN MAJ, use pin number and not name)
#define C_BLACK 0x0
#define C_RED 0x2       // Inverted with green !    0x1 -> 0x2
#define C_YELLOW 0x3
#define C_GREEN 0x1     // Inverted with red !      0x2 -> 0x1
#define C_TEAL 0x5      // Inverted with red-green  0x6 -> 0x5
#define C_BLUE 0x4
#define C_VIOLET 0x6    // Inverted with red-green  0x5 -> 0x6
#define C_WHITE 0x7

/*          TEST FOR E ink screen

//  D1 mini
#define EPD_CS D0
#define EPD_DC D3
#define EPD_RST -1  // can set to -1 and share with microcontroller Reset!
#define EPD_BUSY -1 // can set to -1 to not use a pin (will wait a fixed delay)

//  D32 Pro
// #define EPD_CS 14
// #define EPD_DC 27
// #define EPD_RST 33  // can set to -1 and share with microcontroller Reset!
// #define EPD_BUSY -1 // can set to -1 to not use a pin (will wait a fixed delay)

LOLIN_SSD1680 EPD(250, 122, EPD_DC, EPD_RST, EPD_CS, EPD_BUSY); //hardware SPI for E ink screen
*/


#define NB_INPUT 5

/*// Neopixels setup
#define PIN 0
#define NB_INPUT 5
Adafruit_NeoPixel pixels(NB_INPUT, PIN, NEO_GRB + NEO_KHZ800);
*/

// Connect via i2c, address 0x20 (A0-A2 not jumpered)
LiquidTWI2 LCD_1(0x00);
LiquidTWI2 LCD_2(0x01);
LiquidTWI2 LCD_3(0x02);
LiquidTWI2 LCD_4(0x03);


////////  Define global variables
int _testInt = 0;
bool _testBool = 0;


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
    // Suround every "Serial" order between "#ifdef LOG" and "#endif"
    #ifdef LOG
    Serial.println("--- Model ---");
    #endif

    /*
    EPD.begin();

    EPD.setRotation(0);
    EPD.clearBuffer();
    EPD.setTextSize(1);
    EPD.setTextColor(EPD_BLACK);
    EPD.setCursor(0,0);
    EPD.println("hello world! Welcome to wemos.cc, this is test for long text.");

    EPD.setTextColor(EPD_RED);
    EPD.setTextSize(2);
    EPD.println("I'm red!");

    EPD.display();

    EPD.deepSleep();
    */

    // set the LCD type
    LCD_1.setMCPType(LTI_TYPE_MCP23017); 
    LCD_2.setMCPType(LTI_TYPE_MCP23017); 
    LCD_3.setMCPType(LTI_TYPE_MCP23017); 
    LCD_4.setMCPType(LTI_TYPE_MCP23017); 

    // set up the LCD's number of rows and columns:
    LCD_1.begin(16, 2);
    LCD_2.begin(16, 2);
    LCD_3.begin(16, 2);
    LCD_4.begin(16, 2);


    // Define pixels property
    pixels.useInfoPixel();
    pixels.addLeds(5);
    pixels.initalize();

    /*/ Neo pixels
    pixels.begin();
    pixels.setBrightness(255);
    pixels.setPixelColor(0, 111, 111, 111);
    pixels.setPixelColor(1, pixels.Color(255, 0, 0)); // turn led red
    pixels.setPixelColor(2, pixels.Color(0, 255, 0)); // turn led red
    pixels.setPixelColor(3, pixels.Color(0, 0, 255)); // turn led red
    pixels.show();
    */

}


///////////////////////////////  Reset all proprety of module  ////////////////////////////////

/// @brief Call after the config and when the module reset by the app
void ResetModule()
{

    #ifdef STANDALONE
    // Set a debug text
    LCD_1.print("Screen 1");
    LCD_2.print("Screen 2");
    LCD_3.print("Screen 3");
    LCD_4.print("Screen 4");

    // Set debug background
    LCD_1.setBacklight(C_RED);
    LCD_2.setBacklight(C_GREEN);
    LCD_3.setBacklight(C_BLUE);
    LCD_4.setBacklight(C_VIOLET);

    // Test pixel colors
    pixels.setPixelColor(0, CRGB::Purple);
    pixels.setPixelColor(1, CRGB::Red);
    //pixels.setPixelColor(2, CRGB::Green);
    pixels.setPixelColor(3, CRGB::Blue);
    //pixels.setPixelColor(4, CRGB::Purple);


    // Utilisation de la classe MyPixels avec ajout de variateurs
    pixels.addVariator(1, CRGB::Blue, CRGB::BlueViolet);
    pixels.addVariator(2, CRGB::Red, CRGB::Black);
    pixels.addVariator(3, CRGB::Green, CRGB::Black);
    pixels.addVariator(4, CRGB::Blue, CRGB::Black);

    #endif

}


/////////////////////////////////  Write here the loop code  /////////////////////////////////

uint8_t btn1_backup;

/// @brief Call at the end of the main loop function
void MyLoop()
{
    
    uint8_t buttons = LCD_1.readButtons();
    //Serial.println(buttons);

    if (buttons != btn1_backup)
    {
        if (buttons & BUTTON_SELECT) 
        {
        LCD_1.setBacklight(VIOLET);
        }
        else
        {
          LCD_1.setBacklight(GREEN);
        }

        btn1_backup = buttons;
    }
 
    
    pixels.update();
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
                pixels.setPixelColor(i, CRGB::Black ); // turn led OFF
                break;

            case 'R':
                pixels.setPixelColor(i, CRGB(255, 0, 0)); // turn led red
                break;

            case 'G':
                pixels.setPixelColor(i, CRGB(0, 255, 0)); // turn led green
                break;

            case 'B':
                pixels.setPixelColor(i, CRGB(0, 0, 255)); // turn led blue
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
            pixels.setPixelColor(i, CRGB(r, g, b)); // turn led blue
        }

        // Update pixels
        //pixels.show();
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