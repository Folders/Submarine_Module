#include <common.h>
#ifdef MODEL

#include <Arduino.h>


//////// Add new include library
//#include <Encoder.h> 


////////  Define global constantes      (ALWAYS IN MAJ)
//const int TEST_IN = D0; 
//const int TEST_OUT = D5; 


////////  Define global variables
int testInt = 0;
bool testBool = 0;


/////////////////////////////////////////////////////////////////////////////////////////////
//                                      User function                                      //
/////////////////////////////////////////////////////////////////////////////////////////////

/// @brief Si je tape ///, il me propose de mettre des commentaires à la fonction
void my_function()
{
    // ...
}


/////////////////////////////////////////////////////////////////////////////////////////////
//                                     Setup and reset                                     //
/////////////////////////////////////////////////////////////////////////////////////////////

/// @brief Setup function for the module
void MySetup() 
{
     Serial.println("--- Model ---");
}
      

    

///////////////////////////////  Reset all proprety of module  //////////////////////////////// 

/// @brief Call after the config and when the module reset by the app
void ResetModule()
{

}



/////////////////////////////////  Write here the loop code  /////////////////////////////////
//
// To send datas to the server, use the send function "Send("COD;xxx;yyy");" 
void MyLoop()
{
    
}



/////////////////////////////////////////////////////////////////////////////////////////////
//                                      Communication                                      //
/////////////////////////////////////////////////////////////////////////////////////////////


// Call when a message is received from server
void Received()
{
    
    // Get info of received string
    comm.Info_Received();


    // Update navigation status
    if (comm.GetCode() == "NVS")
    {
        // DO something
        Serial.println("WOOW, j'ai reçu un NVS!");
    }

}



// Call when a message is received to "close the loop" of the server. 
// Every send fonction will call this in standalone
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