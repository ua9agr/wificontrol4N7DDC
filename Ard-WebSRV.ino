/*
  Arduino Webserver using ESP8266
  modified August 2016
  By Joyson Chinta and Gladson Kujur
  UA9AGR  2021
*/

#define DEBUG false

//////////////gets the data from esp and displays in serial monitor///////////////////////
String sendData(String command, const int timeout, boolean debug)
{
  String response = "";
  Serial.print(command);
  long int time = millis();
  while ( (time + timeout) > millis())
  {
    while (Serial.available())
    {
      char c = Serial.read(); // read the next character.
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
  //Serial.begin(9600);    ///////For Serial monitor
  Serial.begin(115200); ///////ESP Baud rate
    pinMode(LED_BUILTIN,OUTPUT);    /////used if connecting a LED 
    digitalWrite(LED_BUILTIN,1);

  sendData("AT+RST\r\n", 2000, DEBUG); // reset module
  sendData("AT+CWMODE=2\r\n", 1000, DEBUG); // configure as access point
  sendData("AT+CIFSR\r\n", 1000, DEBUG); // get ip address
  sendData("AT+CIPMUX=1\r\n", 1000, DEBUG); // configure for multiple connections
  sendData("AT+CIPSERVER=1,80\r\n", 1000, DEBUG); // turn on server on port 80
}


float sensetemp() ///////function to sense temperature.
{
  int val = analogRead(A0);
  float mv = ( val / 1024.0) * 5000;
  float celcius = mv / 10;
  return (celcius);
}


void loop()
{
  if (Serial.available())
  {
    /////////////////////Recieving from web browser to toggle led
    if (Serial.find("+IPD,")) {
      delay(300);
      connectionId = Serial.read() - 48;
      if (Serial.find("pin=")) {
        //Serial.println("recieving data from web browser");
        int pinNumber = (Serial.read() - 48) * 10;
        pinNumber += (Serial.read() - 48);
        digitalWrite(pinNumber, !digitalRead(pinNumber));
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

//////////////////////////////sends data from ESP to webpage///////////////////////////


