#include <common.h>
#ifdef DIALOGUE

#include <Arduino.h>

//////// Add new include library
// #include <LOLIN_EPD.h>
#include <Adafruit_GFX.h>
#include <Wire.h>
#include <LiquidTWI2.h> // Library for I2C LCD screen
// #include <Adafruit_NeoPixel.h>      // Library for NeoPixel
#include <FastLED.h> // header file

////////  Define global constantes (ALWAYS IN MAJ, use pin number and not name)
#define C_BLACK 0x0
#define C_RED 0x2 // Inverted with green !    0x1 -> 0x2
#define C_YELLOW 0x3
#define C_GREEN 0x1 // Inverted with red !      0x2 -> 0x1
#define C_TEAL 0x5  // Inverted with red-green  0x6 -> 0x5
#define C_BLUE 0x4
#define C_VIOLET 0x6 // Inverted with red-green  0x5 -> 0x6
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


Button btn_LCD1(&LCD_1, BUTTON_SELECT);
Button btn_LCD2(&LCD_1, BUTTON_SELECT);
Button btn_LCD3(&LCD_1, BUTTON_SELECT);
Button btn_LCD4(&LCD_1, BUTTON_SELECT);
Button btn_Next(D4);

////////  Define global variables
bool buttonUsed[5];

/////////////////////////////////////////////////////////////////////////////////////////////
//                                      User function                                      //
/////////////////////////////////////////////////////////////////////////////////////////////

/// @brief Write a message on a LCD
/// @param lcd Destination LCD
/// @param l1 First line of text
/// @param l2 Second line of text
/// @param color Color of background (on char in maj)
void writeText(LiquidTWI2 &lcd, const char *l1, const char *l2, char color)
{
    // Effacer l'écran
    lcd.clear();

    // Écrire sur la première ligne
    lcd.setCursor(0, 0);
    lcd.print(l1);

    // Écrire sur la deuxième ligne
    lcd.setCursor(0, 1);
    lcd.print(l2);

    // Chagement de la couleur
    switch (color)
    {
    case 'R': // Rouge
        lcd.setBacklight(C_RED);
        break;
    case 'Y': // Jaune
        lcd.setBacklight(C_YELLOW);
        break;
    case 'G': // Vert
        lcd.setBacklight(C_GREEN);
        break;
    case 'T': // Bleu claire
        lcd.setBacklight(C_TEAL);
        break;
    case 'B': // Blue
        lcd.setBacklight(C_BLUE);
        break;
    case 'V': // Violet
        lcd.setBacklight(C_VIOLET);
        break;
    case 'W': // Blanc
        lcd.setBacklight(C_WHITE);
        break;
    case '0': // No update
        lcd.setBacklight(C_BLACK);
        break;
    case 'N': // No update
        break;
    }
}


