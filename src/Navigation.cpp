#include <common.h>
#ifdef NAVIGATION

#include <Arduino.h>


//// Add new include library
//#include <Encoder.h> 


////  Define global constantes      (ALWAYS IN MAJ)
const int LIGHT_BLOCKEDLEFT = 16;   // D0
const int LIGHT_LEFT = 14;          // D5
const int LIGHT_CENTER = 12;        // D6
const int LIGHT_RIGHT = 13;         // D7
const int LIGHT_BLOCKEDRIGHT = 15;  // D8

const int BTN_LEFT = 5; 
const int BTN_RIGHT = 4; 

const int MAX = 150; 
const int WAIT_MAX = 80;


////  Define global variables
int Old_Left = 0;
int Old_Right = 0;
bool Sta_Left = 0;
bool Sta_Right = 0;
bool Sta_Move = 0;
int Pos = 0;
int Wait = 0;


////////////////////////////  Write here the your functions  ////////////////////////////

// we recived a comm from the app for lighting the direction LED
void StatusUpdate(char sta) 
{ 
  
  #ifdef LOG
  Serial.println("");
  Serial.println("Sta : ");
  Serial.println(sta);
  #endif


  switch (sta){ 

    case 'L': 
      digitalWrite(LIGHT_BLOCKEDLEFT, HIGH);
      digitalWrite(LIGHT_LEFT, LOW);
      digitalWrite(LIGHT_CENTER, LOW);
      digitalWrite(LIGHT_RIGHT, LOW);
      digitalWrite(LIGHT_BLOCKEDRIGHT, LOW);
      break;
      
    case 'l':
      digitalWrite(LIGHT_BLOCKEDLEFT, LOW);
      digitalWrite(LIGHT_LEFT, HIGH);
      digitalWrite(LIGHT_CENTER, LOW);
      digitalWrite(LIGHT_RIGHT, LOW);
      digitalWrite(LIGHT_BLOCKEDRIGHT, LOW);
      break;
    case 'c': 
      digitalWrite(LIGHT_BLOCKEDLEFT, LOW);
      digitalWrite(LIGHT_LEFT, LOW);
      digitalWrite(LIGHT_CENTER, HIGH);
      digitalWrite(LIGHT_RIGHT, LOW);
      digitalWrite(LIGHT_BLOCKEDRIGHT, LOW);
      break;
    case 'r': 
      digitalWrite(LIGHT_BLOCKEDLEFT, LOW);
      digitalWrite(LIGHT_LEFT, LOW);
      digitalWrite(LIGHT_CENTER, LOW);
      digitalWrite(LIGHT_RIGHT, HIGH);
      digitalWrite(LIGHT_BLOCKEDRIGHT, LOW);
      break;
    case 'R': 
      digitalWrite(LIGHT_BLOCKEDLEFT, LOW);
      digitalWrite(LIGHT_LEFT, LOW);
      digitalWrite(LIGHT_CENTER, LOW);
      digitalWrite(LIGHT_RIGHT, LOW);
      digitalWrite(LIGHT_BLOCKEDRIGHT, HIGH);
      break;
      
    default:
      digitalWrite(LIGHT_BLOCKEDLEFT, LOW);
      digitalWrite(LIGHT_LEFT, LOW);
      digitalWrite(LIGHT_CENTER, HIGH);
      digitalWrite(LIGHT_RIGHT, LOW);
      digitalWrite(LIGHT_BLOCKEDRIGHT, LOW);

  }
  
}


/////////////////////////////////  Write here the setup code  /////////////////////////////////
void MySetup() 
{
    // All led output
    pinMode (LIGHT_BLOCKEDLEFT,OUTPUT);
    pinMode (LIGHT_LEFT,OUTPUT);
    pinMode (LIGHT_CENTER,OUTPUT);
    pinMode (LIGHT_RIGHT,OUTPUT);
    pinMode (LIGHT_BLOCKEDRIGHT,OUTPUT);  
    
    // All button input
    pinMode (BTN_RIGHT,INPUT_PULLUP);
    pinMode (BTN_LEFT,INPUT_PULLUP);  
}
      

    

///////////////////////////////  Reset all proprety of module  //////////////////////////////// 
//
// Call after the config and when the module reset
void ResetModule()
{
    // Reset led status
    StatusUpdate('c');
}



/////////////////////////////////  Write here the loop code  /////////////////////////////////
//
// To send datas to the server, use the send function "Send("COD;xxx;yyy");" 
void MyLoop()
{
    
    if (digitalRead(BTN_LEFT))
        Sta_Left = false;
    else
        Sta_Left = true;
    
    if (digitalRead(BTN_RIGHT))
       Sta_Right = false;
    else
      Sta_Right = true;
    
    if (not (Sta_Left and Sta_Right))
    {
        if (Sta_Left)
        {
            Pos -= 1;
            Sta_Move = true;
        }
        
        if (Sta_Right)
        {
            Pos += 1;
            Sta_Move = true;
        } 
        
        
        if (Pos > MAX)
        {
            Pos = 0;
            comm.send("NVC;R");
        }
        
        if (Pos < -MAX)
        {
            Pos = 0;
            comm.send("NVC;L");
        }
        
        // If stopped
        if (Sta_Left == false and Sta_Right == false)
        {
          
            // Wait to stop
            if (Sta_Move)
            {
                Wait += 1;
                
                if (Wait > WAIT_MAX )
                {
                    Sta_Move = false;
                    Pos = 0;
                    Wait = 0;
                    
                    comm.send("NVC;S");

                }
            }
        }
    }     
}



/////////////////////////////////////////////////////////////////////////////////////////////
//                                      Communication                                      //
/////////////////////////////////////////////////////////////////////////////////////////////


// Call when a message is received from server
void Received()
{
    
    // Update navigation status
    if ( comm.GetCode() == "NVS")
    {
        StatusUpdate( comm.GetParameter(1)[0] );
    }

}

int simuPos = 0;

// Call when a message is received to "close the loop" of the server. 
// Every send fonction will call this in standalone
void ServerSimulation()
{
    // test
    comm.Info_Received();

    // Update navigation status
    if ( comm.GetCode() == "NVC")
    {
        if (comm.GetParameter(1) == "L" && simuPos > -2)
        {
            simuPos -= 1;
        }

        if (comm.GetParameter(1) == "R" && simuPos < 2)
        {
            simuPos += 1;
        }

        switch (simuPos)
        {
        case -2:
            StatusUpdate('L');
            break;
        case -1:
            StatusUpdate('l');
            break;
        case 0:
            StatusUpdate('c');
            break;
        case 1:
            StatusUpdate('r');
            break;
        case 2:
            StatusUpdate('R');
            break;
        
        default:
            break;
        }
    }

}



#endif