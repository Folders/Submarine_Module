#ifndef COMMON_FILE_H
#define COMMON_FILE_H

#include <Arduino.h>
#include <string>
#include <vector>
#include <WiFiUDP.h>
#include <LiquidTWI2.h>
// #include <FastLED.h>
#include <Adafruit_NeoPixel.h>

// Enabled all serial communication (put in comment to disabled log mode)
#define LOG

// Work without the server (put in comment to work with the server)
// #define STANDALONE

// Define witch module is used (ONLY ONE)
// #define MODEL
// #define BREACH
// #define DIALOGUE
#define ELECTRICITY
// #define ENERGY
// #define ENGINE
// #define EXTINGUISHER
// #define FIRE
// #define NAVIGATION
// #define RADAR
// #define REACTOR
// #define SHIELD
// #define TORPEDO
// #define TEST

// Define module number if more then one is used
#define NUMBER 3

// Define module name for application
#if defined(MODEL)
#define TYPE "---"
#elif defined(BREACH)
#define TYPE ("BRE_" + std::to_string(NUMBER)).c_str()
#elif defined(ELECTRICITY)
#define TYPE "PWR"
#elif defined(ENERGY)
#define TYPE "NRJ;*"
#elif defined(ENGINE)
#define TYPE "ENG"
#elif defined(EXTINGUISHER)
#define TYPE "EXT;*"
#elif defined(FIRE)
#define TYPE "FIR_" + std::to_string(NUMBER)).c_str()
#elif defined(NAVIGATION)
#define TYPE "NAV"
#elif defined(RADAR)
#define TYPE "RDR"
#elif defined(REACTOR)
#define TYPE "REA"
#elif defined(SHIELD)
#define TYPE "SHI"
#elif defined(TORPEDO)
#define TYPE "TRP"
#elif defined(DIALOGUE)
#define TYPE "DLG"
#elif defined(TEST)
#define TYPE "TST"
#else
#error "No module selected!"
#endif

///////////////////////////////////////////////////////////////////////////////////////////////
//                                          Function                                         //
///////////////////////////////////////////////////////////////////////////////////////////////

/// @brief Convert chars to int
/// @param p Text to convert
/// @return Value in integer
int CharToInt(char *p);

/// @brief Convert chars to float
/// @param p Text to convert
/// @return Value in float
float CharToFloat(char *p);

///////////////////////////////////////////////////////////////////////////////////////////////
//                                            Class                                          //
///////////////////////////////////////////////////////////////////////////////////////////////

/// @brief Manager of communication
class MyComm
{
private:
    /// @brief Function to call when a text is received
    void (*ReceiveFuncPtr)();

    /// @brief Function to call when a text is send (in standalone)
    void (*SendFuncPtr)();

    /// @brief Buffer of text to send
    String str_out; // Le texte enregistré dans le tampon

    /// @brief Array of datas received
    std::vector<String> str_in;

    /// @brief Code of last received datas
    String _code;

    /// @brief If module is connected to the master
    bool _connected;

    /// @brief Call when a message is received.
    /// @param msg Received message
    /// @param test Need to test the source of the message
    /// @return The message is valid
    bool _Receive(String msg, bool test);

    /// @brief Call to send datas to server
    void _SendServer();

public:
    /// @brief Create a componant to manage the communication
    MyComm();

    ///////////////////////////////////////   Server   ///////////////////////////////////////

    /// @brief Communication with master is started
    void Started();

    /// @brief Communciation with application is running
    /// @return The link with unity is done or not
    bool IsRunning();

    //////////////////////////////////////   Function   //////////////////////////////////////

    /// @brief Define witch function must be called when a message is received
    /// @param func Received function
    void setReceiveFunction(void (*func)());

    /// @brief Define witch function must be called when a message is send in standalone. It will execute the simulation received
    /// @param func ServerSimulation function
    void setSendFunction(void (*func)());

    /// @brief Call module receive function
    void callReceiveFunction();

    ////////////////////////////////////////   Send   ////////////////////////////////////////

