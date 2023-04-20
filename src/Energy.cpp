#include <common.h>
#ifdef ENERGY

#include <Arduino.h>

//////// Add new include library
#include <Adafruit_NeoPixel.h>
#include <MFRC522.h>
#include <SPI.h>
#include <Ticker.h>

////////  Define global constantes      (ALWAYS IN MAJ)

// define NEOPIXELS settings
#define NEOPIXELS
#define PIN 5
#define NUMPIXELS 3 // insert the total of pixels
Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);
const int BRIGHTNESS = 255;

// RFID settings
#define SS_PIN 15
#define RST_PIN 0
MFRC522 rfid(SS_PIN, RST_PIN);

////////  Define global variables
Ticker _Display_Led_Percent;
int percent = 0;
bool locked = false;

/////////////////////////////////////////////////////////////////////////////////////////////
//                                      User function                                      //
/////////////////////////////////////////////////////////////////////////////////////////////

/// @brief Convert the UID array into a string
/// @param array
/// @param len
/// @param buffer
void array_to_string(byte array[], unsigned int len, char buffer[])
{
    for (unsigned int i = 0; i < len; i++)
    {
        byte nib1 = (array[i] >> 4) & 0x0F;
        byte nib2 = (array[i] >> 0) & 0x0F;
        buffer[i * 2 + 0] = nib1 < 0xA ? '0' + nib1 : 'A' + nib1 - 0xA;
        buffer[i * 2 + 1] = nib2 < 0xA ? '0' + nib2 : 'A' + nib2 - 0xA;
    }
    buffer[len * 2] = '\0';
}

/// @brief convert the UID scanned into a string. First in array, then string
void convert_UID() // convert the UID into String
{
    char UID_str[32] = "";
    array_to_string(rfid.uid.uidByte, 4, UID_str);
    comm.start("NFC;");
    comm.send(UID_str);

}

/// @brief For print the UID into Serial
/// @param buffer
/// @param bufferSize
void printHex(byte *buffer, byte bufferSize)
{ // display the UID in Serial
    for (byte i = 0; i < bufferSize; i++)
    {

#ifdef LOG
        Serial.print(((buffer[i]) >> 4) & 0x0F, HEX);
        Serial.print(buffer[i] & 0x0F, HEX);
        Serial.print(" ");
#endif
    }
}

/**
 * Returns true if a PICC responds to PICC_CMD_WUPA.
 * All cards in state IDLE or HALT are invited.
 *
 * @return bool
 */

/// @brief Read if there is a card on the RFID reader
/// @return a bool
bool PICC_IsAnyCardPresent()
{
    byte bufferATQA[2];
    byte bufferSize = sizeof(bufferATQA);

    // Reset baud rates
    rfid.PCD_WriteRegister(rfid.TxModeReg, 0x00);
    rfid.PCD_WriteRegister(rfid.RxModeReg, 0x00);
    // Reset ModWidthReg
    rfid.PCD_WriteRegister(rfid.ModWidthReg, 0x26);

    MFRC522::StatusCode result = rfid.PICC_WakeupA(bufferATQA, &bufferSize);
    return (result == MFRC522::STATUS_OK || result == MFRC522::STATUS_COLLISION);
} // End PICC_IsAnyCardPresent()

/// @brief display the percent with colors. Green = full, red = empty
void percent_leddisplay()
{
    float r = (255 - (255 * percent / 100));
    float g = (255 * percent / 100);
    int b = 0;
    for (int i = 0; i < NUMPIXELS; i++)
    {
        pixels.setPixelColor(i, r, g, b);
    }
    pixels.show();
}

/////////////////////////////////////////////////////////////////////////////////////////////
//                                     Setup and reset                                     //
/////////////////////////////////////////////////////////////////////////////////////////////

/// @brief Setup function for the module
void MySetup()
{
    // RFID setup
    SPI.begin();     // Init SPI bus
    rfid.PCD_Init(); // Init MFRC522
    rfid.uid.size = 0;

    // Neopixels setup
    pixels.begin();
    pixels.setBrightness(BRIGHTNESS);
    _Display_Led_Percent.attach(0.5, percent_leddisplay); // start the ticker who's display the percent with leds
}

///////////////////////////////  Reset all proprety of module  ////////////////////////////////

/// @brief Call after the config and when the module reset by the app
void ResetModule()
{
    percent = 0;
    locked = false;
    rfid.uid.size = 0;
}

/////////////////////////////////  Write here the loop code  /////////////////////////////////

/// @brief Call at the end of the main loop function
void MyLoop()
{

    bool cardPresent = PICC_IsAnyCardPresent(); // read if there is a RFID connection

    // Reset the loop if no card was locked an no card is present.
    // This saves the select process when no card is found.
    if (!locked && !cardPresent)
        return;

    // When a card is present (locked) the rest ahead is intensive (constantly checking if still present).

    // Ask for the locked card (if rfid.uid.size > 0) or for any card if none was locked.
    // (Even if there was some error in the wake up procedure, attempt to contact the locked card.
    // This serves as a double-check to confirm removals.)
    // If a card was locked and now is removed, other cards will not be selected until next loop,
    // after rfid.uid.size has been set to 0.

    MFRC522::StatusCode result = rfid.PICC_Select(&rfid.uid, 8 * rfid.uid.size);

    if (!locked && result == MFRC522::STATUS_OK)
    {
        locked = true;

        //////////// Actions on card detection //////////////////

#ifdef LOG
        Serial.print(F("locked! NUID tag: "));
        printHex(rfid.uid.uidByte, rfid.uid.size); // print the UID into the Serial
        Serial.println();
#endif

        convert_UID(); // convert UID into STRING and send it to the server
    }
    else if (locked && result != MFRC522::STATUS_OK)
    {
        locked = false;
        rfid.uid.size = 0;

        ///////////// Action on card removal ////////////
       
        comm.send("NFC;N"); //

#ifdef LOG
        Serial.print(F("unlocked! Reason for unlocking: "));
        Serial.println(rfid.GetStatusCodeName(result));
#endif
    }
    else if (!locked && result != MFRC522::STATUS_OK)
    {
        // Clear locked card data just in case some data was retrieved in the select procedure
        // but an error prevented locking.
        rfid.uid.size = 0;
    }

    rfid.PICC_HaltA();
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

    if (comm.GetCode() == "NRJ")
    {
        percent = comm.GetParameter(1).toInt();
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