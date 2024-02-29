#include <common.h>
#ifdef EXTINGUISHER

#include <Arduino.h>

//////// Add new include library
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h> //charge the Adafruit_SSD1306 Wemos Mini OLED !
#include <Bounce2.h>
#include <Ticker.h>
#include "SoundData.h"
#include "XT_DAC_Audio.h"

////////  Define global constantes      (ALWAYS IN MAJ)

// OLED
#define SCREEN_WIDTH 128    // OLED display width, in pixels
#define SCREEN_HEIGHT 64    // OLED display height, in pixels
#define OLED_RESET -1       // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// IR
const int LED_IR = 4;

// Buttons
#define TRIGGER 18
#define CONTACT 19

// Song
// #define PLAY_PIN 15

const float ANIM_FREQUENCE = 0.6; // animation frequencie

////////  Define global variables

Bounce trigger = Bounce();
Bounce contact = Bounce();

// display
int percent;

// animations
byte batt_lvl_charging = 9;
byte batt_lvl_refill;

bool arrowcolor;
bool lightingcolor;
bool a_arrowcolor;
bool a_lightingcolor;
bool trigger_button;
bool contact_button;

// Sounds settings
XT_Wav_Class ForceWithYou(Force);   // create an object of type XT_Wav_Class that is used by
                                    // the dac audio class (below), passing wav data as parameter.
XT_DAC_Audio_Class DacAudio(25, 0); // Create the main player class object.
                                    // Use GPIO 25, one of the 2 DAC pins and timer 0
bool play_song;


/// @brief make cligning the arrow
void clign_symbol_arrow()
{
    // change color with ticker
    arrowcolor = !arrowcolor;
}

/// @brief charging battery animation
void refill_battery_animation() // for charging animation
{
    batt_lvl_charging++; // add bars to battery
    if (batt_lvl_charging > 11)
    {
        batt_lvl_charging = 0; // restart charging animation
    }
}

/// @brief make clingning the lighting
void clign_symbol_lighting()
{
    // change color with ticker
    lightingcolor = !lightingcolor;
}

/// @brief using battery animation
void using_battery_animation() // for using battery animation
{
    batt_lvl_refill++;
    if (batt_lvl_refill > 11)
    {
        batt_lvl_refill = 0;
    }
}

// Tickers
Ticker _Clign(clign_symbol_arrow, 1000, 0, MILLIS);
Ticker _Clign_1(clign_symbol_lighting, 1000, 0, MILLIS);
Ticker _Animation(refill_battery_animation, (1000 * ANIM_FREQUENCE), 0, MILLIS);
Ticker _Animation_1(using_battery_animation, (1000 * ANIM_FREQUENCE), 0, MILLIS);

/////////////////////////////////////////////////////////////////////////////////////////////
//                                      User function                                      //
/////////////////////////////////////////////////////////////////////////////////////////////

/// @brief //display percent on OLED
void display_percent()
{
    display.clearDisplay();
    display.setTextColor(WHITE);

    if (percent < 10 && percent > 0)
    { // for centering the text when 1-10
        display.setTextSize(5);
        display.setCursor(40, 15);
        display.print(percent);
        display.print("%");
    }
    else if (percent == 0)
    { // bigger size if percent = 0
        display.setTextSize(2);
        display.setCursor(55, 15);
        display.print("0%");
        display.println();
        display.setCursor(38, 35);
        display.println("vide!");
    }
    else if (percent == 100)
    { // smaller size if percent = 100
        display.setTextSize(4);
        display.setCursor(18, 18);
        display.print("100%");
    }
    else if (percent >= 10 && percent < 100)
    {
        display.setTextSize(4);
        display.setCursor(32, 17);
        display.print(percent);
        display.print("%");
    }

    display.display();
}

/// @brief draw a battery symbol
void display_battery_unfill()
{
    display.clearDisplay();

    // Draw the outer battery rectangle
    display.drawRect(20, 10, 90, 44, WHITE);

    // Draw the "head" of the battery on the right side
    display.drawRect(110, 24, 4, 16, WHITE);

    // Draw the battery level indicators inside
    for (int i = 0; i < 11; i++)
    {
        display.fillRect((22 + i * 8), 12, 6, 40, WHITE);
    }

    // unfill effect
    for (int i = 0; i < 11; i++)
    {
        if (i < batt_lvl_charging)
        {
            display.fillRect((22 + (10 - i) * 8), 12, 6, 40, BLACK);
        }
    }

    // Arrow symbol
    a_arrowcolor = arrowcolor ? BLACK : WHITE; // change arrow's logo color
    display.fillRect(8, 29, 8, 5, a_arrowcolor);
    display.fillTriangle(2, 31, 8, 27, 8, 35, a_arrowcolor); //
    display.display();
}