    /// @brief Start text buffer to send datas to server
    void start();

    /// @brief Start text buffer to send datas to server
    /// @param c First part of the text
    void start(char c);

    /// @brief Start text buffer to send datas to server
    /// @param s First part of the text
    void start(String s);

    /// @brief Add text to add in buffer opened with "start"
    /// @param c Char to add
    void add(char c);

    /// @brief Add text to add in buffer opened with "start"
    /// @param c Char to add
    void add(const char *c);

    /// @brief Add text to add in buffer opened with "start"
    /// @param s String to add
    void add(String s);

    /// @brief Add integer to add in buffer opened with "start"
    /// @param s Integer to add
    void add(int s);

    /// @brief Add float to add in buffer opened with "start"
    /// @param s Float to add
    void add(float s);

    /// @brief Send to the server the datas in the buffer opened with "start"
    void send();

    /// @brief Send to the server the datas in string OR concate the datas to the buffer opened with "start"
    /// @param s String to send / concatenate with buffer
    void send(String s);

    /// @brief Force to send the datas to server, and not the to standalone function
    void sendForced();

    /// @brief Force to send the datas to server, and not the to standalone function
    /// @param s String to send / concatenate with buffer
    void sendForced(String s);

    //////////////////////////////////////   Received   //////////////////////////////////////

    /// @brief Call when a message is received.
    /// @param msg Received message
    /// @return The message is valid
    bool Receive(String msg);

    /// @brief Call when a message is received.
    /// @param msg Received message
    /// @return The message is valid
    bool ReceiveIntern(String msg);

    /// @brief Get the current received code (COD)
    /// @return Current code of tree letters
    String GetCode();

    /// @brief Get a specific parameter.
    /// @param i Index of parameter to read. Must be between 1 and the max (you can get the size with GetSize)
    /// @return Value of selected parameter
    String GetParameter(unsigned int i);

    /// @brief Get a specific parameter.
    /// @param i Index of parameter to read. Must be between 1 and the max (you can get the size with GetSize)
    /// @return Value of selected parameter
    char *GetParameterInChar(unsigned int i);

    /// @brief Get the number of parameter
    /// @return Number of parameter
    int GetSize();

    /// @brief Send to serial datas received
    void Info_Received();
};

/// @brief Pixel management
class Pixel
{
private:
    /// @brief If the led is one
    bool _on = true;

    /// @brief Current brigntness of the pixel, 0 is disabled, 1 is min and 255 is max.
    uint8_t _brightness = 0;



public:
    /// @brief Create a pixel object
    Pixel();

    /// @brief Create a pixel object with color parameters
    /// @param r Red componant of pixel, between 0..255
    /// @param g Green componant of pixel, between 0..255
    /// @param b Blue componant of pixel, between 0..255
    Pixel(uint8_t r, uint8_t g, uint8_t b);

    /// @brief Create a pixel object with color parameters
    /// @param r Red componant of pixel, between 0..255
    /// @param g Green componant of pixel, between 0..255
    /// @param b Blue componant of pixel, between 0..255
    /// @param brightness 0= Full ligne (disabled), 1= lower light, 255 = almost max.
    Pixel(uint8_t r, uint8_t g, uint8_t b, uint8_t brightness);

    /// @brief Create a pixel object with one packed color parameter
    /// @param color Set a pixel's color using a 32-bit 'packed' RGB (Ex: 0xAAFF88)
    Pixel(uint32_t color);

    /// @brief Red componant of pixel, between 0..255
    uint8_t Red;

    /// @brief Green componant of pixel, between 0..255
    uint8_t Green;

    /// @brief Blue componant of pixel, between 0..255
    uint8_t Blue;

    /// @brief Set the brightness of the pixel
    /// @param brightness 0= Full ligne (disabled), 1= lower light, 255 = almost max.
    void SetBrightness(uint8_t brightness);

    /// @brief Set the brightness of the pixel
    /// @param brightness 0= off, 1 = max.
    void SetBrightness(float brightness);

