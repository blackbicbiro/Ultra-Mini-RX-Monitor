//########################
#define CodeVersion 1.1
//########################


/*

Author: Rob Paddock
 To do:
 3. Connection Check
 4. Modulations Values
 
 */



#include <SPI.h>
#include <SoftwareSerial.h>
#include "mcp_can.h"

const int LCDdelay=10;  // conservative, 2 actually works
#define txPin 6 //LCD display pin
#define COMMAND 0xFE
#define CLEAR   0x01
#define LINE0   0x80
#define LINE1   0xC0
SoftwareSerial LCD =  SoftwareSerial(3, txPin); // Serial LCD connected to output 6

unsigned char Flag_Recv = 0;
unsigned char len = 0;
unsigned char buf[8];
char str[20];

int DTemp = 0;        //Decoder tempreture
int RSSI_New = 0;
int Carrier_Noise = 0;
int MER = 0;
int refresh = 150; //delay between each sent command stops from clogging CANBUS
int CAN_Check = 0;

void setup()
{
  pinMode(txPin, OUTPUT);
  Serial.begin(115200);
  pinMode(txPin, OUTPUT);
  LCD.begin(9600);
  backlightOn();        //Turn LCD back light on

    LCDPosition(0,0);      
  LCD.write("  UltraMini RX  ");
  LCDPosition(1,0);
  LCD.write("     Monitor    ");
  delay(1000);
  ClearLCD();
  LCDPosition(0,0);
  LCD.write("      V");
  LCD.print(CodeVersion);
  delay(1000);
  ClearLCD();


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

void loop()  //################################################################################################//
{

  Request_Sig();        //Send Request for RSSI
  Check_CAN_RX();       //Check for incoming data and fill arduino buffer
  SigCheck();           // Check buffer for Signal CAN message and calculate values
  Request_DTemp();
  Check_CAN_RX();
  TempCheck();

 
    Serial.print("RSSI: -");      //Print values to serial
    Serial.print(RSSI_New);
    Serial.print("dB");
    Serial.print("\t");
    Serial.print("C/noise: ");
    Serial.print(Carrier_Noise);
    Serial.print("\t");
    Serial.print("MER:");
    Serial.print(MER);
    Serial.print("\t");
    Serial.print("Decoder Temp:");
    Serial.print(DTemp);
    Serial.println(); 

    LCDPosition(0,0);
    LCD.write("RSSI:-          ");    //spaces stops old char being left behind
    LCDPosition(0,6);
    LCD.print(RSSI_New);
    LCDPosition(0,9);
    LCD.write("dB");
    LCDPosition(0,13);
    LCD.print(DTemp);
    LCD.write(223);


    LCDPosition(1,0);
    LCD.write("C/N:            ");  //spaces stops old char being left behind
    LCDPosition(1,6);
    LCD.print(Carrier_Noise);
    LCDPosition(1,9);
    LCD.write("dB");
  


}//End of Void Loop()
//##################################################################################################################################//


//////////////////SubFuntions//////////////

void Request_Sig(){    //Send Resqust for RSSI and Carrier To nois
  unsigned char stmp[8] = {
    4, 0, 0, 0, 0, 0, 0, 0   };
  CAN.sendMsgBuf(0x21, 0, 8, stmp);   // send data:  id = 0x00, standrad flame, data len = 8, stmp: data buf
  delay(refresh);                          // when the delay less than 20ms, you shield use receive_interrupt
  CAN_Check = 4;    // changed dependinf on data request
}





void Request_DTemp(){    //Send Request for Decoder Tempreture
  unsigned char stmp[8] = {
    5, 0, 0, 0, 0, 0, 0, 0   };
  CAN.sendMsgBuf(0x22, 0, 8, stmp);   // send data:  id = 0x00, standrad flame, data len = 8, stmp: data buf
  delay(refresh);                          // when the delay less than 20ms, you shield use receive_interrupt
  CAN_Check = 5;    // changed dependinf on data request
}








void Check_CAN_RX(){
  if(CAN_MSGAVAIL == CAN.checkReceive())            // check if data coming
  {
    CAN.readMsgBuf(&len, buf);    // read data,  len: data length, buf: data bus
    for(int i = 0; i<len; i++)    // print the data
    {
      Serial.print(buf[i]);    // for serial debug
      Serial.print("\t");      // for serial debug
    }
    Serial.println();
  }
  else{
    //Clear Display if no messages are being RX'd
    buf[0] = CAN_Check;
    buf[1] = 0;
    buf[2] = 0;
    buf[3] = 0;
    buf[4] = 0;
    buf[5] = 0;
    buf[6] = 0;
    buf[7] = 0;
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





void TempCheck(){
  if(buf[0] == 5){  
    DTemp = 0.5* buf[3];      //convertes raw data to Actual tempreture
  } 
}




////LCD function Setup

void LCDPosition(int row, int col) {    // set up screen position
  LCD.write(0xFE);   //command flag
  LCD.write((col + row*64 + 128));    //position 
  delay(LCDdelay);
}
void ClearLCD(){     // clear LCD
  LCD.write(0xFE);   //command flag
  LCD.write(0x01);   //clear command.
  delay(LCDdelay);
}
void backlightOn() {  //turns on the backlight       //Check this works
  LCD.write(0x7C);   //command flag for backlight stuff
  LCD.write(158);    //light level.
  delay(LCDdelay);
}
void backlightOff(){  //turns off the backlight CHCK THIS WORKS
  LCD.write(0x7C);   //command flag for backlight stuff
  LCD.write(128);     //light level for off.
  delay(LCDdelay);
}
void serCommand(){   //a general function to call the command flag for issuing all other commands   
  LCD.write(0xFE);
}







//////future expansion

void RX_Mod_Check(){////////////check Modulation type/////////////

}








