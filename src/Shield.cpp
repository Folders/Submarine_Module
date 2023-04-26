#include <common.h>
#ifdef SHIELD

//////// Add new include library
#include <Arduino.h>
#include <Bounce2.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include <Ticker.h>
#include <Shield.h>

////////  Define global constantes (ALWAYS IN MAJ, use pin number and not name)

// calibre for the map
const int OFFSET_X = 20; // 20
const int OFFSET_Y = 50; // 50

// sizes for the map
const int SQUARE_SIZE = 20;
const int SUBMARINE_WIDTH = 11;
const int SUBMARINE_LENGTH = 20;

// calibre for the minimap
const int OFFSET_M_X = 219;
const int OFFSET_M_Y = 1;

// walls
const int WALL_WIDTH = 1;
const int WALL_HEIGHT = 10;
const int WALL_OFFSET_X = 15;
const int WALL_OFFSET_Y = 8;

// table for the x & y verticals walls
bool WALLS_V[6][6][6] =
    {
        {{0, 1, 0, 1, 0, 0},
         {0, 0, 1, 0, 1, 0},
         {0, 0, 0, 1, 0, 0},
         {0, 0, 1, 0, 1, 0},
         {0, 1, 0, 0, 1, 0},
         {1, 0, 1, 0, 0, 0}},

        {{1, 0, 1, 0, 1, 0},
         {0, 0, 1, 0, 0, 0},
         {0, 1, 0, 0, 1, 0},
         {0, 0, 1, 1, 0, 0},
         {0, 1, 0, 1, 0, 0},
         {0, 1, 0, 0, 1, 0}},

        {{0, 1, 0, 0, 1, 0},
         {0, 1, 0, 1, 0, 0},
         {1, 0, 1, 1, 0, 0},
         {1, 1, 0, 0, 1, 0},
         {0, 1, 0, 0, 1, 0},
         {0, 0, 1, 0, 0, 0}},

        {{1, 0, 1, 0, 0, 0},
         {0, 1, 0, 0, 1, 0},
         {0, 0, 1, 1, 0, 0},
         {0, 0, 1, 1, 0, 0},
         {0, 1, 0, 0, 1, 0},
         {1, 0, 1, 1, 0, 0}},

        {{1, 0, 1, 0, 0, 0},
         {0, 1, 0, 0, 1, 0},
         {0, 0, 1, 1, 0, 0},
         {0, 0, 1, 1, 0, 0},
         {0, 1, 0, 0, 1, 0},
         {1, 0, 1, 1, 0, 0}},

        {{0, 0, 0, 0, 0, 0},
         {0, 0, 0, 0, 0, 0},
         {0, 0, 0, 0, 0, 0},
         {0, 0, 0, 0, 0, 0},
         {0, 0, 0, 0, 0, 0},
         {0, 0, 0, 0, 0, 0}}};

// table for the x & y horizontals walls
bool WALLS_H[6][6][6] =
    {
        {{1, 0, 0, 1, 0, 0},
         {0, 1, 0, 0, 0, 1},
         {1, 0, 1, 0, 0, 0},
         {0, 1, 0, 1, 0, 1},
         {0, 0, 0, 0, 1, 0},
         {0, 0, 0, 0, 0, 0}},

        {{0, 0, 1, 0, 1, 0},
         {0, 1, 0, 1, 0, 0},
         {1, 0, 0, 0, 0, 1},
         {1, 0, 1, 0, 1, 1},
         {0, 1, 0, 1, 0, 0},
         {0, 0, 0, 0, 0, 0}},

        {{0, 1, 1, 0, 0, 0},
         {0, 0, 0, 0, 1, 1},
         {1, 0, 1, 1, 0, 0},
         {0, 0, 1, 1, 0, 1},
         {0, 1, 0, 0, 1, 0},
         {0, 0, 0, 0, 0, 0}},

        {{0, 0, 1, 0, 1, 0},
         {0, 1, 1, 1, 0, 1},
         {1, 1, 0, 0, 1, 0},
         {0, 1, 1, 0, 1, 0},
         {0, 0, 0, 0, 0, 0},
         {0, 0, 0, 0, 0, 0}},

        {{0, 0, 1, 0, 1, 0},
         {0, 1, 1, 1, 0, 1},
         {1, 1, 0, 0, 1, 0},
         {0, 1, 1, 0, 1, 0},
         {0, 0, 0, 0, 0, 0},
         {0, 0, 0, 0, 0, 0}},

        {{0, 0, 0, 0, 0, 0},
         {0, 0, 0, 0, 0, 0},
         {0, 0, 0, 0, 0, 0},
         {0, 0, 0, 0, 0, 0},
         {0, 0, 0, 0, 0, 0},
         {0, 0, 0, 0, 0, 0}}};