    /// @brief Return red color of the pixel, with brightness
    /// @return Current red value (0..255)
    uint8_t GetRed();

    /// @brief Return green color of the pixel, with brightness
    /// @return Current green value (0..255)
    uint8_t GetGreen();

    /// @brief Return blue color of the pixel, with brightness
    /// @return Current blue value (0..255)
    uint8_t GetBlue();

    /// @brief Return packed color of the pixel, can be used directely with NeoPixel "setPixelColor" function.
    /// @return Current value (0xRRGGBB)
    uint32_t GetColor();

    /// @brief Update color of the pixel
    /// @param newColor New pixel color
    void SetColor(Pixel newColor);

    /// @brief Update color of the pixel
    /// @param r Red componant (0..255)
    /// @param g Green componant (0..255)
    /// @param b Blue componant (0..255)
    void SetColor(uint8_t r, uint8_t g, uint8_t b);

    /// @brief Update color of the pixel
    /// @param newColor Set a pixel's color using a 32-bit 'packed' RGB (0xAAFF88)
    void SetColor(uint32_t newColor);

};

/// Representation of an RGB pixel (Red, Green, Blue)
struct CRGB {

    /// Predefined RGB colors
    typedef enum {
        AliceBlue=0xF0F8FF,             ///< @htmlcolorblock{F0F8FF}
        Amethyst=0x9966CC,              ///< @htmlcolorblock{9966CC}
        AntiqueWhite=0xFAEBD7,          ///< @htmlcolorblock{FAEBD7}
        Aqua=0x00FFFF,                  ///< @htmlcolorblock{00FFFF}
        Aquamarine=0x7FFFD4,            ///< @htmlcolorblock{7FFFD4}
        Azure=0xF0FFFF,                 ///< @htmlcolorblock{F0FFFF}
        Beige=0xF5F5DC,                 ///< @htmlcolorblock{F5F5DC}
        Bisque=0xFFE4C4,                ///< @htmlcolorblock{FFE4C4}
        Black=0x000000,                 ///< @htmlcolorblock{000000}
        BlanchedAlmond=0xFFEBCD,        ///< @htmlcolorblock{FFEBCD}
        Blue=0x0000FF,                  ///< @htmlcolorblock{0000FF}
        BlueViolet=0x8A2BE2,            ///< @htmlcolorblock{8A2BE2}
        Brown=0xA52A2A,                 ///< @htmlcolorblock{A52A2A}
        BurlyWood=0xDEB887,             ///< @htmlcolorblock{DEB887}
        CadetBlue=0x5F9EA0,             ///< @htmlcolorblock{5F9EA0}
        Chartreuse=0x7FFF00,            ///< @htmlcolorblock{7FFF00}
        Chocolate=0xD2691E,             ///< @htmlcolorblock{D2691E}
        Coral=0xFF7F50,                 ///< @htmlcolorblock{FF7F50}
        CornflowerBlue=0x6495ED,        ///< @htmlcolorblock{6495ED}
        Cornsilk=0xFFF8DC,              ///< @htmlcolorblock{FFF8DC}
        Crimson=0xDC143C,               ///< @htmlcolorblock{DC143C}
        Cyan=0x00FFFF,                  ///< @htmlcolorblock{00FFFF}
        DarkBlue=0x00008B,              ///< @htmlcolorblock{00008B}
        DarkCyan=0x008B8B,              ///< @htmlcolorblock{008B8B}
        DarkGoldenrod=0xB8860B,         ///< @htmlcolorblock{B8860B}
        DarkGray=0xA9A9A9,              ///< @htmlcolorblock{A9A9A9}
        DarkGrey=0xA9A9A9,              ///< @htmlcolorblock{A9A9A9}
        DarkGreen=0x006400,             ///< @htmlcolorblock{006400}
        DarkKhaki=0xBDB76B,             ///< @htmlcolorblock{BDB76B}
        DarkMagenta=0x8B008B,           ///< @htmlcolorblock{8B008B}
        DarkOliveGreen=0x556B2F,        ///< @htmlcolorblock{556B2F}
        DarkOrange=0xFF8C00,            ///< @htmlcolorblock{FF8C00}
        DarkOrchid=0x9932CC,            ///< @htmlcolorblock{9932CC}
        DarkRed=0x8B0000,               ///< @htmlcolorblock{8B0000}
        DarkSalmon=0xE9967A,            ///< @htmlcolorblock{E9967A}
        DarkSeaGreen=0x8FBC8F,          ///< @htmlcolorblock{8FBC8F}
        DarkSlateBlue=0x483D8B,         ///< @htmlcolorblock{483D8B}
        DarkSlateGray=0x2F4F4F,         ///< @htmlcolorblock{2F4F4F}
        DarkSlateGrey=0x2F4F4F,         ///< @htmlcolorblock{2F4F4F}
        DarkTurquoise=0x00CED1,         ///< @htmlcolorblock{00CED1}
        DarkViolet=0x9400D3,            ///< @htmlcolorblock{9400D3}
        DeepPink=0xFF1493,              ///< @htmlcolorblock{FF1493}
        DeepSkyBlue=0x00BFFF,           ///< @htmlcolorblock{00BFFF}
        DimGray=0x696969,               ///< @htmlcolorblock{696969}
        DimGrey=0x696969,               ///< @htmlcolorblock{696969}
        DodgerBlue=0x1E90FF,            ///< @htmlcolorblock{1E90FF}
        FireBrick=0xB22222,             ///< @htmlcolorblock{B22222}
        FloralWhite=0xFFFAF0,           ///< @htmlcolorblock{FFFAF0}
        ForestGreen=0x228B22,           ///< @htmlcolorblock{228B22}
        Fuchsia=0xFF00FF,               ///< @htmlcolorblock{FF00FF}
        Gainsboro=0xDCDCDC,             ///< @htmlcolorblock{DCDCDC}
        GhostWhite=0xF8F8FF,            ///< @htmlcolorblock{F8F8FF}
        Gold=0xFFD700,                  ///< @htmlcolorblock{FFD700}
        Goldenrod=0xDAA520,             ///< @htmlcolorblock{DAA520}
        Gray=0x808080,                  ///< @htmlcolorblock{808080}
        Grey=0x808080,                  ///< @htmlcolorblock{808080}
        Green=0x008000,                 ///< @htmlcolorblock{008000}
        GreenYellow=0xADFF2F,           ///< @htmlcolorblock{ADFF2F}
        Honeydew=0xF0FFF0,              ///< @htmlcolorblock{F0FFF0}
        HotPink=0xFF69B4,               ///< @htmlcolorblock{FF69B4}
        IndianRed=0xCD5C5C,             ///< @htmlcolorblock{CD5C5C}
        Indigo=0x4B0082,                ///< @htmlcolorblock{4B0082}
        Ivory=0xFFFFF0,                 ///< @htmlcolorblock{FFFFF0}
        Khaki=0xF0E68C,                 ///< @htmlcolorblock{F0E68C}
        Lavender=0xE6E6FA,              ///< @htmlcolorblock{E6E6FA}
        LavenderBlush=0xFFF0F5,         ///< @htmlcolorblock{FFF0F5}
        LawnGreen=0x7CFC00,             ///< @htmlcolorblock{7CFC00}
        LemonChiffon=0xFFFACD,          ///< @htmlcolorblock{FFFACD}
        LightBlue=0xADD8E6,             ///< @htmlcolorblock{ADD8E6}
        LightCoral=0xF08080,            ///< @htmlcolorblock{F08080}
        LightCyan=0xE0FFFF,             ///< @htmlcolorblock{E0FFFF}
        LightGoldenrodYellow=0xFAFAD2,  ///< @htmlcolorblock{FAFAD2}
        LightGreen=0x90EE90,            ///< @htmlcolorblock{90EE90}
        LightGrey=0xD3D3D3,             ///< @htmlcolorblock{D3D3D3}
        LightPink=0xFFB6C1,             ///< @htmlcolorblock{FFB6C1}
        LightSalmon=0xFFA07A,           ///< @htmlcolorblock{FFA07A}
        LightSeaGreen=0x20B2AA,         ///< @htmlcolorblock{20B2AA}
        LightSkyBlue=0x87CEFA,          ///< @htmlcolorblock{87CEFA}
        LightSlateGray=0x778899,        ///< @htmlcolorblock{778899}
        LightSlateGrey=0x778899,        ///< @htmlcolorblock{778899}
        LightSteelBlue=0xB0C4DE,        ///< @htmlcolorblock{B0C4DE}
        LightYellow=0xFFFFE0,           ///< @htmlcolorblock{FFFFE0}
        Lime=0x00FF00,                  ///< @htmlcolorblock{00FF00}
        LimeGreen=0x32CD32,             ///< @htmlcolorblock{32CD32}
        Linen=0xFAF0E6,                 ///< @htmlcolorblock{FAF0E6}
        Magenta=0xFF00FF,               ///< @htmlcolorblock{FF00FF}
        Maroon=0x800000,                ///< @htmlcolorblock{800000}
        MediumAquamarine=0x66CDAA,      ///< @htmlcolorblock{66CDAA}
        MediumBlue=0x0000CD,            ///< @htmlcolorblock{0000CD}
        MediumOrchid=0xBA55D3,          ///< @htmlcolorblock{BA55D3}
        MediumPurple=0x9370DB,          ///< @htmlcolorblock{9370DB}
        MediumSeaGreen=0x3CB371,        ///< @htmlcolorblock{3CB371}
        MediumSlateBlue=0x7B68EE,       ///< @htmlcolorblock{7B68EE}
        MediumSpringGreen=0x00FA9A,     ///< @htmlcolorblock{00FA9A}
        MediumTurquoise=0x48D1CC,       ///< @htmlcolorblock{48D1CC}
        MediumVioletRed=0xC71585,       ///< @htmlcolorblock{C71585}
        MidnightBlue=0x191970,          ///< @htmlcolorblock{191970}
        MintCream=0xF5FFFA,             ///< @htmlcolorblock{F5FFFA}
        MistyRose=0xFFE4E1,             ///< @htmlcolorblock{FFE4E1}
        Moccasin=0xFFE4B5,              ///< @htmlcolorblock{FFE4B5}
        NavajoWhite=0xFFDEAD,           ///< @htmlcolorblock{FFDEAD}
        Navy=0x000080,                  ///< @htmlcolorblock{000080}
        OldLace=0xFDF5E6,               ///< @htmlcolorblock{FDF5E6}
        Olive=0x808000,                 ///< @htmlcolorblock{808000}
        OliveDrab=0x6B8E23,             ///< @htmlcolorblock{6B8E23}
        Orange=0xFFA500,                ///< @htmlcolorblock{FFA500}
        OrangeRed=0xFF4500,             ///< @htmlcolorblock{FF4500}
        Orchid=0xDA70D6,                ///< @htmlcolorblock{DA70D6}
        PaleGoldenrod=0xEEE8AA,         ///< @htmlcolorblock{EEE8AA}
        PaleGreen=0x98FB98,             ///< @htmlcolorblock{98FB98}
        PaleTurquoise=0xAFEEEE,         ///< @htmlcolorblock{AFEEEE}
        PaleVioletRed=0xDB7093,         ///< @htmlcolorblock{DB7093}
        PapayaWhip=0xFFEFD5,            ///< @htmlcolorblock{FFEFD5}
        PeachPuff=0xFFDAB9,             ///< @htmlcolorblock{FFDAB9}
        Peru=0xCD853F,                  ///< @htmlcolorblock{CD853F}
        Pink=0xFFC0CB,                  ///< @htmlcolorblock{FFC0CB}
        Plaid=0xCC5533,                 ///< @htmlcolorblock{CC5533}
        Plum=0xDDA0DD,                  ///< @htmlcolorblock{DDA0DD}
        PowderBlue=0xB0E0E6,            ///< @htmlcolorblock{B0E0E6}
        Purple=0x800080,                ///< @htmlcolorblock{800080}
        Red=0xFF0000,                   ///< @htmlcolorblock{FF0000}
        RosyBrown=0xBC8F8F,             ///< @htmlcolorblock{BC8F8F}
        RoyalBlue=0x4169E1,             ///< @htmlcolorblock{4169E1}
        SaddleBrown=0x8B4513,           ///< @htmlcolorblock{8B4513}
        Salmon=0xFA8072,                ///< @htmlcolorblock{FA8072}
        SandyBrown=0xF4A460,            ///< @htmlcolorblock{F4A460}
        SeaGreen=0x2E8B57,              ///< @htmlcolorblock{2E8B57}
        Seashell=0xFFF5EE,              ///< @htmlcolorblock{FFF5EE}
        Sienna=0xA0522D,                ///< @htmlcolorblock{A0522D}
        Silver=0xC0C0C0,                ///< @htmlcolorblock{C0C0C0}
        SkyBlue=0x87CEEB,               ///< @htmlcolorblock{87CEEB}
        SlateBlue=0x6A5ACD,             ///< @htmlcolorblock{6A5ACD}
        SlateGray=0x708090,             ///< @htmlcolorblock{708090}
        SlateGrey=0x708090,             ///< @htmlcolorblock{708090}
        Snow=0xFFFAFA,                  ///< @htmlcolorblock{FFFAFA}
        SpringGreen=0x00FF7F,           ///< @htmlcolorblock{00FF7F}
        SteelBlue=0x4682B4,             ///< @htmlcolorblock{4682B4}
        Tan=0xD2B48C,                   ///< @htmlcolorblock{D2B48C}
        Teal=0x008080,                  ///< @htmlcolorblock{008080}
        Thistle=0xD8BFD8,               ///< @htmlcolorblock{D8BFD8}
        Tomato=0xFF6347,                ///< @htmlcolorblock{FF6347}
        Turquoise=0x40E0D0,             ///< @htmlcolorblock{40E0D0}
        Violet=0xEE82EE,                ///< @htmlcolorblock{EE82EE}
        Wheat=0xF5DEB3,                 ///< @htmlcolorblock{F5DEB3}
        White=0xFFFFFF,                 ///< @htmlcolorblock{FFFFFF}
        WhiteSmoke=0xF5F5F5,            ///< @htmlcolorblock{F5F5F5}
        Yellow=0xFFFF00,                ///< @htmlcolorblock{FFFF00}
        YellowGreen=0x9ACD32,           ///< @htmlcolorblock{9ACD32}

        // LED RGB color that roughly approximates
        // the color of incandescent fairy lights,
        // assuming that you're using FastLED
        // color correction on your LEDs (recommended).
        FairyLight=0xFFE42D,           ///< @htmlcolorblock{FFE42D}

        // If you are using no color correction, use this
        FairyLightNCC=0xFF9D2A         ///< @htmlcolorblock{FFE42D}

    } HTMLColorCode;
};


