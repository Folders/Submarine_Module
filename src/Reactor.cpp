#include <common.h>
#ifdef REACTOR

#include <Arduino.h>


#ifdef ESP8266
#error "Only use an ESP32"
// 1. The TFT drawLine function make an overflow for the ESP8266 because of yield function.
// 2. We need two analog input
#endif



//////// Add new include library
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include <Ticker.h>

/*                 -----  Screen layout  -----


    0 ->                                                       320 (LEFT)
    |   __________________________________________________________
    V   |  ________________________________________          __   |
        |  |                                       |     |   | |  |
        |  |                                       |     |   | |  |
        |  |                                       |     |   | |  |
        |  |              (1)                      |     |   | |  |
        |  |             GRAPH                     |  (4)►   | |  |
        |  |                                       |     |   | |  |
        |  |                                       |     |   |2|  |
        |  |                                       |     |   | |  |
        |  |                                       |     |   | |  |
        |  |_______________________________________|     |   | |  |
        |                                                |   | |  |
        |                      (5)                       |   | |  |
        |  ---------------------▼--------------------    |   |_|  |
        |  __________________________________________             |
        |  |_________________________________________|(3)         |
   240  |_________________________________________________________|
  (TOP)

*/

// 1. Graph position and size
#define GRAPH_WIDTH 250         // Multiple of graph step
#define GRAPH_HEIGHT 200
#define GRAPH_LEFT 2            // Min = 2
#define GRAPH_TOP 2             // Min = 2
#define GRAPH_STEP 5            // Width of graph update

// 2. Power gauge
#define GAUGE_POWER_WIDTH 21
#define GAUGE_POWER_HEIGHT 181
#define GAUGE_POWER_LEFT 290
#define GAUGE_POWER_TOP 10 

// 3. Temperature gauge
#define GAUGE_TEMP_WIDTH 250
#define GAUGE_TEMP_HEIGHT 21
#define GAUGE_TEMP_LEFT 10
#define GAUGE_TEMP_TOP 210 



/* ESP8266 Analog Pin ADC0 = A0 */
#define ANALOG_POWER 16 
#define ANALOG_TEMP 18 




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
#define TFT_RST -1 // RST
#define TFT_DC 15  // D8

#define TFT_SCK 14  // D5
#define TFT_MISO 12 // D6
#define TFT_MOSI 13 // D7
#define TFT_CS 16   // D0

#elif defined(ESP32)

// TFT for ESP32
#define TFT_RST -1 // RST
#define TFT_DC 12  // D8

#define TFT_SCK 7   // Ok
#define TFT_MISO 9  // Ok
#define TFT_MOSI 11 // Ok
#define TFT_CS 5    // D0

#endif

// TFT screen
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCK, TFT_RST, TFT_MISO);

/////////////////////////////////////////////////////////////////////////////////////////////
//                                      User function                                      //
/////////////////////////////////////////////////////////////////////////////////////////////
#pragma region "User function"

/*
/// @brief Create draw line function for a canvas. The TFT drawLine function make an overflow for the ESP8266 because of yield function.
/// @param canvas Target convas to write the line
/// @param x0 Start position of line on X axis
/// @param y0 Start position of line on Y axis 
/// @param x1 End position of line on X axis
/// @param y1 End position of line on Y axis
/// @param color Color of the line
void drawLine(GFXcanvas1 &canvas, int x0, int y0, int x1, int y1, uint16_t color)
{ 
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
    while (true)
    {
        // Dessiner le pixel courant
        canvas.drawPixel(x0, y0, color);

        // Vérifier si on est arrivé au point final
        if (x0 == x1 && y0 == y1)
        {
            break;
        }

        // Calculer l'erreur pour le prochain pixel
        e2 = 2 * err;

        // Corriger l'erreur si nécessaire et avancer d'un pixel
        if (e2 > -dy)
        {
            err -= dy;
            x0 += sx;
        }
        if (e2 < dx)
        {
            err += dx;
            y0 += sy;
        }
    }
}
*/

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

    /* Gauge computation
    //                     pos_gauge (filling)
    //                      /
    //          (       |  |  |              )
    //          /      /       \             \
    //         0    pos_low    pos_high      length    ->  [pixel]
    */

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


