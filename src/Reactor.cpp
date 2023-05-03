#include <common.h>
#ifdef REACTOR

#include <Arduino.h>

//////// Add new include library
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include <Ticker.h>

////////  Define global constantes (ALWAYS IN MAJ, use pin number and not name)
// const int TEST_IN = 10;
// const int TEST_OUT = 11;

////////  Define global variables
int _testInt = 0;
bool _testBool = 0;

int _ConsoPower = 50;
int _TempNeeded = 50;
int _Tol = 5;

// Timmer
Ticker Time_Update;

#ifdef ESP8266

// TFT for ESP3266
#define TFT_RST -1  // RST
#define TFT_DC 15   // D8
#define TFT_MOSI 13 // D7
#define TFT_MISO 12 // D6
#define TFT_CLK 14  // D5
#define TFT_CS 16   // D0

#elif defined(ESP32)
// TFT for ESP32
#define TFT_RST 33  // RST
#define TFT_DC 27   // D8
#define TFT_MOSI 15 // D7
#define TFT_MISO 16 // D6
#define TFT_CLK 17  // D5
#define TFT_CS 14   // D0
#endif

Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_MOSI, TFT_CLK, TFT_RST, TFT_MISO); // screen

/////////////////////////////////////////////////////////////////////////////////////////////
//                                      User function                                      //
/////////////////////////////////////////////////////////////////////////////////////////////

/// @brief Si je tape ///, il me propose de mettre des commentaires à la fonction
void my_function()
{
    // ...
}

/// @brief Draw a standard gauge componant
/// @param width Width of the gauge [pix]
/// @param hight Height of the gauge [pix]
/// @param left Final left position [pix]
/// @param top Final top position [pix]
/// @param filling Current position of the filling [0..100%]
/// @param tol Tolerence of the filling [0..100%]
/// @param color_low Color of the low side of gauge
/// @param color_high Color of the high side of gauge
void DrawGauge(int16_t width, int16_t height, int16_t left, int16_t top, float filling, float tol, uint16_t color_low, uint16_t color_high)
{
    // DrawGauge(21, 181, 50, 0, 290, 10, ILI9341_BLUE, ILI9341_RED);

    bool IsVert;
    int16_t radius;
    int16_t length;

    // Canvas of power gauge
    GFXcanvas16 canvas(width, height);

    // Define direction of gauge
    if (height > width)
    {
        // Compute radius for vertical gauge
        IsVert = true;
        radius = width / 2;
        length = height - radius - radius;
    }
    else
    {
        // Compute radius for horizontal gauge
        IsVert = false;
        radius = height / 2;
        length = width - radius - radius;
    }

    // Gauge computation
    //                     pos_gauge (filling)
    //                     /
    //         (       |  |  |              )
    //         /      /       \             \
    //        0    pos_low    pos_high      length    ->  [pixel]

    // Compute filling gauge position
    float pas = static_cast<float>(length) / 100.0;
    float pos_gauge = pas * filling;
    float pos_high = pos_gauge + (pas * tol);
    float pos_low = pos_gauge - (pas * tol);

    // Limit high value
    if (pos_high > length)
        pos_high = length;

    // Limit low value
    if (pos_low < 0)
        pos_low = 0;

    // Convert in pixel
    int16_t pix_gauge = static_cast<int16_t>(pos_gauge);
    int16_t pix_high = static_cast<int16_t>(pos_high);
    int16_t pix_low = static_cast<int16_t>(pos_low);

    // Outside rectangle
    canvas.fillRoundRect(0, 0, width, height, radius, ILI9341_WHITE);


    // Draw circle of higher value. If the value is at maximum, don't draw the circle
    if (pos_high < length)
    {
        if (IsVert)
        {
            // Draw top circle
            canvas.fillCircle(radius, radius, radius - 2, color_high);
        }
        else
        {
            // Draw right circle
            canvas.fillCircle(width - radius - 1, radius, radius - 2, color_high);
        }
    }


    // Draw circle of lower value. If the value is at minumum, don't draw the circle
    if (pos_low > 0)
    {
        if (IsVert)
        {
            // Draw bottom circle
            canvas.fillCircle(radius, height - radius - 1, radius - 2, color_low);
        }
        else
        {
            // Draw left circle
            canvas.fillCircle(radius, radius, radius - 2, color_low);
        }
    }


    // Write near rectangle (Two pixel smaller). On vertical, the near = the high value
    if (IsVert)
    {
        // Draw top rectangle
        canvas.fillRect(2, radius, width - 4, length - pix_gauge, color_high);
        // Draw bottom rectangle
        canvas.fillRect(2, radius + length - pix_gauge, width - 4, pix_gauge, color_low);

        // Draw tolerance rectangle
        canvas.fillRect(2, radius + length - pix_high, width - 4, pix_high - pix_low, ILI9341_WHITE);
    }
    else
    {
        // Draw left rectangle
        canvas.fillRect(radius, 2, pix_gauge, height - 4, color_low);
        // Draw right rectangle
        canvas.fillRect(radius + pix_gauge, 2, length - pix_gauge, height - 4, color_high);

        // Draw tolerance rectangle
        canvas.fillRect(radius + pix_low, 2, pix_high - pix_low, height - 4, ILI9341_WHITE);
    }


    // Put the canvas at the right side
    tft.drawRGBBitmap(left, top, canvas.getBuffer(), canvas.width(), canvas.height());
}


/// @brief Draw the power gauge. Size: 21x181 - Pos: 290;10 
void Draw_PowerGauge()
{
    // Size max: 320x240

    // Draw power gauge
    DrawGauge(21, 181, 290, 10, _ConsoPower, _Tol, ILI9341_MAROON, ILI9341_YELLOW);
}

