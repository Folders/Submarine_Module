#include <common.h>
#include <Arduino.h>

// Special library
#include <errno.h>
#include <vector>
// #include <FastLED.h> // header file
#include <Adafruit_NeoPixel.h>

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

     // Clear buffer
     str_out.clear();
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

/////////////////////////////////////////////////////////////////////////////////////////////
//                                          Pixel                                          //
/////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////   Constructor   ///////////////////////////

/// @brief Create a pixel object
Pixel::Pixel()
{
     this->Red = 0;
     this->Green = 0;
     this->Blue = 0;
}

/// @brief Create a pixel object with color parameters
/// @param r Red componant of pixel, between 0..255
/// @param g Green componant of pixel, between 0..255
/// @param b Blue componant of pixel, between 0..255
Pixel::Pixel(uint8_t r, uint8_t g, uint8_t b)
{
     this->Red = r;
     this->Green = g;
     this->Blue = b;
}

/// @brief Create a pixel object with color parameters
/// @param r Red componant of pixel, between 0..1
/// @param g Green componant of pixel, between 0..1
/// @param b Blue componant of pixel, between 0..1
/// @param brightness 0= Full ligne (disabled), 1= lower light, 255 = almost max.
Pixel::Pixel(uint8_t r, uint8_t g, uint8_t b, uint8_t brightness)
{
     this->Red = r;
     this->Green = g;
     this->Blue = b;
     this->_brightness = brightness;
}

/// @brief Create a pixel object with one packed color parameter
/// @param color Set a pixel's color using a 32-bit 'packed' RGB (Ex: 0xAAFF88)
Pixel::Pixel(uint32_t color)
{
     this->Red = (uint8_t)(color >> 16);
     this->Green = (uint8_t)(color >> 8);
     this->Blue = (uint8_t)(color);
}

///////////////////////////   Brightness   ///////////////////////////

/// @brief Set the brightness of the pixel
/// @param brightness 0= Full ligne (disabled), 1= lower light, 255 = almost max.
void Pixel::SetBrightness(uint8_t brightness)
{
     this->_brightness = brightness;
}

/// @brief Set the brightness of the pixel
/// @param brightness 0= off, 1 = max.
void Pixel::SetBrightness(float brightness)
{
     // If maximum
     if (brightness >= 1.0)
     {
          // Maximum of brightness
          this->_on = true;
          this->_brightness = 0;
     }
     else if (brightness <= 0)
     {
          // Turn off
          this->_on = false;
          this->_brightness = 1;
     }
     else
     {
          // Convert float to int
          this->_on = true;
          this->_brightness = static_cast<uint8_t>(brightness * 254.0) + 1;
     }
}

///////////////////////////   Get color   ///////////////////////////

/// @brief Return current color of the pixel (with brightness)
/// @return Current red value (0..255)
uint8_t Pixel::GetRed()
{
     // If the led is On
     if (_on)
     {
          // Compute red level with brightness
          if (_brightness)
               return (Red * _brightness) >> 8;
          else
               return Red;
     }
     else
          return 0;
}

/// @brief Return current color of the pixel (with brightness)
/// @return Current green value (0..255)
uint8_t Pixel::GetGreen()
{
     // If the led is On
     if (_on)
     {
          // Compute red level with brightness
          if (_brightness)
               return (Green * _brightness) >> 8;
          else
               return Green;
     }
     else
          return 0;
}

/// @brief Return current color of the pixel (with brightness)
/// @return Current blue value (0..255)
uint8_t Pixel::GetBlue()
{
     // If the led is On
     if (_on)
     {
          // Compute red level with brightness
          if (_brightness)
               return (Blue * _brightness) >> 8;
          else
               return Blue;
     }
     else
          return 0;
}

/// @brief Return packed color of the pixel, can be used directely with NeoPixel "setPixelColor" function.
/// @return Current value (0xRRGGBB)
uint32_t Pixel::GetColor()
{
     // If the led is On
     if (_on)
     {
          // Compute packed level with brightness
          if (_brightness)
               return (((uint32_t)(Red << 8) / _brightness) << 16) |
                      (((uint32_t)(Green << 8) / _brightness) << 8) |
                      ((uint32_t)(Blue << 8) / _brightness);
          else
               return ((uint32_t)Red << 16) | ((uint32_t)Green << 8) | (uint32_t)Blue;
     }
     else
          return 0;
}

///////////////////////////   Color update   ///////////////////////////

/// @brief Update color of the pixel
/// @param newColor New pixel color
void Pixel::SetColor(Pixel newColor)
{
     this->Red = newColor.Red;
     this->Green = newColor.Green;
     this->Blue = newColor.Blue;
}