void display_battery_refill()
{
    display.clearDisplay();
    // Draw the outer battery rectangle
    display.drawRect(20, 10, 90, 44, WHITE);

    // Draw the "head" of the battery on the right side
    display.drawRect(110, 24, 4, 16, WHITE);

    // Refill effect
    for (int i = 0; i <= batt_lvl_refill && i <= 11; i++)
    {
        if (i <= 10)
        {
            display.fillRect((22 + i * 8), 12, 6, 40, WHITE);
        }
        if (i == 11)
        {
            display.fillRect(21, 11, 88, 42, BLACK); // fill the battery in black
        }
    }

    // lighting symbol
    a_lightingcolor = lightingcolor ? BLACK : WHITE; // change lighing's symbol color
    display.fillTriangle(12, 26, 8, 33, 12, 31, a_lightingcolor);
    display.fillTriangle(10, 37, 10, 32, 14, 29, a_lightingcolor);

    display.display();
}


/// @brief read buttons
void read_buttons()
{
    trigger.update(); // read trigger
    contact.update(); // read contact

    if (trigger.fell()) // trigger's button has been pressed
    {
        comm.send("EXR;1"); // send trigger's on to server
#ifdef LOG
        Serial.println("Trigger button pressed");
#endif

        if (percent > 0)
        {
            digitalWrite(LED_IR, HIGH); // turn on IR led

            trigger_button = true;
            batt_lvl_charging = 9;

            // play the song
            play_song = true ;
        }
        else // if percent = 0 do nothing
        {
            trigger_button = false;
            play_song = false;
        }
    }

    else if (trigger.rose()) // trigger's button has been released
    {
        comm.send("EXR;0"); // send trigger's off to server
#ifdef LOG
        Serial.println("Trigger button released");
#endif

        // turn off the song
        DacAudio.StopAllSounds();

        // turn off the IR led
        digitalWrite(LED_IR, LOW);
        play_song = false;

        trigger_button = false;
    }

    if (contact.fell()) // contact's button has been pressed
    {
        comm.send("EXC;1"); // send contact's on to server
#ifdef LOG
        Serial.println("Contact button pressed");
#endif

        if (percent < 100)
        {
            contact_button = true;
        }

        else // if percent = 100, do nothing and stop animation
        {
            contact_button = false;
        }
    }

    else if (contact.rose()) // contact's button has been released
    {
        comm.send("EXC;0"); // send contact's off to server
#ifdef LOG
        Serial.println("Contact button released");
#endif

        contact_button = false;
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////
//                                     Setup and reset                                     //
/////////////////////////////////////////////////////////////////////////////////////////////

/// @brief Setup function for the module
void MySetup()
{
    // set input's IO
    pinMode(TRIGGER, INPUT_PULLUP);
    pinMode(CONTACT, INPUT_PULLUP);
    trigger.attach(TRIGGER);
    trigger.interval(5);
    contact.attach(CONTACT);
    contact.interval(5);

    // LEDS outputs
    pinMode(LED_IR, OUTPUT);
    digitalWrite(LED_IR, LOW);

    percent = 100;

    // display setup
    if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS))
    {
        Serial.println(F("SSD1306 allocation failed"));
        for (;;)
            ; // Don't proceed, loop forever
    }

    // Clear the buffer
    display.clearDisplay();
    display.display();

#ifdef LOG
    Serial.println("Start");
#endif

    // Start tickers
    _Clign.start();
    _Clign_1.start();
    _Animation.start();
    _Animation_1.start();
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
    // Update tickers
    _Clign.update();
    _Clign_1.update();
    _Animation.update();
    _Animation_1.update();

    read_buttons(); // read buttons

    if (trigger_button == false && contact_button == false) // no button pressed
    {
        display_percent();
    }

    if (trigger_button == true && percent > 0) // trigger button pressed
    {
        display_battery_unfill(); // unfilling animation
    }

    if (contact_button == true && percent < 100) // contact button pressed
    {
        display_battery_refill(); // refilling animation
    }

    if (play_song == true)
    {
            DacAudio.FillBuffer();             // Fill the sound buffer with data
    if (ForceWithYou.Playing == false) // if not playing,
        DacAudio.Play(&ForceWithYou);  // play it, this will cause it to repeat and repeat...
        #ifdef LOG
        Serial.println("Should be playing");
#endif
    }

        if (play_song == false)
    {
        DacAudio.StopAllSounds();
        #ifdef LOG
        Serial.println("Stop playing");
#endif
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

    if (comm.GetCode() == "EXP")
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

    // decrease percent if trigger's on
    if (comm.GetCode() == "EXR" && comm.GetParameter(1) == "1")
    {

        // DO something
        if (percent > 5)
        {
            percent = percent - 5;
        }

        else
        {
            percent = 0;
        }
    }

    // decrease percent if trigger's on
    if (comm.GetCode() == "EXC" && comm.GetParameter(1) == "1")
    {
        // DO something
        if (percent < 100)
        {
            percent = percent + 5;
        }
        else
        {
            percent = 100;
        }
    }
}

#endif