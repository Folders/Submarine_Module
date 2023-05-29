#include <common.h>
#ifdef ENGINE

#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include <Bounce2.h>
#include <Ticker.h>

//////// Add new include library

////////  Define global constantes      (ALWAYS IN MAJ)

#define PIN 16
#define NUMPIXELS 4 // insert the total of pixels
Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_RGB + NEO_KHZ800);

#define DIRECTION 15
#define CLUTCH 13
#define START 12

const int RESTARTING_TIME = 5;

////////  Define global variables

Bounce start = Bounce();
Bounce direction = Bounce();
Bounce clutch = Bounce();

bool motor_dir = 0; // direction of motor
bool declutch = 0;  // clutch state
bool motor = 0;     // motor on or off
bool error = 0;     // stall or not

Ticker _restart;
int restarting = 0;

/////////////////////////////////////////////////////////////////////////////////////////////
//                                      User function                                      //
/////////////////////////////////////////////////////////////////////////////////////////////

/// @brief Si je tape ///, il me propose de mettre des commentaires à la fonction
void my_function()
{
}

/// @brief read direction interrupt and change motor_dir
void read_direction()
{
    direction.update();

    if (direction.changed() && (declutch == 0)) // direction changed without clutch
    {
        error = 1;
    }

    if (direction.fell())
    {
        motor_dir = 1;
    }

    if (direction.rose())
    {
        motor_dir = 0;
    }
}

/// @brief read clutch interrupt and change declutch
void read_clutch()
{
   
    bool state = digitalRead(CLUTCH);

    Serial.println(state);

    if (state == LOW) // Bouton enfoncé
    {
        Serial.println("Ok");
        declutch = 0;
    }
    else // Bouton relâché
    {
        Serial.println("KO");
        declutch = 1;
    }
   
   
   
   /* clutch.update();
    bool state = clutch.read() ; 

    Serial.println(state);

    if (clutch.fell())
    {
        Serial.println("Ok");
        declutch = 0;
    }
    if (clutch.rose())
    {
        Serial.println("KO");
        declutch = 1;
    }
    */
}

void motor_status()
{

    if (error == 1) // there is an error
    {
        motor = 0; // motor's stall
      //  comm.send("MDR;S");
#ifdef LOG
       // Serial.println("Le moteur à calé");
#endif
    }

    else
    {
        if (declutch == 1) // clutch is on
        {
            motor = 1; // motor's on

            if (motor_dir == 0)
            {
         //       comm.send("MDR;B");
#ifdef LOG
       //         Serial.println("Le moteur tourne en arrière");
#endif
            }

            if (motor_dir == 1)
            {
        //        comm.send("MDR;F");
#ifdef LOG
        //        Serial.println("Le moteur tourne en avant");
#endif
            }
        }
    }

    if (declutch == 0) // if motor is declutched
    {
        motor = 0;
    //    comm.send("MDR;N");
#ifdef LOG
    //    Serial.println("Le moteur est debrayé");
#endif
    }
}

/// @brief display the motor status on LEDS
void display_motor_status()
{
    if (error == 1) // motor's stall
    {
        pixels.clear();
        for (int i = 0; i < NUMPIXELS; i++)
        {
            pixels.setPixelColor(i, pixels.Color(256, 0, 0)); // turn pixels red
        }
        pixels.show();
    }

    else
    {
        if (motor == 0) // motor's decltuch     -> need to have a motor's off status (without declutch?)
        {
            pixels.clear();
            for (int i = 0; i < NUMPIXELS; i++)
            {
                pixels.setPixelColor(i, pixels.Color(0, 0, 256)); // turn pixels blue
            }
            pixels.show();
        }

        else
        {
            if (motor_dir == 1) // motor's forwards
            {
                pixels.clear();
                for (int i = 2; i < NUMPIXELS; i++)
                {
                    pixels.setPixelColor(i, pixels.Color(0, 256, 0)); // turn 2 lasts pixels green
                }
                pixels.show();
            }

            if (motor_dir == 0) // motor,s backwards
            {
                pixels.clear();
                for (int i = 0; i < 2; i++)
                {
                    pixels.setPixelColor(i, pixels.Color(0, 256, 0)); // turn 2 firsts pixels green
                }
            }
            pixels.show();
        }
    }
}

/// @brief restarting counter
void restartCounter()
{
    restarting++;
}

/// @brief read start button and start counting
void restart_motor()
{
    if (error == 1)
    {
        start.update();

        if (start.fell())
        {
            _restart.attach(1, restartCounter);
        }

        if (start.rose())
        {
            _restart.detach();
            restarting = 0;
        }

        if (restarting >= RESTARTING_TIME)
        {
            _restart.detach();
            restarting = 0;
            error = 0;
        }
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
    Serial.println("--- Model ---");
#endif

    pinMode(DIRECTION, INPUT);
   // pinMode(CLUTCH, INPUT);
    pinMode(START, INPUT);

    start.attach(START);
    start.interval(5);

    direction.attach(DIRECTION);
    direction.interval(5);

   // clutch.attach(CLUTCH, INPUT_PULLUP);
   // clutch.interval(5);

    /// Turn off all leds
    pixels.clear();
    pixels.show();
}

///////////////////////////////  Reset all proprety of module  ////////////////////////////////

/// @brief Call after the config and when the module reset by the app
void ResetModule()
{
     pinMode(CLUTCH, INPUT);
    /// Turn off all leds
    pixels.clear();
    pixels.show();
}

/////////////////////////////////  Write here the loop code  /////////////////////////////////

/// @brief Call at the end of the main loop function
void MyLoop()
{
    read_clutch();
   // read_direction();
  //  motor_status();
  //  display_motor_status();
 //  restart_motor();
}

/////////////////////////////////////////////////////////////////////////////////////////////
//                                      Communication                                      //
/////////////////////////////////////////////////////////////////////////////////////////////

/// @brief Call when a message is received from server (or serial)
void Received()
{

    // If you want debug the received string, you can write this commande:
    comm.Info_Received();

    if (comm.GetCode() == "MRN")
    {
        int i = comm.GetParameter(1).toInt();

        if (i == 0)
        {
            motor = 0;
        }

        if (i == 1)
        {
            motor = 1;
        }
    }

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