/// @brief Update color of the pixel
/// @param r Red componant (0..255)
/// @param g Green componant (0..255)
/// @param b Blue componant (0..255)
void Pixel::SetColor(uint8_t r, uint8_t g, uint8_t b)
{
     this->Red = r;
     this->Green = g;
     this->Blue = b;
}

/// @brief Update color of the pixel
/// @param newColor Set a pixel's color using a 32-bit 'packed' RGB (0xAAFF88)
void Pixel::SetColor(uint32_t newColor)
{
     this->Red = (uint8_t)(newColor >> 16);
     this->Green = (uint8_t)(newColor >> 8);
     this->Blue = (uint8_t)(newColor);
}

/////////////////////////////////////////////////////////////////////////////////////////////
//                                     Neopixel manager                                    //
/////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////   Constructor   ///////////////////////////

/// @brief Create a pixel object
MyPixels::MyPixels()
{
     this->_numLEDs = 0;
     this->_asInfo = false;
}

/// @brief Use the first pixel as status info. Need to be done in MySetup
void MyPixels::useInfoPixel()
{
     // Add the number of pixel
     this->_numLEDs = this->_numLEDs + 1;

     // and save the info pixel used
     this->_asInfo = true;
}

/// @brief Waiting the wifi connection
void MyPixels::staWaitWifi()
{
#ifdef LOG
     Serial.println("Status: Wait wifi");
#endif
     // Backup of color
     _infoCOlor = 0x0000FF;

     // Put the led in blue
     _leds.setPixelColor(0, _infoCOlor);

     // Update color
     _leds.show();
}

/// @brief Waiting for the server
void MyPixels::staWaitServer()
{
#ifdef LOG
     Serial.println("Status: Wait server");
#endif
     // Backup of color
     _infoCOlor = 0xEE82EE;

     // Put the led in blue
     _leds.setPixelColor(0, _infoCOlor);

     // Update color
     _leds.show();
}

/// @brief Waiting for the server
void MyPixels::staConnected()
{
#ifdef LOG
     Serial.println("Status: Connected");
#endif
     // Backup of color
     _infoCOlor = 0x32CD32;

     // Put the led in blue
     _leds.setPixelColor(0, _infoCOlor);

     // Update color
     _leds.show();
}

/// @brief Module is in simulation (Yellow)
void MyPixels::staSimulation()
{
#ifdef LOG
     Serial.println("Status: In simulation");
#endif
     // Backup of color
     _infoCOlor = 0xFFFF00;

     // Put the led in color
     _leds.setPixelColor(0, _infoCOlor);

     // Update color
     _leds.show();
}

/// @brief Add a number of led to control. Need to be done in MySetup
/// @param number Number of led in the project
void MyPixels::addLeds(int number)
{
     // Add the number of pixel
     this->_numLEDs = this->_numLEDs + number;
}

/// @brief Step added in every cycle to make the animation. The value will move from 0 to 255
/// @param step Value added in the variation step, default is 1, need to change depanding of the load of IC)
void MyPixels::timeVariator(float step)
{
     // Update step time
     this->_ratioStep = step;
}

/// @brief Clear every led and delete variator
void MyPixels::clear()
{
     // Clear all led
     _leds.clear();

     // Set status led back
     if (_asInfo)
          _leds.setPixelColor(0, _infoCOlor);

     // Delete all variator
     _variators.clear();

     // Update leds
     _leds.show();
}

/// @brief Set a pixel color with a pixel object
/// @param index Pixel to edit, start at 1
/// @param newColor Pixel color
void MyPixels::setPixelColor(int index, const Pixel &newColor)
{
     this->setPixelColor(index, newColor.Red, newColor.Green, newColor.Blue);
}

/// @brief Set a pixel color with RRB values
/// @param index Pixel to edit, start at 1
/// @param r Red color (0..255)
/// @param g Green color (0..255)
/// @param b Blue color (0..255)
void MyPixels::setPixelColor(int index, uint8_t r, uint8_t g, uint8_t b)
{
     // Check if index is in range
     if (index >= 0 && index < this->_numLEDs)
     {
          // Check if the pixel as variator
          if (_asVariator(index))
          {
               deleteVariator(index);
          }

          // Set new pixel color
          _leds.setPixelColor(index, r, g, b);

          // Ask for pixel update
          _updateRequest = true;

#ifdef LOG
          Serial.print("Pixels: Set pixel ");
          Serial.print(index);
          Serial.print(" with color ");
          Serial.print(r);
          Serial.print(",");
          Serial.print(g);
          Serial.print(",");
          Serial.println(b);
#endif
     }
}