// TFT
#define TFT_CS 14   // D0
#define TFT_DC 27   // D8
#define TFT_RST 33  // RST
#define TFT_MOSI 15 // D7
#define TFT_MISO 16 // D6
#define TFT_CLK 17  // D5

Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_MOSI, TFT_CLK, TFT_RST, TFT_MISO); // screen

/// buttons

const int INPUTPINS[] = {21, 19, 18, 32, 23}; // left - up - down - right - start
const int NUMBEROFINPUTS = sizeof(INPUTPINS) / sizeof(INPUTPINS[0]);

// create an array of Bounce objects to debounce each input
Bounce debouncers[NUMBEROFINPUTS];

const int TORPEDO_SPEED = 10; // speed of torpeedo for 1 case

////////  Define global variables
int countdown = 20; // countdown duration

byte game_state = 0;  // games states
byte game_result = 0; // victory or fail

int torpedo_direction; // to call different patern

///// map settings ///////
int positionX;
int positionY; // position variables
int positionX_Victoire;
int positionY_Victoire;

int color;
int countdowncolor;
bool i_color; // countdown & color vairables
bool i_clean;

// set the value of the countdown
int i_countdown = countdown;

// position animation

/// @brief make a clign effect for squares
void _Clign2()
{

    i_color = !i_color;

    if (i_color)
    {
        color = ILI9341_ORANGE;
    }

    if (!i_color)
    {
        color = ILI9341_BLACK;
    }
}

/// @brief fill countdown with black screen
void clean_countdown()
{
    if (i_clean != i_countdown % 2)
    {
        tft.fillRect(261, 201, 58, 38, ILI9341_BLACK);
        i_clean = !i_clean;
    }
}

/// @brief  decrease countdown
void Countdown()
{

    i_countdown--;
    if (i_countdown < 0)
    {
        i_countdown = 0; // stop the countdown at 0
    }
    clean_countdown();
}

/// @brief fill position in black
void clean_position()
{ // set the last position in black
    tft.fillRect(((positionX * 30) + 21), ((positionY * 30) + 51), 18, 18, ILI9341_BLACK);
}

Ticker _Clign(_Clign2, 500);
Ticker _Countdown(Countdown, 1000);
Ticker _Fail(un_Fail, 5000);
Ticker _Victory(un_Victory, 5000);


/////////////////////////////////////////////////////////////////////////////////////////////
//                                      User function                                      //
/////////////////////////////////////////////////////////////////////////////////////////////

/// @brief draw the map on the screen
void draw_map()
{
    tft.setCursor(40, 10);
    tft.setTextColor(ILI9341_BLUE);
    tft.setTextSize(2);
    tft.println("LABYRINTHE!");

    // Draw the maze
    for (int iX = 0; iX <= 5; iX++)
    {
        for (int iY = 0; iY <= 5; iY++)
        {
            tft.drawRect(((iX * (SQUARE_SIZE + 10)) + OFFSET_X), ((iY * (SQUARE_SIZE + 10)) + OFFSET_Y), SQUARE_SIZE, SQUARE_SIZE, ILI9341_WHITE);
        }
    }

    // Draw the minimap
    tft.drawRect(260, 200, 60, 40, ILI9341_WHITE);
    tft.drawRect(OFFSET_M_X, OFFSET_M_Y, 78, 78, ILI9341_WHITE); // the minimap rect.

    // Draw the submarine
    tft.fillRect((OFFSET_M_X + 34), (OFFSET_M_Y + 124), SUBMARINE_WIDTH, SUBMARINE_LENGTH, ILI9341_WHITE); // big part
    tft.fillCircle((OFFSET_M_X + 39), (OFFSET_M_Y + 124), SUBMARINE_LENGTH / 4, ILI9341_WHITE);            // narrow of the sub

    // Draw the left and right wings
    for (int i = 0; i <= 1; i++)
    {
        int offsetX = (i == 0 ? 31 : 44);
        int offsetY = (i == 0 ? 0 : 3);
        tft.fillRect((OFFSET_M_X + offsetX), (OFFSET_M_Y + 129), 3, 3, ILI9341_WHITE);
        tft.fillCircle((OFFSET_M_X + offsetX + offsetY), (OFFSET_M_Y + 130), 1, ILI9341_WHITE);
    }

    tft.fillRect((OFFSET_M_X + 34), (OFFSET_M_Y + 147), SUBMARINE_WIDTH, 4, ILI9341_WHITE);     // low rect
    tft.fillCircle((OFFSET_M_X + 39), (OFFSET_M_Y + 143), SUBMARINE_LENGTH / 6, ILI9341_WHITE); // low part
}

