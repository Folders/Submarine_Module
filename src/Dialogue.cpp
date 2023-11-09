#include <common.h>
#ifdef DIALOGUE

#include <Arduino.h>

//////// Add new include library
#include <LOLIN_EPD.h>
#include <Adafruit_GFX.h>
#include <Wire.h>
#include <LiquidTWI2.h>

////////  Define global constantes (ALWAYS IN MAJ, use pin number and not name)
#define C_BLACK 0x0
#define C_RED 0x1
#define C_YELLOW 0x3
#define C_GREEN 0x2
#define C_TEAL 0x6
#define C_BLUE 0x4
#define C_VIOLET 0x5
#define C_WHITE 0x7


/*D1 mini*/
#define EPD_CS D0
#define EPD_DC D3
#define EPD_RST -1  // can set to -1 and share with microcontroller Reset!
#define EPD_BUSY -1 // can set to -1 to not use a pin (will wait a fixed delay)

/*D32 Pro*/
// #define EPD_CS 14
// #define EPD_DC 27
// #define EPD_RST 33  // can set to -1 and share with microcontroller Reset!
// #define EPD_BUSY -1 // can set to -1 to not use a pin (will wait a fixed delay)

LOLIN_SSD1680 EPD(250, 122, EPD_DC, EPD_RST, EPD_CS, EPD_BUSY); //hardware SPI

// #define EPD_MOSI D7
// #define EPD_CLK D5
// LOLIN_SSD1680 EPD(250,122, EPD_MOSI, EPD_CLK, EPD_DC, EPD_RST, EPD_CS, EPD_BUSY); //IO

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
    // set up the LCD's number of rows and columns:
    LCD_1.begin(16, 2);
    // Print a message to the LCD.
    LCD_1.print("Screen 1");
    
    // set the LCD type
    LCD_2.setMCPType(LTI_TYPE_MCP23017); 
    // set up the LCD's number of rows and columns:
    LCD_2.begin(16, 2);
    // Print a message to the LCD.
    LCD_2.print("Screen 2");
    
    // set the LCD type
    LCD_3.setMCPType(LTI_TYPE_MCP23017); 
    // set up the LCD's number of rows and columns:
    LCD_3.begin(16, 2);
    // Print a message to the LCD.
    LCD_3.print("Screen 3");
    
    // set the LCD type
    LCD_4.setMCPType(LTI_TYPE_MCP23017); 
    // set up the LCD's number of rows and columns:
    LCD_4.begin(16, 2);
    // Print a message to the LCD.
    LCD_4.print("Screen 4");

    LCD_1.setBacklight(C_RED);
    LCD_2.setBacklight(C_GREEN);
    LCD_3.setBacklight(C_BLUE);
    LCD_4.setBacklight(C_YELLOW);
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
    // To send datas to the server, use the send function
    comm.send("LED;R;1");

    // It's possible to send with more then on line
    comm.start();      // Open the buffer
    comm.add("LED");   // Write String
    comm.add(';');     // Add char
    // comm.add(testInt); // Add from variable
    comm.send();       // Send concatened variable
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