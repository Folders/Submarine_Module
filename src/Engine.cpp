#include <common.h>
#ifdef ENGINE

#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include <Bounce2.h>
#include <Ticker.h>

//////// Add new include library

////////  Define global constantes      (ALWAYS IN MAJ)

#define PIN 0
#define NUMPIXELS 4 // insert the total of pixels
Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

#define DIRECTION 12
#define CLUTCH 13
#define START 14

const int RESTARTING_TIME = 3; // time needed to restart motor

////////  Define global variables

Bounce start = Bounce();
Bounce direction = Bounce();
Bounce clutch = Bounce();

bool motor_dir = 0;      // direction of motor : 0 = backward, 1 = forward
bool declutch = 0;       // clutch state : 0 = clutched (embrayé) 1 = uncltuched (debrayé)
bool motor = 0;          // motor's on or off
bool stall = 0;          // stall or not
bool status_changed = 0; // the motor has changed
bool clutch_changed = 0; // clutch changed

int animation_led = 0; // for the led effect

Ticker _restart;    // Ticker for restarting the motor
Ticker _animation;  // Ticker for leds animation
int restarting = 0; // Counter for leds animation

/////////////////////////////////////////////////////////////////////////////////////////////
//                                      User function                                      //
/////////////////////////////////////////////////////////////////////////////////////////////

/// @brief read direction interrupt and change motor_dir
void read_direction()
{
    direction.update();

    if ((direction.changed()) && (declutch == 0)) // direction changed without clutch
    {
        if (stall == 0)
        {
            stall = 1;
            status_changed = 1;
        }

#ifdef LOG
        Serial.println("STALL");
#endif
    }

    else
    {

        if (direction.fell())
        {
            motor_dir = 1;
            status_changed = 1;

#ifdef LOG
            Serial.println("AVANT");
#endif
        }

        if (direction.rose())
        {
            motor_dir = 0;
            status_changed = 1;

#ifdef LOG
            Serial.println("ARRIÈRE");
#endif
        }
    }
}

/// @brief read clutch's interrupt and change declutch
void read_clutch()
{
    clutch.update();

    if (stall == 0) // only make change if the motor is not stalled
    {
        if (clutch.fell()) // motor's unclutch
        {
            declutch = 1;
            status_changed = 1;
            clutch_changed = 1;

#ifdef LOG
            Serial.println("DECLUTCH");
#endif
        }

        if (clutch.rose()) // motor's clutch
        {
            declutch = 0;
            status_changed = 1;
            clutch_changed = 1;

#ifdef LOG
            Serial.println("CLUTCH");
#endif
        }
    }
}

/// @brief Counter for leds animation
void clign_anim()
{
    animation_led++;
    if (animation_led >= 3)
    {
        animation_led = 0;
    }
}

/// @brief change the motor status depending situations
void motor_status()
{
    if (status_changed == 1) // make change only if motor status changed
    {
        _animation.detach(); // stop animation ticker

        if (stall == 1) // motor's stalled
        {
            motor = 0; // motor's off

            comm.send("MDR;S");

#ifdef LOG
            Serial.println("Le moteur à calé");
#endif
        }

        else
        {
            if (declutch == 0) // clutch is on
            {
                motor = 1; // motor's on
#ifdef LOG
                Serial.println("Le moteur est en marche");
#endif
                _animation.attach(0.5, clign_anim);

                if (motor_dir == 0)
                {
                    comm.send("MDR;B");
#ifdef LOG
                    Serial.println(" Il tourne en arrière");
#endif
                }

                if (motor_dir == 1)
                {
                    comm.send("MDR;F");
#ifdef LOG
                    Serial.println(" Il tourne en avant");
#endif
                }
            }

            if (declutch == 1 && clutch_changed == 1) // if motor is declutched and changed
            {
                motor = 0;
                comm.send("MDR;N");
                clutch_changed = 0;
#ifdef LOG
                Serial.println("Le moteur est debrayé");
#endif
            }
        }
        status_changed = 0;
    }
}