/// @brief draw a minimap on screen
void draw_minimap()
{
    // Draw minimap border
    tft.drawRect(OFFSET_M_X, OFFSET_M_Y, 78, 78, ILI9341_WHITE);

    // Draw dots on the minimap
    for (int iX = 0; iX <= 5; iX++)
    {
        for (int iY = 0; iY <= 5; iY++)
        {
            tft.fillRect(((iX * 11) + OFFSET_M_X + 9), ((iY * 11) + OFFSET_M_Y + 11), 3, 3, ILI9341_WHITE);
        }
    }

// Draw text on the minimap
#ifdef LOG
    tft.setCursor((OFFSET_M_X - 10), OFFSET_M_Y);
    tft.setTextColor(ILI9341_WHITE);
    tft.setTextSize(1);
    tft.println(torpedo_direction);
#endif

    // draw walls
    for (int iX = 0; iX <= 5; iX++)
    {
        for (int iY = 0; iY <= 5; iY++)
        {

            if (WALLS_V[torpedo_direction][iX][iY])
            {
                tft.fillRect((iY * 11) + OFFSET_M_X + WALL_OFFSET_X, (iX * 11) + OFFSET_M_Y + WALL_OFFSET_Y, WALL_WIDTH, WALL_HEIGHT, ILI9341_YELLOW);
            }

            if (WALLS_H[torpedo_direction][iX][iY])
            {
                tft.fillRect((iY * 11) + OFFSET_M_X + 5, (iX * 11) + OFFSET_M_Y + 17, WALL_HEIGHT, WALL_WIDTH, ILI9341_YELLOW);
            }
        }
    }

    // draw arrow and label for Torpeedo direction
    switch (torpedo_direction)
    {
    case 0: // North

        // Up arrow
        tft.fillRect((OFFSET_M_X + 38), (OFFSET_M_Y + 104), 3, 8, ILI9341_RED);
        tft.fillTriangle((OFFSET_M_X + 39), (OFFSET_M_Y + 115), (OFFSET_M_X + 37), (OFFSET_M_Y + 112), (OFFSET_M_X + 41), (OFFSET_M_Y + 112), ILI9341_RED);
        tft.setCursor((OFFSET_M_X + 34), (OFFSET_M_Y + 87));
        tft.setTextColor(ILI9341_RED);
        tft.setTextSize(2);
        tft.println("N");
        break;

    case 1: // South
        // down arrow
        tft.fillRect((OFFSET_M_X + 38), (OFFSET_M_Y + 158), 3, 9, ILI9341_RED);
        tft.fillTriangle((OFFSET_M_X + 39), (OFFSET_M_Y + 155), (OFFSET_M_X + 37), (OFFSET_M_Y + 158), (OFFSET_M_X + 41), (OFFSET_M_Y + 158), ILI9341_RED);
        tft.setCursor((OFFSET_M_X + 35), (OFFSET_M_Y + 172));
        tft.setTextColor(ILI9341_RED);
        tft.setTextSize(2);
        tft.println("S");
        break;

    case 2: // Nort-East
        // NE arrow
        tft.fillTriangle((OFFSET_M_X + 53), (OFFSET_M_Y + 122), (OFFSET_M_X + 53), (OFFSET_M_Y + 126), (OFFSET_M_X + 57), (OFFSET_M_Y + 126), ILI9341_RED);
        tft.drawLine((OFFSET_M_X + 52), (OFFSET_M_Y + 125), (OFFSET_M_X + 59), (OFFSET_M_Y + 118), ILI9341_RED);
        tft.drawLine((OFFSET_M_X + 52), (OFFSET_M_Y + 125), (OFFSET_M_X + 60), (OFFSET_M_Y + 119), ILI9341_RED);
        tft.setCursor((OFFSET_M_X + 63), (OFFSET_M_Y + 105));
        tft.setTextColor(ILI9341_RED);
        tft.setTextSize(2);
        tft.println("NE");
        break;

    case 3: // south-east

        // SE arrow
        tft.fillTriangle((OFFSET_M_X + 52), (OFFSET_M_Y + 146), (OFFSET_M_X + 50), (OFFSET_M_Y + 142), (OFFSET_M_X + 54), (OFFSET_M_Y + 142), ILI9341_RED);
        tft.drawLine((OFFSET_M_X + 53), (OFFSET_M_Y + 144), (OFFSET_M_X + 59), (OFFSET_M_Y + 148), ILI9341_RED);
        tft.drawLine((OFFSET_M_X + 53), (OFFSET_M_Y + 144), (OFFSET_M_X + 58), (OFFSET_M_Y + 149), ILI9341_RED);
        tft.setCursor((OFFSET_M_X + 63), (OFFSET_M_Y + 144));
        tft.setTextColor(ILI9341_RED);
        tft.setTextSize(2);
        tft.println("SE");
        break;

    case 4: // Nort-West

        // NW arrow
        tft.fillTriangle((OFFSET_M_X + 26), (OFFSET_M_Y + 146), (OFFSET_M_X + 24), (OFFSET_M_Y + 142), (OFFSET_M_X + 28), (OFFSET_M_Y + 142), ILI9341_RED);
        tft.drawLine((OFFSET_M_X + 25), (OFFSET_M_Y + 144), (OFFSET_M_X + 19), (OFFSET_M_Y + 148), ILI9341_RED);
        tft.drawLine((OFFSET_M_X + 25), (OFFSET_M_Y + 144), (OFFSET_M_X + 20), (OFFSET_M_Y + 149), ILI9341_RED);
        tft.setCursor((OFFSET_M_X - 8), (OFFSET_M_Y + 144));
        tft.setTextColor(ILI9341_RED);
        tft.setTextSize(2);
        tft.println("NW");
        break;

    case 5: // South-West

        // SW arrow

        tft.fillTriangle((OFFSET_M_X + 24), (OFFSET_M_Y + 122), (OFFSET_M_X + 22), (OFFSET_M_Y + 126), (OFFSET_M_X + 26), (OFFSET_M_Y + 126), ILI9341_RED);
        tft.drawLine((OFFSET_M_X + 23), (OFFSET_M_Y + 124), (OFFSET_M_X + 17), (OFFSET_M_Y + 120), ILI9341_RED);
        tft.drawLine((OFFSET_M_X + 23), (OFFSET_M_Y + 124), (OFFSET_M_X + 18), (OFFSET_M_Y + 119), ILI9341_RED);
        tft.setCursor((OFFSET_M_X - 8), (OFFSET_M_Y + 105));
        tft.setTextColor(ILI9341_RED);
        tft.setTextSize(2);
        tft.println("SW");

        break;
    }
}

