#define VERSION     "\n\nAndroTest V2.0 - @kas2014\ndemo for V5.x App"
#include "SoftwareSerial.h"
#define    STX          0x02
#define    ETX          0x03
#define    ledPin       13
#define    SLOW         750                            // Datafields refresh rate (ms)
#define    FAST         250                             // Datafields refresh rate (ms)

SoftwareSerial mySerial(2,3);                           // BlueTooth module: pin#2=TX pin#3=RX
byte cmd[8] = {0, 0, 0, 0, 0, 0, 0, 0};                 // bytes received
byte buttonStatus = 0;                                  // first Byte sent to Android device
long previousMillis = 0;                                // will store last time Buttons status was updated
long sendInterval = SLOW;                               // interval between Buttons status transmission (milliseconds)
String displayStatus = "xxxx";    
// message to Android device
//motor one
int enA = 5;
int in1 = 6;
int in2 = 7;
// motor two
int enB = 10;
int in3 = 8;
int in4 = 9;
int speedA = 0;
int speedB = 0;

void setup()  {
 Serial.begin(9600);
 mySerial.begin(9600);                    
 pinMode(ledPin, OUTPUT);    
 Serial.println(VERSION);
 pinMode(enA, OUTPUT);
 pinMode(enB, OUTPUT);
 pinMode(in1, OUTPUT);
 pinMode(in2, OUTPUT);
 pinMode(in3, OUTPUT);
 pinMode(in4, OUTPUT);
 while(mySerial.available())  mySerial.read();         // empty RX buffer
 analogWrite(enB, 150);
}

void loop() {
 if(mySerial.available())  {                           // data received from smartphone
   delay(2);
   cmd[0] =  mySerial.read();  
   if(cmd[0] == STX)  {
     int i=1;      
     while(mySerial.available())  {
       delay(1);
       cmd[i] = mySerial.read();
       if(cmd[i]>127 || i>7)                 break;     // Communication error
       if((cmd[i]==ETX) && (i==2 || i==7))   break;     // Button or Joystick data
       i++;
     }

     if(i==7)          getJoystickState(cmd);     // 6 Bytes  ex: < STX "200" "180" ETX >
   }
 }
 sendBlueToothData();
}

void sendBlueToothData()  {
 static long previousMillis = 0;                            
 long currentMillis = millis();
 if(currentMillis - previousMillis > sendInterval) {   // send data back to smartphone
   previousMillis = currentMillis;

// Data frame transmitted back from Arduino to Android device:
// < 0X02   Buttons state   0X01   DataField#1   0x04   DataField#2   0x05   DataField#3    0x03 >  
// < 0X02      "01011"      0X01     "120.00"    0x04     "-4500"     0x05  "Motor enabled" 0x03 >    // example

   mySerial.print((char)STX);                                             // Start of Transmission
   mySerial.print(getButtonStatusString());  mySerial.print((char)0x1);   // buttons status feedback
   mySerial.print(GetdataInt1());            mySerial.print((char)0x4);   // datafield #1
   mySerial.print(GetdataFloat2());          mySerial.print((char)0x5);   // datafield #2
   mySerial.print(displayStatus);                                         // datafield #3
   mySerial.print((char)ETX);                                             // End of Transmission
 }  
}

String getButtonStatusString()  {
 String bStatus = "";
 for(int i=0; i<6; i++)  {
   if(buttonStatus & (B100000 >>i))      bStatus += "1";
   else                                  bStatus += "0";
 }
 return bStatus;
}

int GetdataInt1()  {              // Data dummy values sent to Android device for demo purpose
 static int i= -30;              // Replace with your own code
 i ++;
 if(i >0)    i = -30;
 return i;  
}

float GetdataFloat2()  {           // Data dummy values sent to Android device for demo purpose
 static float i=50;               // Replace with your own code
 i-=.5;
 if(i <-50)    i = 50;
 return i;  
}

void getJoystickState(byte data[8])    {
 int joyX = (data[1]-48)*100 + (data[2]-48)*10 + (data[3]-48);       // obtain the Int from the ASCII representation
 int joyY = (data[4]-48)*100 + (data[5]-48)*10 + (data[6]-48);
 joyX = joyX - 200;                                                  // Offset to avoid
 joyY = joyY - 200;                                                  // transmitting negative numbers

 if(joyX<-100 || joyX>100 || joyY<-100 || joyY>100)     return;      // commmunication error
 
// Your code here ...
   Serial.print("Joystick pos:  ");
   Serial.print(joyX);  
   Serial.print(", ");  
   Serial.println(joyY);

//DC endavant / enrrere   
   if (joyY >= 0){
    speedA = joyY*2.5;
    speedB = joyY*2.5;
   digitalWrite(in1, LOW);
   digitalWrite(in2, HIGH);
   digitalWrite(in4, LOW);
   digitalWrite(in3, HIGH);
   analogWrite(enA, speedA);
   analogWrite(enB, speedB);
   }
  
   if (joyY < 0){
    speedA = joyY*2.5*-1;
    speedB = joyY*2.5*-1;    
   digitalWrite(in1, HIGH);
   digitalWrite(in2, LOW);
   digitalWrite(in3, LOW);
   digitalWrite(in4, HIGH);
   analogWrite(enA, speedA);
   analogWrite(enB, speedB);
    
   }

}