void buttonColor(int ind, char color)
{
    // Exit of parameter are not OK
    if (ind == 0 || ind > 4)
        return;

    // Set as used the current button
    buttonUsed[ind]= true;

    // Chagement de la couleur
    switch (color)
    {
    case 'R': // Rouge
        pixels.addVariator(ind, CRGB::Red, CRGB::Black);
        break;
    case 'Y': // Jaune
        pixels.addVariator(ind, CRGB::Yellow, CRGB::Black);
        break;
    case 'G': // Vert
        pixels.addVariator(ind, CRGB::Green, CRGB::Black);
        break;
    case 'T': // Bleu claire
        pixels.addVariator(ind, CRGB::Teal, CRGB::Black);
        break;
    case 'B': // Blue
        pixels.addVariator(ind, CRGB::Blue, CRGB::Black);
        break;
    case 'V': // Violet
        pixels.addVariator(ind, CRGB::Violet, CRGB::Black);
        break;
    case 'W': // Blanc
        pixels.addVariator(ind, CRGB::White, CRGB::Black);
        break;
    case '0': // Turn off
        pixels.setPixelColor(ind, CRGB::Black);

        // The button is not used
        buttonUsed[ind]= false;
        break;
    case 'N': // No update
        break;
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
    Serial.println("--- Dialogue ---");
#endif

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

    // Change parameter of button
    btn_Next.invert();


#ifdef STANDALONE

    // Set a debug text
    writeText(LCD_1, "Screen 1", "", 'R');
    writeText(LCD_2, "Screen 2", "", 'G');
    writeText(LCD_3, "Screen 3", "", 'B');
    writeText(LCD_4, "Screen 4", "", 'Y');

    // Test pixel colors
    pixels.setPixelColor(0, CRGB::Purple);

    // Test variator
    pixels.addVariator(1, CRGB::Red, CRGB::Black);
    pixels.addVariator(2, CRGB::Green, CRGB::Black);
    pixels.addVariator(3, CRGB::Blue, CRGB::Black);
    pixels.addVariator(4, CRGB::Yellow, CRGB::Black);
    pixels.addVariator(5, CRGB::Blue, CRGB::BlueViolet);

#else
    // Clear text
    LCD_1.clear();
    LCD_2.clear();
    LCD_3.clear();
    LCD_4.clear();

    // Set debug background
    LCD_1.setBacklight(C_BLACK);
    LCD_2.setBacklight(C_BLACK);
    LCD_3.setBacklight(C_BLACK);
    LCD_4.setBacklight(C_BLACK);

    // Test pixel colors
    pixels.setPixelColor(1, CRGB::Black);
    pixels.setPixelColor(2, CRGB::Black);
    pixels.setPixelColor(3, CRGB::Black);
    pixels.setPixelColor(4, CRGB::Black);
    pixels.setPixelColor(5, CRGB::Black);
#endif
}

/////////////////////////////////  Write here the loop code  /////////////////////////////////

uint8_t buttons, btn1_backup;

/// @brief Call at the end of the main loop function
void MyLoop()
{
    /*
    buttons = LCD_1.readButtons();
    Serial.print(buttons);
    buttons = LCD_2.readButtons();
    Serial.print(buttons);
    buttons = LCD_3.readButtons();
    Serial.print(buttons);
    buttons = LCD_4.readButtons();
    Serial.println(buttons);

    if (buttons != btn1_backup)
    {
        if (buttons & BUTTON_SELECT)
        {
            LCD_1.setBacklight(C_VIOLET);
        }
        else
        {
            LCD_1.setBacklight(C_GREEN);
        }

        btn1_backup = buttons;
    }
    */

   // Read input
    btn_LCD1.read();
    btn_LCD2.read();
    btn_LCD3.read();
    btn_LCD4.read();
    btn_Next.read();


    if (btn_LCD1.up())
        LCD_1.setBacklight(C_VIOLET);
        
    if (btn_LCD1.down())
        LCD_1.setBacklight(C_GREEN);


    if (btn_Next.up())
        LCD_4.setBacklight(C_VIOLET);
        
    if (btn_Next.down())
        LCD_4.setBacklight(C_GREEN);



    Serial.println(digitalRead(D4));


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
    if (comm.GetCode() == "TXT")
    {
        // Get the number of parameter
        int  i =  comm.GetSize();

        //  Write LCD 1
        if (i >= 3)
        {
            // Write in first LCD
            writeText(LCD_1, comm.GetParameterInChar(1), comm.GetParameterInChar(2), comm.GetParameterInChar(3)[0]);
            buttonColor(1, comm.GetParameterInChar(3)[0]);
        }
        else
        {
            // Clear LCD if not used
            LCD_1.clear();
            LCD_1.setBacklight(C_BLACK);
            buttonColor(1, '0');
        }

        //  Write LCD 2
        if (i >= 6)
        {
            // Write in first LCD
            writeText(LCD_2, comm.GetParameterInChar(4), comm.GetParameterInChar(5), comm.GetParameterInChar(6)[0]);
            buttonColor(2, comm.GetParameterInChar(6)[0]);
        }
        else
        {
            // Clear LCD if not used
            LCD_2.clear();
            LCD_2.setBacklight(C_BLACK);
            buttonColor(2, '0');
        }
        
        //  Write LCD 3
        if (i >= 9)
        {
            // Write in first LCD
            writeText(LCD_3, comm.GetParameterInChar(7), comm.GetParameterInChar(8), comm.GetParameterInChar(9)[0]);
            buttonColor(3, comm.GetParameterInChar(9)[0]);
        }
        else
        {
            // Clear LCD if not used
            LCD_3.clear();
            LCD_3.setBacklight(C_BLACK);
            buttonColor(3, '0');
        }

        //  Write LCD 4
        if (i >= 12)
        {
            // Write in first LCD
            writeText(LCD_4, comm.GetParameterInChar(10), comm.GetParameterInChar(11), comm.GetParameterInChar(12)[0]);
            buttonColor(4, comm.GetParameterInChar(12)[0]);
        }
        else
        {
            // Clear LCD if not used
            LCD_4.clear();
            LCD_4.setBacklight(C_BLACK);
            buttonColor(4, '0');
        }
        
    }

    // The get the received code, use the function GetCode()
    if (comm.GetCode() == "LED")
    {
        // Get index of light
        int i = comm.GetParameter(1).toInt();

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
        // pixels.show();
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