/// @brief draw player position and victory position
void draw_position()
{
    tft.fillRect((positionX_Victoire * 30) + OFFSET_X + 1, (positionY_Victoire * 30) + OFFSET_Y + 1, 18, 18, ILI9341_GREEN); // fill the victory square in green
    tft.fillRect((positionX * 30) + OFFSET_X + 1, (positionY * 30) + OFFSET_Y + 1, 18, 18, color);                           // fill the position square in orange flashing black
    clean_countdown();                                                                                                       // draw the countdown on the map
    tft.setCursor(266, 206);
    tft.setTextColor(ILI9341_RED);
    tft.setTextSize(4);
    tft.print(i_countdown);
}

/// @brief write "perdu" on red sreen
void failure()
{

    tft.setCursor(45, 100);
    tft.setTextColor(ILI9341_BLACK);
    tft.setTextSize(5);
    tft.println("Perdu :( ");
}

/// @brief write "Bravo" on green screen
void victory()
{

    tft.setCursor(75, 100);
    tft.setTextColor(ILI9341_BLACK);
    tft.setTextSize(5);
    tft.println("Bravo!");
}

/// @brief read buttons and make move
void read_buttons()
{
    for (int i = 0; i < NUMBEROFINPUTS; i++)
    {
        debouncers[i].update();
    }

    // check the state of each input and take action if necessary

    if (debouncers[0].fell()) // going left
    {
        if (debug)
        {
            comm.send("BTN;LEF;1");
        }
#ifdef LOG
        Serial.println("LEFT");
#endif

        if (positionX == 0 || WALLS_V[torpedo_direction][positionY][positionX - 1]) // they hit a wall or go outside
        {
            game_result = 1; // fail

            if (debug)
            {
                comm.send("SHI;MOV");
            }

#ifdef LOG
            Serial.println("Mauvais mouvement vers la gauche");
#endif
        }
        else // they don't hit a wall
        {
            clean_position(); // refresh the position
            positionX -= 1;   // going left
        }
    }

    if (debouncers[1].fell()) // going up
    {
        if (debug)
        {
            comm.send("BTN;UP;1");
        }
#ifdef LOG
        Serial.println("UP");
#endif
        if (positionY == 0 || WALLS_H[torpedo_direction][positionY - 1][positionX]) // they hit a wall or go outside
        {
            game_result = 1; // fail

            if (debug)
            {
                comm.send("SHI;MOV");
            }

#ifdef LOG
            Serial.println("Mauvais mouvement vers la haut");
#endif
        }

        else // they don't hit a wall
        {
            clean_position(); // refresh the position
            positionY -= 1;   // going up
        }
    }

    if (debouncers[2].fell()) // going down
    {
        if (debug)
        {
            comm.send("BTN;DOW;1");
        }
#ifdef LOG
        Serial.println("DOWN");
#endif
        if (positionY == 5 || WALLS_H[torpedo_direction][positionY][positionX]) // they hit a wall or go outside
        {
            game_result = 1; // fail

            if (debug)
            {
                comm.send("SHI;MOV");
            }

#ifdef LOG
            Serial.println("Mauvais mouvement vers le bas");
#endif
        }
        else // they don't hit a wall
        {
            clean_position(); // refresh the position
            positionY += 1;   // going down
        }
    }

    if (debouncers[3].fell()) // going right
    {
        if (debug)
        {
            comm.send("BTN;RIG;1");
        }
#ifdef LOG
        Serial.println("RIGHT");
#endif
        if (positionX == 5 || WALLS_V[torpedo_direction][positionY][positionX])
        { // they hit a wall or go outside

            game_result = 1; // fail

            if (debug)
            {
                comm.send("SHI;MOV");
            }

#ifdef LOG
            Serial.println("Mauvais mouvement vers la droite");
#endif
        }

        else
        {
            clean_position(); // refresh the position
            positionX += 1;   // going right
        }
    }

    if (debouncers[4].fell()) // start button
    {
    }

    if (debug)
    {
        if (debouncers[0].rose())
        {
            comm.send("BTN;LEF;0");
        }
        if (debouncers[1].rose())
        {
            comm.send("BTN;UP;0");
        }
        if (debouncers[2].rose())
        {
            comm.send("BTN;DOW;0");
        }
        if (debouncers[3].rose())
        {
            comm.send("BTN;RIG;0");
        }
    }
}

