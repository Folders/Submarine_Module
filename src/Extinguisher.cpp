#include <common.h>
#ifdef EXTINGUISHER

#include <Arduino.h>

//////// Add new include library
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>       //charge the Adafruit_SSD1306 Wemos Mini OLED !
#include <Bounce2.h>
#include <Ticker.h>
#include "SoundData.h"
#include "XT_DAC_Audio.h"

////////  Define global constantes      (ALWAYS IN MAJ)

// OLED
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire,OLED_RESET);

// IR
const int LED_IR = 4;

// Buttons
#define TRIGGER 18
#define CONTACT 19

// Song
//#define PLAY_PIN 15

const float ANIM_FREQUENCE = 0.6;
const int INTERRUPT_DELAY = 60;

////////  Define global variables

Bounce trigger = Bounce();
Bounce contact = Bounce();

// display
int percent;

// animations
byte batt_lvl_charging = 9;
byte batt_lvl_refill;

bool filling_state;

bool arrowcolor;
bool lightingcolor;
bool a_arrowcolor;
bool a_lightingcolor;
bool trigger_button;
bool contact_button;


XT_Wav_Class ForceWithYou(Force);     // create an object of type XT_Wav_Class that is used by 
                                      // the dac audio class (below), passing wav data as parameter.
                                      
XT_DAC_Audio_Class DacAudio(25,0);    // Create the main player class object. 
                                      // Use GPIO 25, one of the 2 DAC pins and timer 0

uint32_t DemoCounter=0;               // Just a counter to use in the serial monitor
                                      // not essential to playing the sound


/// @brief make cligning the arrow
void clign_symbol()
{
    // change color with ticker
    arrowcolor = !arrowcolor;
}

/// @brief charging battery animation
void battery_animation() // for charging animation
{
    batt_lvl_charging++; // add bars to battery
    if (batt_lvl_charging > 9)
    {
        batt_lvl_charging = 0; // restart charging animation
    }
}

/// @brief make clingning the lighting
void clign_symbol_1()
{
    // change color with ticker
    lightingcolor = !lightingcolor;
}

/// @brief using battery animation
void battery_animation_1() // for using battery animation
{
    batt_lvl_refill++;
    if (batt_lvl_refill > 9)
    {
        batt_lvl_refill = 0;
    }
}

Ticker _Clign(clign_symbol, 1000, 0, MILLIS);
Ticker _Clign_1(clign_symbol_1, 1000, 0, MILLIS);
Ticker _Animation(battery_animation, (1000*ANIM_FREQUENCE), 0, MILLIS);
Ticker _Animation_1(battery_animation_1, (1000*ANIM_FREQUENCE), 0, MILLIS);

// Ticker _Clign;
//Ticker _Animation;



// Ticker _Clign_1;
//Ticker _Animation_1;


/////////////////////////////////////////////////////////////////////////////////////////////
//                                      User function                                      //
/////////////////////////////////////////////////////////////////////////////////////////////

/// @brief Si je tape ///, il me propose de mettre des commentaires à la fonction
void my_function()
{
    // ...
}

/// @brief //display percent on OLED
void display_percent()
{
    display.display();
    display.clearDisplay();
    display.setTextColor(WHITE);

    if (percent < 10 && percent > 0) // for centering the text when 1-10
    {
        display.setTextSize(5);
        display.setCursor(20, 4);
        display.print(percent);
        display.setCursor(50, 4);
        display.setTextSize(1);
        display.print("%");
    }

    else if (percent == 0)      //bigger size if percent = 0
    {
        display.setTextSize(3);
        display.setCursor(14, 0);
        display.print("0%");
        display.setTextSize(2);
        display.println();
        display.println();
        display.println("vide!");
    }

    else if (percent == 100)        //smaller size if percent = 100
    {
        display.setTextSize(3);
        display.setCursor(0, 15);
        display.print("100");
        display.setTextSize(1);
        display.print("%");
    }

    else if (percent >= 10 && percent < 100) // affich the percent with the "%" symbol
    {
        display.setTextSize(4, 5);
        display.setCursor(8, 5);
        display.print(percent);
        display.setCursor(55, 4);
        display.setTextSize(1);
        display.print("%");
    }

    display.display();
}

/// @brief draw a battery symbol
void display_battery()
{
    // draw BATTERY
    display.drawRect(16, 16, 31, 16, WHITE);
    display.drawRect(47, 22, 2, 4, WHITE);

    // draw battery bars
    for (int i = 0; i < 10; i++)
    {
        display.drawRect((45 - (i * 3)), 16, 2, 16, WHITE);
    }

    display.display();
}

/// @brief refilling animation
void refilling()
{
    // lighting symbol
    a_lightingcolor = lightingcolor ? BLACK : WHITE; // change lighing's symbol color
    display.fillTriangle(32, 36, 28, 43, 32, 41, a_lightingcolor);
    display.fillTriangle(30, 47, 30, 42, 34, 39, a_lightingcolor);

    // refilling effect
    for (int i = 0; i < 10; i++)
    {
        display.drawRect((45 - (i * 3)), 16, 2, 16, WHITE);
    }
    display.fillRect((17 + (batt_lvl_refill * 3)), 17, (29 - (batt_lvl_refill * 3)), 14, BLACK);

    display.display();
}