class Variator
{
public:
    Variator(int index, const Pixel &colorStart, const Pixel &colorEnd)
        : _index(index), _colorStart(colorStart), _colorEnd(colorEnd) {}

    void update(Adafruit_NeoPixel *leds, float ratio)
    {
        // Assurez-vous que blendIndex est dans la plage valide (0 à 255)
        uint8_t blendIndex = constrain(static_cast<uint8_t>(ratio), 0, 255);

        // Calculez les nouvelles valeurs de couleur en fonction de l'indice de mélange
        uint8_t r = (_colorStart.Red * (255 - blendIndex) + _colorEnd.Red * blendIndex) / 255;
        uint8_t g = (_colorStart.Green * (255 - blendIndex) + _colorEnd.Green * blendIndex) / 255;
        uint8_t b = (_colorStart.Blue * (255 - blendIndex) + _colorEnd.Blue * blendIndex) / 255;

        leds->setPixelColor(_index, r, g, b);
    }

    int const getIndex()
    {
        return _index;
    }

private:
    int _index;
    Pixel _colorStart;
    Pixel _colorEnd;
};

/// @brief Pixel management
class MyPixels
{
private:
    /// @brief Array of leds
    Adafruit_NeoPixel _leds = Adafruit_NeoPixel();

    /// @brief Number of leds
    int _numLEDs;