/// @brief Draw the power gauge.
void Draw_PowerGauge()
{
    // Draw power gauge
    DrawGauge(GAUGE_POWER_WIDTH, GAUGE_POWER_HEIGHT, GAUGE_POWER_LEFT, GAUGE_POWER_TOP, _ConsoPower, _Tol, ILI9341_MAROON, ILI9341_YELLOW);
}


/// @brief Draw the temperature gauge.
void Draw_TempGauge()
{
    // Test horizontal power gauge
    DrawGauge(GAUGE_TEMP_WIDTH, GAUGE_TEMP_HEIGHT, GAUGE_TEMP_LEFT, GAUGE_TEMP_TOP, _TempNeeded, _Tol, ILI9341_BLUE, ILI9341_RED);
}



int _graphInd;
int _graphMax;
int _graphOld = 0;

/// @brief Draw a new random line in the next segment of graph
void Draw_Graph()
{
    // Canvas of power gauge
    GFXcanvas1 canvas(GRAPH_STEP, GRAPH_HEIGHT);

    // Get a random value in range [%]
    int val = (rand() % (_Tol * 2 + 1));
    val = _ConsoPower + val - _Tol;

#ifdef LOG
    Serial.printf("Power is %i%% and the tolerance is ±%i%%. The random value is %i%%\n", _ConsoPower, _Tol, val);
#endif

    // Define trace point
    int y0 = map(_graphOld, 0, 100, GRAPH_HEIGHT, 0);
    int y1 = map(val, 0, 100, GRAPH_HEIGHT, 0);
    int x0 = 0;
    int x1 = GRAPH_STEP - 1;

    // Draw arrow if we have place
    if (_graphInd < GRAPH_WIDTH - GRAPH_STEP)
    {
        // Draw triangle to the end position
        canvas.drawLine(0, y1, 3, y1 + 3, 0xFFFF);
        canvas.drawLine(0, y1, 3, y1 - 3, 0xFFFF);
        canvas.drawLine(3, y1 - 3, 3, y1 + 3, 0xFFFF);
        //drawLine(canvas, 0, y1, 3, y1 + 3, 0xFFFF);
        //drawLine(canvas, 0, y1, 3, y1 - 3, 0xFFFF);
        //drawLine(canvas, 3, y1 - 3, 3, y1 + 3, 0xFFFF);

        // Put in next emplacement the triangle
        tft.drawBitmap(GRAPH_LEFT + _graphInd + GRAPH_STEP, GRAPH_TOP, canvas.getBuffer(), canvas.width(), canvas.height(), ILI9341_RED, 0x0000);

        // Undraw triangle to the end position
        canvas.drawLine(0, y1, 3, y1 + 3, 0x0000);
        canvas.drawLine(0, y1, 3, y1 - 3, 0x0000);
        canvas.drawLine(3, y1 - 3, 3, y1 + 3, 0x0000);
        //drawLine(canvas, 0, y1, 3, y1 + 3, 0x0000);
        //drawLine(canvas, 0, y1, 3, y1 - 3, 0x0000);
        //drawLine(canvas, 3, y1 - 3, 3, y1 + 3, 0x0000);
    }

#ifdef LOG
    Serial.printf("Line go from (%i;%i) to (%i;%i)\n", x0, y0, x1, y1);
#endif

    ////////////////////// Normal line  -> STACK OVERFLOW
    // tft.drawLine(0, startY, GRAPH_STEP-1, endY, 0xFFFF);

    // Draw the line to the new random position
    canvas.drawLine(x0, y0, x1, y1, 0xFFFF);
    //drawLine(canvas, x0, y0, x1, y1, 0xFFFF);

    // Draw de canvas
    tft.drawBitmap(GRAPH_LEFT + _graphInd, GRAPH_TOP, canvas.getBuffer(), canvas.width(), canvas.height(), 0xFFFF, 0x0000);

    // Update graph position
    _graphOld = val;
    _graphInd += GRAPH_STEP;

    // Overflow control
    if (_graphInd >= GRAPH_WIDTH)
        _graphInd = 0;
}

/// @brief Call to draw the new graph segment
void Draw_Update()
{
    Draw_Graph();
}

#pragma endregion


/////////////////////////////////////////////////////////////////////////////////////////////
//                                     Setup and reset                                     //
/////////////////////////////////////////////////////////////////////////////////////////////

