/*
  Arduino Webserver using ESP8266
  modified August 2016
  By Joyson Chinta and Gladson Kujur
  UA9AGR  2021
*/

#define DEBUG true

#include <SoftwareSerial.h>

SoftwareSerial espSerial(2, 3); // RX, TX

//////////////gets the data from esp and displays in serial monitor///////////////////////
String sendData(String command, const int timeout, boolean debug)
{
  String response = "";
  espSerial.print(command);
  long int time = millis();
  while ( (time + timeout) > millis())
  {
    while (espSerial.available())
    {
      char c = espSerial.read(); // read the next character.
      response += c;
    }
  }

  if (debug)
  {
    Serial.print(response); //displays the esp response messages in arduino Serial monitor
  }
  return response;
}

int connectionId;
void espsend(String d)
{
  String cipSend = " AT+CIPSEND=";
  cipSend += connectionId;
  cipSend += ",";
  cipSend += d.length();
  cipSend += "\r\n";
  sendData(cipSend, 1000, DEBUG);
  sendData(d, 1000, DEBUG);
}

void setup()
{
  Serial.begin(115200);    ///////For Serial monitor
  espSerial.begin(115200); ///////ESP Baud rate - soft serial
  
    pinMode(LED_BUILTIN,OUTPUT);    /////used if connecting a LED 
    digitalWrite(LED_BUILTIN,1);

  sendData("AT+RST\r\n", 2000, DEBUG); // reset module
  sendData("AT+CWMODE=2\r\n", 1000, DEBUG); // configure as access point
  sendData("AT+CIFSR\r\n", 1000, DEBUG); // get ip address
  sendData("AT+CIPMUX=1\r\n", 1000, DEBUG); // configure for multiple connections
  sendData("AT+CIPSERVER=1,80\r\n", 1000, DEBUG); // turn on server on port 80
}


void loop()
{
  if (Serial.available())
  {
    /////////////////////Recieving from web browser to toggle led
    if (espSerial.find("+IPD,")) {
      delay(300);
      connectionId = espSerial.read() - 48;
      if (espSerial.find("pin=")) {
        Serial.print("recieving URL. id: ");
        Serial.println(connectionId);
        int pinNumber = (Serial.read() - 48) * 10;
        pinNumber += (Serial.read() - 48);
        digitalWrite(pinNumber, !digitalRead(pinNumber));
        Serial.print("swithing pin: ");
        Serial.println(pinNumber);
      }
      
    }
    /////////////////////Sending data to browser
    else
    {
      String webpage = "<h1>PIN states:</h1>";
      espsend(webpage);
    }


    


    String add1 = "<h4>FWD (a3)= </h4>";
    String two =  String(analogRead(A3));
    add1 += two;
    add1 += "<br>"; //////////Hex code for degree celcius
    espsend(add1);

    add1 = "<h4>C1 (d2)= </h4>";
    two =  String(digitalRead(2));
    add1 += two;
    add1 += "<br>"; //////////Hex code for degree celcius
    espsend(add1);


    String closeCommand = "AT+CIPCLOSE=";  ////////////////close the socket connection////esp command
    closeCommand += connectionId; // append connection id
    closeCommand += "\r\n";
    sendData(closeCommand, 3000, DEBUG);
  }
}