    /// @brief Output to control the leds (default = 0)
    int _output;

    /// @brief Info led is used
    bool _asInfo = false;

    /// @brief Backup color of info led 
    uint32_t _infoCOlor = 0;

    /// @brief An update is requered
    bool _updateRequest;

    /// @brief Current value of the variaton
    float _ratio;

    /// @brief Step of variaton for a cycle
    float _ratioStep = 1.0;

    bool _ratioDown = false;

    /// @brief Add variator object
    std::vector<Variator> _variators;

    // Private function
    void _show();
    bool _asVariator(int index);

public:
    MyPixels();

    // Info pixel function
    void useInfoPixel();
    void staWaitWifi();
    void staWaitServer();
    void staConnected();
    void staSimulation();

    // Function used in "MySetup" before initalization
    void addLeds(int number);
    void timeVariator(float step);

    // User function for pixel
    void clear();
    void setPixelColor(int index, const Pixel &newColor);
    void setPixelColor(int index, uint8_t r, uint8_t g, uint8_t b);

    // User function for variator
    void addVariator(int index, const Pixel &colorStart, const Pixel &colorEnd);
    void deleteVariator(int index);

    // Fonction used in "main"
    void initalize();
    void update();
};

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
    void _SetAvg(uint8_t average);

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
    void _SetMap(uint16_t in_Min, uint16_t in_Max, uint16_t out_Min, uint16_t out_Max);