/// @brief display the motor status on LEDS
void display_motor_status()
{
    if (stall == 1) // motor's stall
    {
        pixels.clear();
        for (int i = 0; i < NUMPIXELS; i++)
        {
            pixels.setPixelColor(i, pixels.Color(250, 0, 0)); // turn pixels red
        }
        pixels.show();
    }

    else
    {
        if (motor == 0) // motor's decltuch
        {
            pixels.clear();
            for (int i = 0; i < NUMPIXELS; i++)
            {
                pixels.setPixelColor(i, pixels.Color(0, 0, 250)); // turn pixels blue
            }
            pixels.show();
        }

        else
        {
            if (motor_dir == 1) // motor's forwards
            {
                switch (animation_led)
                {
                case 0:
                    pixels.clear();
                    pixels.setPixelColor((animation_led + 2), pixels.Color(0, 250, 0));
                    pixels.show();
                    break;

                case 1:
                    pixels.clear();
                    pixels.setPixelColor((animation_led + 1), pixels.Color(0, 0, 0));
                    pixels.setPixelColor((animation_led + 2), pixels.Color(0, 250, 0));
                    pixels.show();
                    break;

                case 2:
                    pixels.clear();
                    pixels.show();
                    break;
                }
            }

            if (motor_dir == 0) // motor,s backwards
            {
                switch (animation_led)
                {
                case 0:
                    pixels.clear();
                    pixels.setPixelColor((animation_led + 1), pixels.Color(0, 250, 0));
                    pixels.show();
                    break;

                case 1:
                    pixels.clear();
                    pixels.setPixelColor((animation_led), pixels.Color(0, 0, 0));
                    pixels.setPixelColor((animation_led - 1), pixels.Color(0, 250, 0));
                    pixels.show();
                    break;

                case 2:
                    pixels.clear();
                    pixels.show();
                    break;
                }
            }
        }
    }
}

/// @brief restarting counter
void restartCounter()
{
    restarting++;
    if (restarting > RESTARTING_TIME)
    {
        restarting = RESTARTING_TIME;
    }
}

/// @brief read clutch and position at starting before motor's restarting
void starting_positions()
{
    clutch.update();

    bool clutch_state = clutch.read();

    if (clutch_state == 0)
    {
#ifdef LOG
        Serial.println("DECLUTCH");
#endif
        declutch = 1;
    }
    if (clutch_state == 1)
    {
#ifdef LOG
        Serial.println("CLUTCH");
#endif
        declutch = 0;
    }

    direction.update();

    bool direction_state = direction.read();

    if (direction_state == 0)
    {
#ifdef LOG
        Serial.println("AVANT");
#endif
        motor_dir = 1;
    }

    if (direction_state == 1)
    {
#ifdef LOG
        Serial.println("ARRIÈRE");
#endif
        motor_dir = 0;
    }
}

/// @brief read start button and start counting
void restart_motor()
{
    if (stall == 1) // read only if the motor's stall
    {
        start.update();

        if (start.fell()) // button has been pressed
        {
            _restart.attach(1, restartCounter);
        }

        if (start.rose()) // button has been released
        {
            _restart.detach();
            restarting = 0;
        }

        if (restarting == RESTARTING_TIME) // reach restart time
        {
            _restart.detach();
            restarting = 0;
#ifdef LOG
            Serial.println("Le moteur a redémaré");
#endif
            stall = 0;
            starting_positions();
            status_changed = 1;
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

    start.attach(START, INPUT);
    start.interval(5);

    direction.attach(DIRECTION, INPUT);
    direction.interval(5);

    clutch.attach(CLUTCH, INPUT);
    clutch.interval(5);

    starting_positions(); // read interrupts
    stall = 1;            // turn off motor (stall)

    /// Turn off all leds
    pixels.begin();
    pixels.setBrightness(50);
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
    read_clutch();
    read_direction();
    motor_status();
    display_motor_status();
    restart_motor();
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
            stall = 0;
            status_changed = 1;
        }

        if (i == 1)
        {
            stall = 1;
            status_changed = 1;
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