/// @brief Setup function for the module
void MySetup()
{
    // Start TFT
    tft.begin();        // start the screen
    tft.setRotation(1); // landscreen, connexion up



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
    tft.drawRect(GRAPH_LEFT - 1, GRAPH_TOP - 1, GRAPH_WIDTH + 2, GRAPH_HEIGHT + 2, ILI9341_WHITE);
    tft.drawRect(GRAPH_LEFT - 2, GRAPH_TOP - 2, GRAPH_WIDTH + 4, GRAPH_HEIGHT + 4, ILI9341_WHITE);

    // And the graphique
    // Draw_Graph();

#ifdef STANDALONE
    // Trigger timer every second in standalone
    if (!Time_Update.active())
        Time_Update.attach(1, Draw_Update);
#endif
}

/////////////////////////////////  Write here the loop code  /////////////////////////////////

/// @brief Analog management
class Analog
{
private:
    uint8_t _pin;
    uint16_t _in;

    //////////////////////////  Average datas  //////////////////////////
    bool _avgUsed = false;
    float _avgSum;
    float _avgNb = 0;
    float _avgOut = 0;

    /// @brief Define average function
    /// @param average Average number
    void _SetAvg(uint8_t average)
    {
        // Init avg info
        _avgUsed = true;
        _avgNb = static_cast<float>(average);
    }

    //////////////////////////  Mapping datas  //////////////////////////
    bool _mapUsed = false;
    uint16_t _mapInMin = 0;
    uint16_t _mapInMax = 0;
    uint16_t _mapOutMin = 0;
    uint16_t _mapOutMax = 0;

    /// @brief Define map function
    /// @param in_Min 
    /// @param in_Max 
    /// @param out_Min 
    /// @param out_Max 
    void _SetMap(uint16_t in_Min, uint16_t in_Max, uint16_t out_Min, uint16_t out_Max)
    {
        // Init avg info
        _mapUsed = true;
        _mapInMin = in_Min;
        _mapInMax = in_Max;
        _mapOutMin = out_Min;
        _mapOutMax = out_Max;
    }

    //////////////////////////  Update datas  //////////////////////////
    bool _updateUsed = false;
    /// @brief Function to call when a text is received
    void (*ReceiveFuncPtr)();
    
public:

    /// @brief Read analog value from an input
    /// @return 
    uint16_t Read()
    {
        // Get the analog value
        _in = analogRead(_pin);

        // Make an averge of the value
        if (_avgUsed)
        {
            // Make an average
            _avgSum += static_cast<float>(_in);
            _avgSum -= _avgOut;
            _avgOut = _avgSum / _avgNb;

            // Update the value
            _in = static_cast<uint16_t>(_avgOut);
        }

        // Map the value to an ouput if needed
        if (_mapUsed)
        {
            _in = map(_in, _mapInMin, _mapInMax, _mapOutMin, _mapOutMax);
        }

        // Check if an update is needed

        return _in;
    }

    Analog(uint8_t pin)
    {
        _pin = pin;
    }

    Analog(uint8_t pin, uint8_t average)
    {
        _pin = pin;

        // Init avg info
        _SetAvg(average);
    }
    
    Analog(uint8_t pin, uint8_t average, uint16_t in_Min, uint16_t in_Max, uint16_t out_Min, uint16_t out_Max )
    {
        _pin = pin;

        // Init avg info
        _SetAvg(average);

        // Init map
        _SetMap(in_Min, in_Max, out_Min, out_Max);
    }

};


Analog Power(ANALOG_POWER);
Analog Temp(ANALOG_POWER, 50, 0, 8191, 0,100);

/// @brief Call at the end of the main loop function
void MyLoop()
{
    int ValPower = Power.Read(); //analogRead(ANALOG_POWER);
    int ValTemp = Temp.Read(); //analogRead(ANALOG_TEMP);
    
#ifdef LOG
    Serial.printf("Power is %4i and temperature is %4i.\n", ValPower, ValTemp);
#endif
}

/////////////////////////////////////////////////////////////////////////////////////////////
//                                      Communication                                      //
/////////////////////////////////////////////////////////////////////////////////////////////
#pragma region "Communication"

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

    // The get the received code, use the function GetCode()
    if (comm.GetCode() == "NXT")
    {
        Draw_Graph();
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
#pragma endregion

#endif