/// @brief unfilling animation
void unfilling()

{
    // ARROW symbol
    a_arrowcolor = arrowcolor ? BLACK : WHITE; // change arrow's logo color
    display.fillRect(28, 40, 10, 2, a_arrowcolor);
    display.fillTriangle(23, 41, 28, 38, 28, 44, a_arrowcolor);

    // unfilling effect
    display.fillRect((44 - (batt_lvl_charging * 3)), 17, (2 + (batt_lvl_charging * 3)), 14, BLACK);

    if (batt_lvl_charging > 8)
    {
        for (int i = 0; i < 10; i++)
        {
            display.drawRect((45 - (i * 3)), 16, 2, 16, WHITE);
        }
    }
    display.display();
}



/// @brief play the song with a delay when using
void play()
{
    DacAudio.FillBuffer();                // Fill the sound buffer with data
  if(ForceWithYou.Playing==false)         // if not playing,
    DacAudio.Play(&ForceWithYou);         // play it, this will cause it to repeat and repeat...
    /*
    // turn of first for restart the song with a small delay
    digitalWrite(PLAY_PIN, LOW);
    _tempo.once_ms(INTERRUPT_DELAY, []()
                   { digitalWrite(PLAY_PIN, HIGH); });
    */
}

/// @brief read buttons
void read_buttons()
{
    trigger.update(); // read trigger
    contact.update(); // read contact

    if (trigger.fell()) // trigger's button has been pressed
    {
        comm.send("EXR;1");     //send trigger's on to server

        if (percent > 0)
        {
            digitalWrite(LED_IR, HIGH); // turn on IR led

            trigger_button = true;
            batt_lvl_charging = 9;

            // start animation
            display.clearDisplay();
            display_battery();
           // _Animation.attach(ANIM_FREQUENCE, battery_animation);
           // _Clign.attach(1, clign_symbol);

            // play the song
            play();
            /*
            digitalWrite(PLAY_PIN, HIGH);
            _play.attach(8, play);
            */
        }
        else // if percent = 0 do nothing
        {
            trigger_button = false;
        }
    }

    else if (trigger.rose()) // trigger's button has been released
    {
        comm.send("EXR;0");     //send trigger's off to server

        // turn off the song

         DacAudio.StopAllSounds() ;

        /*
        digitalWrite(PLAY_PIN, LOW);
        _play.detach();
        */
       
        // turn off the IR led
        digitalWrite(LED_IR, LOW);

        trigger_button = false;

        display.clearDisplay();

        // stop animation
       // _Animation.detach();
       // _Clign.detach();
    }

    if (contact.fell()) // contact's button has been pressed
    {
        comm.send("EXC;1");     //send contact's on to server

        if (percent < 100)
        {
            contact_button = true;
            display.clearDisplay();

            // start charging animation
            display_battery();
           // _Animation_1.attach(ANIM_FREQUENCE, battery_animation_1);
           // _Clign_1.attach(1, clign_symbol_1);
        }

        else // if percent = 100, do nothing and stop animation
        {
            contact_button = false;
        }
    }

    else if (contact.rose()) // contact's button has been released
    {
        comm.send("EXC;0");     //send contact's off to server

        contact_button = false;

        // stop charging's animation
        display.clearDisplay();
       // _Animation_1.detach();
        // _Clign_1.detach();
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

    // SONG output
    /*
    pinMode(PLAY_PIN, OUTPUT);
    digitalWrite(PLAY_PIN, LOW);
    */

    percent = 100;

    // display setup
if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  display.display();
  // Clear the buffer
  display.clearDisplay();

  // Draw a single pixel in white
  display.drawPixel(10, 10, SSD1306_WHITE);
  display.drawPixel(20, 30, SSD1306_WHITE);

  // Show the display buffer on the screen. You MUST call display() after
  // drawing commands to make them visible on screen!
  display.display();

#ifdef LOG
    Serial.println("Start");
#endif  

_Clign.start();
_Clign_1.start();
_Animation.start();
_Animation_1.start();

    // init temporisator for the song Ticker
    //_tempo.attach_ms(1, []() {});
   // _tempo.detach();
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
    //comm.add(testInt); // Add from variable
    comm.send();       // Send concatened variable
    */

    _Clign.update();
    _Clign_1.update();
    _Animation.update();
     _Animation_1.update();
/*
    
    read_buttons(); // read buttons

    if (trigger_button == false && contact_button == false) // no button pressed
    {
        display_percent();
    }

    if (trigger_button == true && percent > 0) // trigger button pressed
    {
        unfilling(); // unfilling animation
    }

    if (contact_button == true && percent < 100) // contact button pressed
    {
        refilling(); // refilling animation
    }

    */
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