/// @brief Create a cycling effect on a pixel between two colors
/// @param index Pixel affected by the effect
/// @param colorStart Starting color of cycling effect
/// @param colorEnd Ending color of cycling effect
void MyPixels::addVariator(int index, const Pixel &colorStart, const Pixel &colorEnd)
{
     _variators.push_back(Variator(index, colorStart, colorEnd));
}

/// @brief Delete a cycling effect on a pixel between two colors
/// @param index Pixel affected by the effect
void MyPixels::deleteVariator(int index)
{

     auto it = std::remove_if(_variators.begin(), _variators.end(), [this, index](Variator &v)
                              {
        if (v.getIndex() == index) {
            _leds.setPixelColor(index, 0); // Mise à zéro de la couleur à la fin du variateur (ajustez selon vos besoins)
            return true;
        }
        return false; });

     _variators.erase(it, _variators.end());

#ifdef LOG
     Serial.print("Pixels: Delete variator n°");
     Serial.println(index);
#endif

     // Show up
     _show();
}

bool MyPixels::_asVariator(int index)
{
     if (_variators.size() == 0)
          return false;

     // Update ratio value
     for (auto &variateur : _variators)
     {
          if (variateur.getIndex() == index)
          {
               return true;
          }
     }

     return false;
}

/// @brief Initalize the leds controler. DO NOT USE, already done in master
void MyPixels::initalize()
{

#ifdef LOG
     Serial.print("Pixels: Init ");
     Serial.print(_numLEDs);
     Serial.println(" pixels");
#endif

     // Set parameter of neopixel
     _leds.updateLength(_numLEDs);
     _leds.updateType(NEO_GRB + NEO_KHZ800);
     _leds.setPin(0);

     // Initalize pixels
     _leds.begin();
     _leds.clear();
     _leds.show();
}

/// @brief Update pixel output. DO NOT USE, already done in master
void MyPixels::update()
{

     if (_variators.size() > 0)
     {
          // Update ratio
          if (_ratioDown)
          {
               _ratio -= _ratioStep;

               if (_ratio < 0.0)
               {
                    _ratio = 0;
                    _ratioDown = false;
               }
          }
          else
          {
               _ratio += _ratioStep;

               if (_ratio > 255)
               {
                    _ratio = 255;
                    _ratioDown = true;
               }
          }

          // Update ratio value
          for (auto &variateur : _variators)
          {
               variateur.update(&_leds, _ratio);
          }

          // Update button
          _show();
     }
     else
     {
          // If update is needed
          if (this->_updateRequest)
          {
#ifdef LOG
               // Log send text
               Serial.println("LED - Update is done");
#endif

               _show();
          }
     }
}

/// @brief Update pixel output
void MyPixels::_show()
{

     _leds.show();
     this->_updateRequest = false;

     // yield();
}

/////////////////////////////////////////////////////////////////////////////////////////////
//                                      Button manager                                     //
/////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////   Constructor   ///////////////////////////

/// @brief Define button on a digital input
/// @param pin Input pin
Button::Button(uint8_t pin)
{
     // Save parameter
     _pin = pin;
     _isLCD = false;

     // Config of button
     pinMode(pin, INPUT);
     digitalWrite(pin, HIGH);

     // Read button
     if (digitalRead(pin) > 0)
          _state = true;
     else
          _state = false;

     // Save backup
     _backup = _state;
}

/// @brief Define button on a LCD
/// @param lcd LCD to read input
/// @param input Input to check
Button::Button(LiquidTWI2 *lcd, uint8_t input)
{
     // Save parameter
     _pin = input;
     _lcd = lcd;
     _isLCD = true;

     // get button state on LCD
     if (_lcd->readButtons() & _pin)
          _state = true;
     else
          _state = false;

     // Save backup
     _backup = _state;
}

void Button::invert(void)
{
     // Save if input must be inverted
     _invert = !_invert;

     // Invert the current state
     _state = !_state;
     _backup = !_backup;
}

void Button::read(void)
{
     // Read state
     if (_isLCD)
     {
          // get button state on LCD
          if (_lcd->readButtons() & _pin)
               _state = true;
          else
               _state = false;
     }
     else
     {
          // Read button
          if (digitalRead(_pin) > 0)
               _state = true;
          else
               _state = false;
     }

     // Invert input if needed
     if (_invert)
          _state = !_state;

     // Check if update is done
     if (_backup != _state)
     {
          _backup = _state;

          if (_state == HIGH)
          {
               _up = true;
          }
          else
          {
               _down = true;
          }
     }
     else
     {
          _down = false;
          _up = false;
     }
}

bool Button::value(void)
{

     if (_state)
     {
          return true;
     }
     else
     {
          return false;
     }
}

bool Button::up(void)
{
     return _up;
}

bool Button::down(void)
{
     return _down;
}
