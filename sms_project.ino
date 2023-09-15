
#include <SoftwareSerial.h>
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
void setup()
{
  Serial.begin(9600); 
  mySerial.begin(9600); 
  pinMode(device_1, OUTPUT);
  pinMode(device_2, OUTPUT); 
  pinMode(device_3, OUTPUT); 
  pinMode(device_4, OUTPUT); 
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
  mySerial.println("AT+CMGL=\"REC UNREAD\""); // Read unread messages
  Serial.println("Ready to received message");
}
void loop()
{
  if (mySerial.available()) {
    delay(100);

    // Serial buffer
    while (mySerial.available()) {
      incomingByte = mySerial.read();
      incomingData += incomingByte;
    }
    delay(10);
    if (atCommand == false) {
      receivedMessage(incomingData);
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
void receivedMessage(String inputString) {
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


  //turn Device 1 ON
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
    digitalWrite(device_3, HIGH);
    delay(1000);
    Serial.println("Command: Device 3 Turn On.");
  }

  //turn Device 3 OFF
  if (message.indexOf("D/3/F") > -1) {
    digitalWrite(device_3, LOW);
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

  delay(50);// Added delay between two readings
}
