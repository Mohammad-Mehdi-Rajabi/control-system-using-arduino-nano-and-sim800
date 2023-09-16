
#include <SoftwareSerial.h>
#include <stdio.h>
#include <string.h>
#define SIM800_TX_PIN 8
#define SIM800_RX_PIN 7
SoftwareSerial mySerial(SIM800_TX_PIN, SIM800_RX_PIN); 
int device_1 = 9; 
int device_2 = 10; 
int device_3 = 11; 
int device_4 = 12; 
int index = 0;
String number = "";
String message = "";
char incomingByte;
String incomingData;
bool atCommand = true;
int motor_speed = 255;
int motor_run = 0;
//NTC B3950 Steinhart–Hart equation
int trshd_temp = 40;
int tempPin = A3;
float Vo;
float R1 = 100;
float logR2, R2, T, temp;
float c1 = 1.009249522e-03, c2 = 2.378405444e-04, c3 = 2.019202697e-07;
float tempC;
void setup()
{
  Serial.begin(9600); 
  mySerial.begin(9600); 
  pinMode(device_1, OUTPUT);
  pinMode(device_2, OUTPUT); 
  pinMode(device_3, OUTPUT); 
  pinMode(device_4, OUTPUT);
  pinMode(tempPin, INPUT); 
  digitalWrite(device_1, LOW);
  digitalWrite(device_2, LOW);
  digitalWrite(device_3, LOW); 
  digitalWrite(device_4, LOW); 
    while (!mySerial.available()) {
    mySerial.println("AT");
    delay(1000);
    Serial.println("try to connecting");
  }
  Serial.println("Connected now");
  mySerial.println("AT+CMGF=1");  //Set SMS Text Mode
  delay(1000);
  mySerial.println("AT+CNMI=1,2,0,0,0");  //procedure, how to receive messages from the network
  delay(1000);
  // mySerial.println("AT+CMGL=\"REC UNREAD\""); // Read unread messages
  Serial.println("Ready to received message");
}
void loop()
{
  tempC = get_temp();
  Serial.print("Temperature: ");
  Serial.print(tempC);
  Serial.println(" C");
  analogWrite(device_3, motor_run);

  delay(1500);
   if (tempC>trshd_temp) {
     digitalWrite(device_2, HIGH);
     Serial.println("Device 2 Turn on by temp.");
   }else{
     digitalWrite(device_2, LOW);
     Serial.println("Device 2 Turn off by temp.");

   }
  if (mySerial.available()) {
    delay(100);

    // Serial buffer
    while (mySerial.available()) {
      incomingByte = mySerial.read();
      incomingData += incomingByte;
    }
    delay(10);
    if (atCommand == false) {
      receivedMessage(incomingData, tempC);
    } else {
      atCommand = false;
    }
    //delete messages to save memory
    if (incomingData.indexOf("OK") == -1) {
      mySerial.println("AT+CMGDA=\"DEL ALL\"");
      delay(1000);
      atCommand = true;
    }
    incomingData = "";
  }
}
void receivedMessage(String inputString, float temp) {
  //Get The number of the sender
  index = inputString.indexOf('"') + 1;
  inputString = inputString.substring(index);
  index = inputString.indexOf('"');
  number = inputString.substring(0, index);
  Serial.println("Number: " + number);
  //Get The Message of the sender
  index = inputString.indexOf("\n") + 1;
  message = inputString.substring(index);
  message.trim();
  Serial.println("Message: " + message);
  message.toUpperCase(); // uppercase the message received

if((message.indexOf("T/") > -1)){
  //turn Device 1 ON
  if (message.indexOf("D/1/N") > -1) {
    digitalWrite(device_1, HIGH);
    delay(1000);
    Serial.println("Command: Device 1 Turn On");
    Serial.println("for time:"+getValue(message, '/', 1)+"s");
    delay(getValue(message, '/', 1).toInt()*1000);
        digitalWrite(device_1, LOW);
            Serial.println("for time:"+getValue(message, '/', 1)+"s "+"ended and turn off");


  }


  //turn Device 2 ON
  if (message.indexOf("D/2/N") > -1) {
    digitalWrite(device_2, HIGH);
    delay(1000);
    Serial.println("Command: Device 2 Turn On.");
    Serial.println("for time:"+getValue(message, '/', 1)+"s");
    delay(getValue(message, '/', 1).toInt()*1000);
    digitalWrite(device_2, LOW);
            Serial.println("for time:"+getValue(message, '/', 1)+"s "+"ended and turn off");

  }

  
  //turn Device 3 ON
  if (message.indexOf("D/3/N") > -1) {
    analogWrite(device_3,  motor_speed);
    delay(1000);
    Serial.println("Command: Device 3 Turn On.");
    Serial.println("for time:"+getValue(message, '/', 1)+"s");
    delay(getValue(message, '/', 1).toInt()*1000);
    analogWrite(device_3,  0);
            Serial.println("for time:"+getValue(message, '/', 1)+"s "+"ended and turn off");

  }

 
  //turn Device 4 ON
  if (message.indexOf("D/4/N") > -1) {
    digitalWrite(device_4, HIGH);
    delay(1000);
    Serial.println("Command: Device 4 Turn On.");
     Serial.println("for time:"+getValue(message, '/', 1)+"s");
    delay(getValue(message, '/', 1).toInt()*1000);
    digitalWrite(device_4, LOW);
            Serial.println("for time:"+getValue(message, '/', 1)+"s "+"ended and turn off");
  }

 
}else{
  if (message.indexOf("D/1/N") > -1) {
    digitalWrite(device_1, HIGH);
    delay(1000);
    Serial.println("Command: Device 1 Turn On.");
  }

  //turn Device 1 OFF
  if (message.indexOf("D/1/F") > -1) {
    digitalWrite(device_1, LOW);
    Serial.println("Command: Device 1 Turn Off.");
  }

  //turn Device 2 ON
  if (message.indexOf("D/2/N") > -1) {
    digitalWrite(device_2, HIGH);
    delay(1000);
    Serial.println("Command: Device 2 Turn On.");
  }

  //turn Device 2 OFF
  if (message.indexOf("D/2/F") > -1) {
    digitalWrite(device_2, LOW);
    Serial.println("Command: Device 1 Turn Off.");
  }
  //turn Device 3 ON
  if (message.indexOf("D/3/N") > -1) {
    // digitalWrite(device_3, HIGH);
      motor_run = motor_speed;
    delay(1000);
    Serial.println("Command: Device 3 Turn On.");
  }

  //turn Device 3 OFF
  if (message.indexOf("D/3/F") > -1) {
    motor_run = 0;
    Serial.println("Command: Device 3 Turn Off.");
  }
  //turn Device 4 ON
  if (message.indexOf("D/4/N") > -1) {
    digitalWrite(device_4, HIGH);
    delay(1000);
    Serial.println("Command: Device 4 Turn On.");
  }

  //turn Device 4 OFF
  if (message.indexOf("D/4/F") > -1) {
    digitalWrite(device_4, LOW);
    Serial.println("Command: Device 4 Turn Off.");
  }
}


if(getValue(message,'/',0) =="TEMP"){
  trshd_temp =getValue(message,'/',1).toInt();
  Serial.println("temp threshold change to  ");
  Serial.println(trshd_temp);
}
if(getValue(message,'/',0) =="SPEED"){
  motor_speed =getValue(message,'/',1).toInt();
  Serial.println("motor speed change to  ");
  Serial.println(motor_speed);
}

  // if(temp > ){
  //    digitalWrite(device_3, HIGH);
  //    Serial.println("Device 3 Turn on by temp.");
  //  }
  delay(50);// Added delay between two readings
}
float get_temp() {
  Vo = analogRead(tempPin);
  R2 = R1 * (1023.0 / Vo - 1.0); // R2= R1*(Vin/Vout - 1)
  logR2 = log(R2);
  T = (1.0 / (c1 + c2 * logR2 + c3 * logR2 * logR2 * logR2));
  return T - 273.15; //độ C
}
String getValue(String data, char separator, int index)
{
  int found = 0;
  int strIndex[] = {0, -1};
  int maxIndex = data.length()-1;

  for(int i=0; i<=maxIndex && found<=index; i++){
    if(data.charAt(i)==separator || i==maxIndex){
        found++;
        strIndex[0] = strIndex[1]+1;
        strIndex[1] = (i == maxIndex) ? i+1 : i;
    }
  }

  return found>index ? data.substring(strIndex[0], strIndex[1]) : "";
}