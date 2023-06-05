#include <common.h>
#include <Arduino.h>

// Special library
#include <errno.h>
#include <vector>

/// @brief Convert chars to int
/// @param p Text to convert
/// @return Value in integer
int CharToInt(char *p)
{
     int k = 0;
     while (*p)
     {
          k = (k << 3) + (k << 1) + (*p) - '0';
          p++;
     }
     return k;
}

/// @brief Convert chars to float
/// @param p Text to convert
/// @return Value in float
float CharToFloat(char *p)
{
     char *endptr;
     float result = strtof(p, &endptr);

     if (errno == ERANGE)
     {
          // la valeur convertie est hors des limites de la plage de float
          printf("La valeur convertie est hors des limites de la plage de float\n");
          return 0.0;
     }
     if (p == endptr)
     {
          // aucune conversion n'a été effectuée
          printf("Aucune conversion n'a été effectuée, la chaine est incorrecte\n");
          return 0.0;
     }
     return result;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//                                      Communication                                      //
/////////////////////////////////////////////////////////////////////////////////////////////

/// @brief Create a componant to manage the communication
MyComm::MyComm()
{
     // Clear buffers
     str_in.clear();
     str_out.clear();
}

////////////////////////////   Server   ////////////////////////////

/// @brief Communication with master is started
void MyComm::Started()
{
     _connected = true;
}

/// @brief Communciation with application is running
/// @return The link with unity is done or not
bool MyComm::IsRunning()
{
     return _connected;
}

///////////////////////////   Function   ///////////////////////////

/// @brief Define witch function must be called when a message is received
/// @param func Selected function
void MyComm::setReceiveFunction(void (*func)())
{
     // Save received function address
     ReceiveFuncPtr = func;
}

/// @brief Define witch function must be called when a message is send in standalone. It will execute the simulation received
/// @param func Selected function
void MyComm::setSendFunction(void (*func)())
{
     // Save received function address
     SendFuncPtr = func;
}

/// @brief Call module receive function
void MyComm::callReceiveFunction()
{
     // Serial.println("Function called!");

     // If function is defined, call it
     if (ReceiveFuncPtr != nullptr)
     {
          (*ReceiveFuncPtr)();
     }
}

/////////////////////////////   Send   /////////////////////////////

void MyComm::start()
{
     // Start buffer with emtpy string
     str_out.clear();
}

void MyComm::start(char c)
{
     // Start buffer with a char
     str_out = c;
}

void MyComm::start(String s)
{
     // Start buffer with a string
     str_out = s;
}

void MyComm::add(char c)
{
     // Add char to buffer
     str_out.concat(c);
}

void MyComm::add(const char *c)
{
     // Add array of char to buffer
     str_out.concat(c);
}

void MyComm::add(String s)
{
     // Add string of to buffer
     str_out.concat(s);
}

void MyComm::add(int s)
{
     // Add string of to buffer
     str_out.concat(String(s));
}

void MyComm::add(float s)
{
     // Add string of to buffer
     str_out.concat(String(s));
}

void MyComm::send(String s)
{
     // Concat the input string
     str_out.concat(s);

     // Call send function
     send();
}

void MyComm::send()
{

#ifdef LOG
     // Log send text
     Serial.println("");
     Serial.println("Function send :");
     Serial.println(str_out);
#endif

     // If connected, send to UDP
     if (_connected)
     {
          // Call send to server function
          _SendServer();
     }
     // Else, send to the module close loop function
     else
     {
          // Test if the message is valide
          if (_Receive(str_out, false))
          {
               // If function is defined, call it
               if (SendFuncPtr != nullptr)
               {
                    (*SendFuncPtr)();
               }
          }
     }
     str_out.clear();
}

void MyComm::sendForced(String s)
{
     // Concat the input string
     str_out.concat(s);

     // Call send to server function
     sendForced();
}

void MyComm::sendForced()
{
#ifdef LOG
     // Log send text
     Serial.println("");
     Serial.println("Function forced send :");
     Serial.println(str_out);
#endif

     // Call send to server function
     _SendServer();
}

void MyComm::_SendServer()
{
#ifndef STANDALONE

// Convert string to char*
#ifdef ESP8266

     // Concert string to char
     char *output = new char[str_out.length() + 1];
     str_out.toCharArray(output, str_out.length() + 1);

     // Send to server with UDP
     UDP.beginPacket(Dest, 8888);
     UDP.write(output);
     UDP.endPacket();

#elif defined(ESP32)

     // Convert string to uint8
     uint8_t *output = new uint8_t[str_out.length() + 1]; // Allocation d'un tableau d'octets de taille dynamique
     str_out.getBytes(output, str_out.length() + 1);

     UDP.beginPacket(Dest, 8888);
     UDP.write(output, str_out.length());
     UDP.endPacket();
#endif

#endif
}

///////////////////////////   Received   ///////////////////////////

/// @brief Call when a message is received.
/// @param msg Received message
/// @return The message is valid
bool MyComm::Receive(String msg)
{
     return _Receive(msg, true);
}

/// @brief Call when a message is received.
/// @param msg Received message
/// @return The message is valid
bool MyComm::ReceiveIntern(String msg)
{
     return _Receive(msg, false);
}

bool MyComm::_Receive(String msg, bool test)
{

     Serial.println("Message reçu : " + msg);

     // Clear old array
     str_in.clear();

     // Create variable
     int start = 0;
     int end = msg.indexOf(';');

     // For each ; find
     while (end >= 0)
     {
          // Add sub string in array
          str_in.push_back(msg.substring(start, end));

          // Define new limits
          start = end + 1;
          end = msg.indexOf(';', start);
     }

     // Add the last part in array
     str_in.push_back(msg.substring(start)); // Ajoute la dernière sous-chaîne au tableau

     // Exit if only one string
     if (str_in.size() <= 1)
     {

          Serial.println("Message reçu : Error - Only one string");
          return false;
     }

     // Exit if the message is not for us
     if (test)
     {
          // Check if the first parameter is '*' or 'Y'
          if (str_in[0] == "Y" || str_in[0] == "*")
          {
               // And delete from parameter
               str_in.erase(str_in.begin());
          }
          else
          {
               return false;
          }
     }

     // Save code
     _code = str_in[0];

     // And delete from parameter
     str_in.erase(str_in.begin());

     return true;
}

String MyComm::GetCode()
{
     return _code;
}

String MyComm::GetParameter(unsigned int i)
{
     if (i > 0 && i <= str_in.size())
     {
          return str_in[i - 1];
     }
     return "-"; // Si l'index est invalide, retourne -1
}

char *MyComm::GetParameterInChar(unsigned int i)
{
     if (i > 0 && i <= str_in.size())
     {
          String s = str_in[i - 1];
          char *output = new char[s.length() + 1];
          s.toCharArray(output, s.length() + 1);
          return output;
     }
     else
     {
          char *output = new char[2];
          output[0] = '-';
          output[1] = '\0';
          return output;
     }
}

int MyComm::GetSize()
{
     return str_in.size();
}

void MyComm::Info_Received()
{
#ifdef LOG

     // Get current code
     Serial.println("Received code is : '" + GetCode() + "'");

     // For every parameter
     for (int i = 1; i <= GetSize(); i++)
     {
          Serial.print("Parameter ");
          Serial.print(i);
          Serial.print(": ");
          Serial.println(GetParameter(i));
     }
#endif
}

/////////////////////////////////////////////////////////////////////////////////////////////
//                                          Analog                                         //
/////////////////////////////////////////////////////////////////////////////////////////////

/// @brief Define average function
/// @param average Average number
void Analog::_SetAvg(uint8_t average)
{
     // Init avg info
     _avgUsed = true;
     _avgNb = static_cast<float>(average);
}

/// @brief Define map function
/// @param in_Min
/// @param in_Max
/// @param out_Min
/// @param out_Max
void Analog::_SetMap(uint16_t in_Min, uint16_t in_Max, uint16_t out_Min, uint16_t out_Max)
{
     // Init avg info
     _mapUsed = true;
     _mapInMin = in_Min;
     _mapInMax = in_Max;
     _mapOutMin = out_Min;
     _mapOutMax = out_Max;
}

/// @brief Read analog value from an input
/// @return Current analog value
uint16_t Analog::Read()
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

/// @brief Define analog input
/// @param pin Input pin
Analog::Analog(uint8_t pin)
{
     _pin = pin;
}

/// @brief Define analog input with an average
/// @param pin Input pin
/// @param average Average of the analog input
Analog::Analog(uint8_t pin, uint8_t average)
{
     _pin = pin;

     // Init avg info
     _SetAvg(average);
}

/// @brief Define analog input with a mapping
/// @param pin Input pin
/// @param average Average of the analog input
/// @param in_Min Minimum value of the analog input
/// @param in_Max Maximum value of the analog input
/// @param out_Min Minimum of the user value (mapping)
/// @param out_Max Maximum of the user value (mapping)
Analog::Analog(uint8_t pin, uint8_t average, uint16_t in_Min, uint16_t in_Max, uint16_t out_Min, uint16_t out_Max)
{
     _pin = pin;

     // Init avg info
     _SetAvg(average);

     // Init map
     _SetMap(in_Min, in_Max, out_Min, out_Max);
}
