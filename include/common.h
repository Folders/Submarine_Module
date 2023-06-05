#ifndef COMMON_FILE_H
#define COMMON_FILE_H

#include <Arduino.h>
#include <string>
#include <vector>
#include <WiFiUDP.h>


// Enabled all serial communication (put in comment to disabled log mode)
#define LOG

// Work without the server (put in comment to work with the server)
#define STANDALONE

// Define witch module is used (ONLY ONE)
//#define MODEL
//#define BREACH
//#define ELECTRICITY
//#define ENERGY
//#define ENGINE
//#define EXTINGUISHER
//#define FIRE 
//#define NAVIGATION
//#define RADAR
#define REACTOR
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
#define TYPE "ELC"
#elif defined(ENERGY)
#define TYPE ("NRJ_" + std::to_string(NUMBER)).c_str()
#elif defined(ENGINE)
#define TYPE "ENG"
#elif defined(EXTINGUISHER)
#define TYPE ("EXT_" + std::to_string(NUMBER)).c_str()
#elif defined(FIRE)
#define TYPE ("FIR_" + std::to_string(NUMBER)).c_str()
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
    String str_out;      // Le texte enregistré dans le tampon

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
    void add(const char* c);
    
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
    char* GetParameterInChar(unsigned int i);

    /// @brief Get the number of parameter
    /// @return Number of parameter
    int GetSize();

    /// @brief Send to serial datas received
    void Info_Received();
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

#ifndef STANDALONE
extern IPAddress Dest;
extern WiFiUDP UDP;
#endif // STANDALONE

/// @brief Send more information to the server
extern bool debug;

#endif // COMMON_FILE_H