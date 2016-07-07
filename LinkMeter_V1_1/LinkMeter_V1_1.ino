//###################
#define Version 1.1
//###################


/*

 To do:
 1. clear values when connection lost
 2. LCD display using sLCD
 3. Tempreture Values
 4. Modulations Values
 
 
*/


#include <SPI.h>
#include "mcp_can.h"

unsigned char Flag_Recv = 0;
unsigned char len = 0;
unsigned char buf[8];
char str[20];

int RSSI_New = 0;
int Carrier_Noise = 0;
int MER = 0;
int refresh = 1000; //delay between each sent command

void setup()
{
  Serial.begin(115200);

START_INIT:

  if(CAN_OK == CAN.begin(CAN_500KBPS))                   // init can bus : baudrate = 500k
  {
    Serial.println("CAN BUS Shield init ok!");
  }
  else
  {
    Serial.println("CAN BUS Shield init fail");
    Serial.println("Init CAN BUS Shield again");
    delay(100);
    goto START_INIT;
  }
}///////////////////////////////////End of setup////////////////////////////

void loop()
{

  Request_Sig();        //Send Request for RSSI
  Check_CAN_RX();       //Check for incoming data
  SigCheck();           // Check buffer for Signal CAN message and calculate values


  Serial.print("RSSI: -");      //Print values to serial
  Serial.print(RSSI_New);
  Serial.print("dB");
  Serial.print("\t");
  Serial.print("C/noise:");
  Serial.print(Carrier_Noise);
  Serial.print("\t");
  Serial.print("MER:");
  Serial.print(MER);
  Serial.println(); 

}//End of Void Loop()



//////////////////SubFuntions////////////////

void Request_Sig(){    //Send Resqust for RSSI and Carrier To noise
  unsigned char stmp[8] = {
    4, 0, 0, 0, 0, 0, 0, 0      };
  CAN.sendMsgBuf(0x21, 0, 8, stmp);   // send data:  id = 0x00, standrad flame, data len = 8, stmp: data buf
  delay(refresh);                          // when the delay less than 20ms, you shield use receive_interrupt
}



void Check_CAN_RX(){
  if(CAN_MSGAVAIL == CAN.checkReceive())            // check if data coming
  {
    CAN.readMsgBuf(&len, buf);    // read data,  len: data length, buf: data buf
      
      for(int i = 0; i<len; i++)    // print the data
    {
      Serial.print(buf[i]);
      Serial.print("\t");
    }
    Serial.println();
  } 
}



void SigCheck(){  //////////////signal check////////////
  if(buf[0] == 4){
    RSSI_New = 256-buf[1];  //This Gives me RSSI actual value
    if(RSSI_New > 90){ //if no value show -90db
      RSSI_New = 90; 
    }
    Carrier_Noise = buf[2];
    MER = buf[3];
  }
}




//////future expansion
void TempCheck(){    ////////////tempreture check///////////////

}

void RX_Mod_Check(){////////////check Modulation type/////////////

}