/// @brief Draw the temperature gauge. Size: 250x21 - Pos: 10;210
void Draw_TempGauge()
{
    // Size 320x240

    // Test horizontal power gauge
    DrawGauge(250, 21, 10, 210, _TempNeeded, _Tol, ILI9341_BLUE, ILI9341_RED);
}

int mapInt(int x, int in_min, int in_max, int out_min, int out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}




#define GRAPH_WIDTH 250     // Multiple of graph step
#define GRAPH_HEIGHT 200    //
#define GRAPH_TOP 2         // Min = 2
#define GRAPH_LEFT 2        // Min = 2
#define GRAPH_STEP 5       

int _graphInd;
int _graphMax;
int _graphOld = 0;

void Draw_Graph()
{
    // Canvas of power gauge
    GFXcanvas1  canvas(GRAPH_STEP, GRAPH_HEIGHT);
 
    // Get a random value in range [%]
    int val = (rand() % (_Tol*2+1) );
    val = _ConsoPower + val - _Tol;

#ifdef LOG
    Serial.printf("Power is %i%% and the tolerance is ±%i%%. The random value is %i%%\n", _ConsoPower, _Tol, val);
#endif
    
    // Define trace point
    int y0  = mapInt(_graphOld, 0, 100, GRAPH_HEIGHT, 0);
    int y1    = mapInt(val, 0, 100, GRAPH_HEIGHT, 0);
    int x0 = 0;
    int x1 = GRAPH_STEP - 1;

#ifdef LOG
    Serial.printf("Line go from (%i;%i) to (%i;%i)\n", x0, y0, x1, y1);
#endif

    ////////////////////// Normal line  -> STACK OVERFLOW
    //tft.drawLine(0, startY, GRAPH_STEP-1, endY, 0xFFFF);
    

    // Calculer les différences entre les coordonnées des deux points
    int dx = abs(x1 - x0);
    int dy = abs(y1 - y0);

    // Déterminer dans quelle direction dessiner la ligne
    int sx = (x0 < x1) ? 1 : -1;
    int sy = (y0 < y1) ? 1 : -1;

    // Initialiser les variables d'erreur
    int err = dx - dy;
    int e2;

    // Boucle principale de dessin
    while (true) {
        // Dessiner le pixel courant
        canvas.drawPixel(x0, y0, 0xFFFF);

        // Vérifier si on est arrivé au point final
        if (x0 == x1 && y0 == y1) {
        break;
        }

        // Calculer l'erreur pour le prochain pixel
        e2 = 2 * err;

        // Corriger l'erreur si nécessaire et avancer d'un pixel
        if (e2 > -dy) {
        err -= dy;
        x0 += sx;
        }
        if (e2 < dx) {
        err += dx;
        y0 += sy;
        }
    }

    // Draw new
    tft.drawBitmap(GRAPH_LEFT + _graphInd, GRAPH_TOP, canvas.getBuffer(), canvas.width(), canvas.height(), 0xFFFF, 0x0000);
    
    // Update graph position
    _graphOld = val;
    _graphInd += GRAPH_STEP;

    // Overflow control
    if (_graphInd >= GRAPH_WIDTH)
        _graphInd = 0;
}

void Draw_Update()
{
    Draw_Graph();
}

/////////////////////////////////////////////////////////////////////////////////////////////
//                                     Setup and reset                                     //
/////////////////////////////////////////////////////////////////////////////////////////////

/// @brief Setup function for the module
void MySetup()
{
    // Start TFT
    tft.begin();        // start the screen
    tft.setRotation(1); // landscreen, connexion up

    // Draw every thing on graphique
    ResetModule();

    // Run timmer to try to connect Unity server
    Time_Update.attach(1, Draw_Update);

}

///////////////////////////////  Reset all proprety of module  ////////////////////////////////

/// @brief Call after the config and when the module reset by the app
void ResetModule()
{
    // Empty screen
    tft.fillScreen(ILI9341_BLACK);

    // Draw power gauge
    Draw_PowerGauge();

    // Draw temperature gauge
    Draw_TempGauge();

    // Draw external rectangle of graphique
    tft.drawRect(GRAPH_LEFT-1, GRAPH_TOP-1, GRAPH_WIDTH+2, GRAPH_HEIGHT+2, ILI9341_WHITE);
    tft.drawRect(GRAPH_LEFT-2, GRAPH_TOP-2, GRAPH_WIDTH+4, GRAPH_HEIGHT+4, ILI9341_WHITE);
    // And the graphique
    Draw_Graph();

}

/////////////////////////////////  Write here the loop code  /////////////////////////////////

/// @brief Call at the end of the main loop function
void MyLoop()
{
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
    if (comm.GetCode() == "SPP")
    {
        // If you need to check the number of parameter available, use the function comm.GetSize()
        int NewConso = comm.GetParameter(1).toInt();

        if (NewConso != _ConsoPower)
        {
            // Update parameter
            _ConsoPower = NewConso;

            // Update power gauge
            Draw_PowerGauge();
        }
    }


    // The get the received code, use the function GetCode()
    if (comm.GetCode() == "SPT")
    {
        // If you need to check the number of parameter available, use the function comm.GetSize()
        int NewTemp = comm.GetParameter(1).toInt();

        if (NewTemp != _TempNeeded)
        {
            // Update parameter
            _TempNeeded = NewTemp;

            // Update power gauge
            Draw_PowerGauge();
        }
    }

    // The get the received code, use the function GetCode()
    if (comm.GetCode() == "TOL")
    {
        // If you need to check the number of parameter available, use the function comm.GetSize()
        int NewTol = comm.GetParameter(1).toInt();

        if (NewTol != _Tol)
        {
            // Update parameter
            _Tol = NewTol;

            // Update power gauge
            Draw_PowerGauge();
            Draw_TempGauge();
        }
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