public:
    /// @brief Read analog value from an input
    /// @return Current analog value
    uint16_t Read();

    /// @brief Define analog input
    /// @param pin Input pin
    Analog(uint8_t pin);

    /// @brief Define analog input with an average
    /// @param pin Input pin
    /// @param average Average of the analog input
    Analog(uint8_t pin, uint8_t average);

    /// @brief Define analog input with a mapping
    /// @param pin Input pin
    /// @param average Average of the analog input
    /// @param in_Min Minimum value of the analog input
    /// @param in_Max Maximum value of the analog input
    /// @param out_Min Minimum of the user value (mapping)
    /// @param out_Max Maximum of the user value (mapping)
    Analog(uint8_t pin, uint8_t average, uint16_t in_Min, uint16_t in_Max, uint16_t out_Min, uint16_t out_Max);
};

/// @brief Button management
class Button
{
private:
    // Input parameter
    uint8_t _pin;
    LiquidTWI2 *_lcd;

    // Button parameter
    bool _invert;
    bool _isLCD;

    // Button state
    bool _state, _backup;
    bool _up;
    bool _down;

public:
    /// @brief Define button on a digital input
    /// @param pin Input pin
    Button(uint8_t pin);

    /// @brief Define button on a LCD
    /// @param lcd LCD to read input
    /// @param input Input to check
    Button(LiquidTWI2 *lcd, uint8_t input);

    /// @brief Invert reading value
    void invert();

    /// @brief Read the value of the button
    void read();

    /// @brief Get the value of the button
    /// @return Button state
    bool value();

    /// @brief Get if it's a rising edge
    /// @return Rising edge state
    bool up();

    /// @brief Get if it's a falling edge
    /// @return Falling edge state
    bool down();
};

///////////////////////////////////////////////////////////////////////////////////////////////
//                                      Global variable                                      //
///////////////////////////////////////////////////////////////////////////////////////////////

extern MyComm comm;

extern MyPixels pixels;

#ifndef STANDALONE
extern IPAddress Dest;
extern WiFiUDP UDP;
#endif // STANDALONE

/// @brief Send more information to the server
extern bool debug;

#endif // COMMON_FILE_H