/// @brief  compare start and victory position
/// @param positionX
/// @param positionY
/// @param positionX_Victoire
/// @param positionY_Victoire
/// @return
bool tooclose(int positionX, int positionY, int positionX_Victoire, int positionY_Victoire)
{
    return abs(positionX - positionX_Victoire) <= 1 && abs(positionY - positionY_Victoire) <= 1;
}

/// @brief made the game setup with random start and victory position.
void game_setup()
{
    i_countdown = countdown;
    positionX = random(0, 6);
    positionY = random(0, 6); // random start position

    positionX_Victoire = random(0, 6);
    positionY_Victoire = random(0, 6); // random victory position

    while (tooclose(positionX, positionY, positionX_Victoire, positionY_Victoire))
    {
        positionX_Victoire = random(0, 6);
        positionY_Victoire = random(0, 6);
    }

    /*
  // for testing
  torpedo_direction = random(0, 5); // randomise the torpeedo direction
*/
    _Clign.start();     // start the ticker for flashing the position
    _Countdown.start(); // start the ticker for the countdown

#ifdef LOG
    Serial.print("Votre position :");
    Serial.print(positionX);
    Serial.print(';');
    Serial.println(positionY);

    Serial.print("Position de victoire :");
    Serial.print(positionX_Victoire);
    Serial.print(';');
    Serial.println(positionY_Victoire);
#endif

    draw_minimap(); // draw the minimap
}

