#ifndef COMMON_FILE_H
#define COMMON_FILE_H

#include <Arduino.h>
#include <string>
#include <vector>
#include <WiFiUDP.h>
#include <FastLED.h>

// Enabled all serial communication (put in comment to disabled log mode)
#define LOG

// Work without the server (put in comment to work with the server)
#define STANDALONE

// Define witch module is used (ONLY ONE)
//#define MODEL
//#define BREACH
#define DIALOGUE
//#define ELECTRICITY
//#define ENERGY
//#define ENGINE
//#define EXTINGUISHER
//#define FIRE 
//#define NAVIGATION
//#define RADAR
//#define REACTOR
//#define SHIELD
//#define TORPEDO


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
class MyPixels
{
private:

    /// @brief Array of leds 
    CRGB* _leds;

    /// @brief Number of leds
    int _numLEDs;

    /// @brief Output to control the leds (default = 0)
    int _output;

    /// @brief If the led is one
    bool _asInfo = false;

    /// @brief An update is requered
    bool _updateRequest;
    
public:
    /// @brief Create a pixels manager object
    MyPixels();

    /// @brief Use the first pixel as status info
    void useInfoPixel();

    /// @brief Add a number of led to control
    /// @param number Number of led in the project
    void addLeds(int number);

    /// @brief Initalize the leds controler. Do not used, already done in master
    void initalize();

    void show();

    void update();
    
    void setPixelColor(int index, const CRGB& newColor);




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
    Analog(uint8_t pin, uint8_t average, uint16_t in_Min, uint16_t in_Max, uint16_t out_Min, uint16_t out_Max );

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