/// @brief check if countdown is 0, victory point reached or false move.
void check_game_status()
{
    // if the countdown reach 0, it's a fail
    if (i_countdown <= 0)
    {
#ifdef LOG
        Serial.println("Countdown à zéro");
#endif

        if (debug)
        {
            comm.send("SHI;CTD");
        }
        game_result = 1;
    }

    // victory point has been reach
    if (positionX == positionX_Victoire && positionY == positionY_Victoire)
    {
#ifdef LOG
        Serial.println("VICTOIRE");
#endif

        game_result = 2;
    }

    // fail
    if (game_result == 1)
    {
        comm.send("SHI;0"); // send information to server
        _Countdown.stop();
        tft.fillScreen(ILI9341_BLACK);
        tft.fillScreen(ILI9341_RED);

#ifdef LOG
        Serial.println("ECHEC");
#endif

        game_state = 3; // waiting and make nothing
        failure();      // write "Echec :( " on the screen
        _Fail.start();  // reboot
    }

    // victory
    if (game_result == 2)
    {
        comm.send("SHI;1"); // send information to server
        _Countdown.stop();
        tft.fillScreen(ILI9341_BLACK);
        tft.fillScreen(ILI9341_GREEN);

#ifdef LOG
        Serial.println("BRAVO");
#endif

        game_state = 3;   // waiting and make nothing
        victory();        // write "Bravo" on the screen
        _Victory.start(); // reboot
    }
}

/// @brief stop fail screen
void un_Fail()
{
    game_state = 4; // reset the game
    _Fail.stop();
}

/// @brief stop victory screen
void un_Victory()
{
    game_state = 4; // reset the game
    _Victory.stop();
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

    // Buttons
    for (int i = 0; i < NUMBEROFINPUTS; i++)
    {
        pinMode(INPUTPINS[i], INPUT_PULLUP);
        debouncers[i].attach(INPUTPINS[i]);
        debouncers[i].interval(5); // set the debounce time to 5 milliseconds
    }

    tft.begin();        // start the screen
    tft.setRotation(3); // landscreen, connexion up
    tft.fillScreen(ILI9341_BLACK);

    randomSeed(analogRead(1)); // generate the seed

    // start animations tickers
    _Clign.start();
    _Countdown.start();

    draw_map();
}

///////////////////////////////  Reset all proprety of module  ////////////////////////////////

/// @brief Call after the config and when the module reset by the app
void ResetModule()
{
    int countdown = 20; // countdown duration

    byte game_state = 0;  // games states
    byte game_result = 0; // victory or fail
}

/////////////////////////////////  Write here the loop code  /////////////////////////////////

/// @brief Call at the end of the main loop function
void MyLoop()
{
    // update tickers
    _Clign.update();
    _Countdown.update();
    _Fail.update();
    _Victory.update();

    /// @brief make things depending game status
    switch (game_state)
    {
    case 0:         // display the map without anything else
        draw_map(); // draw the map

        // for testing
        debouncers[4].update(); // read the start button
        if (debouncers[4].fell())
        {
#ifdef LOG
            Serial.println("START");
#endif
            game_state = 1; // go to next state
        }
        break;

    case 1: // randomise position and victory position, start the countdown and the minimap
#ifdef LOG
        Serial.println("Game State 1");
#endif
        game_setup();   // display informations and define positions
        game_state = 2; // go to next state

        break;

    case 2: // gaming state

        read_buttons();      // read buttons
        check_game_status(); // check if it's victory or fail
        if (game_result < 1)
        {
            draw_position(); // draw new position on map
        }
        break;

    case 3:
        game_result = 0; // change result to neutral
        break;

    case 4:
        _Countdown.start();                          // restart countdown refresh
        tft.fillRect(0, 0, 320, 240, ILI9341_BLACK); // clean the screen
        draw_map();                                  // draw initial map
        game_state = 0;                              // reboot
        break;
    }

    // To send datas to the server, use the send function
    comm.send("LED;R;1");

    // It's possible to send with more then on line
    comm.start();    // Open the buffer
    comm.add("LED"); // Write String
    comm.add(';');   // Add char
    // comm.add(testInt); // Add from variable
    comm.send(); // Send concatened variable
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

    if (comm.GetCode() == "SHI")
    {
        torpedo_direction = comm.GetParameter(1).toInt();
        int i = comm.GetParameter(2).toInt();
        countdown = ((i + 1) * TORPEDO_SPEED);
        
#ifdef LOG
        Serial.println("START");
#endif
        game_state = 1; // go to next state
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