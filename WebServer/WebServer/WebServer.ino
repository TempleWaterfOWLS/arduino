/*
  Web Server
 
 A simple web server that shows the value of the analog input pins.
 using an Arduino Wiznet Ethernet shield. 
 
 Circuit:
 * Ethernet shield attached to pins 10, 11, 12, 13
 * Analog inputs attached to pins A0 through A5 (optional)
 
 created 18 Dec 2009
 by David A. Mellis
 modified 9 Apr 2012
 by Tom Igoe
 
 */

#include <SPI.h>
#include <Ethernet.h>
#include <String.h>

// Enter a MAC address and IP address for your controller below.
// The IP address will be dependent on your local network:
byte mac[] = { 
  0x90, 0xA2, 0xDA, 0x0E, 0x02, 0x6E };
IPAddress ip(192,168,0,103);

// Initialize the Ethernet server library
// with the IP address and port you want to use 
// (port 80 is default for HTTP):
EthernetServer server(80);

void setup() {
 // Open serial communications and wait for port to open:
  Serial.begin(9600);
   while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }
  Serial3.begin(9600);

  // start the Ethernet connection and the server:
  Ethernet.begin(mac, ip);
  server.begin();
  Serial.print("server is at ");
  Serial.println(Ethernet.localIP());
}

String readString = String(500);

void loop() {
  // listen for incoming clients
  //Serial3.println("Zack is awesome");
  EthernetClient client = server.available();
  if (client) {
    Serial.println("new client");
    // an http request ends with a blank line
    boolean currentLineIsBlank = true;
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        
        if (readString.length() < 100)
        {
         readString.concat(c); 
        }
        
        
        
        Serial.write(c);
        // if you've gotten to the end of the line (received a newline
        // character) and the line is blank, the http request has ended,
        // so you can send a reply
        if (c == '\n' && currentLineIsBlank) {
          // send a standard http response header
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          client.println();
          client.println("<!DOCTYPE HTML>");
          client.println("<html><head><title>Boat Mechanics Zack Is The Coolest</title></head>");
          client.println("<body><center>Please select motor speed:\n\n ");
          // Setup buttons:
          client.println("<form method='get' action=''><input type='submit' name='M1_0' value='0%'></input></form>");
          client.println("<form method='get' action=''><input type='submit' name='M1_20' value='20%'></input></form>");
          client.println("<form method='get' action=''><input type='submit' name='M1_40' value='40%'></input></form>");
          client.println("<form method='get' action=''><input type='submit' name='M1_60' value='60%'></input></form>");
          client.println("<form method='get' action=''><input type='submit' name='M1_80' value='80%'></input></form>");
          client.println("<form method='get' action=''><input type='submit' name='M1_100' value='100%'></input></form>");
          client.println("</center>");
          client.println("<form method='get' action=''><input type='submit' name='M2_0' value='0%'></input></form>");
          client.println("<form method='get' action=''><input type='submit' name='M2_20' value='20%'></input></form>");
          client.println("<form method='get' action=''><input type='submit' name='M2_40' value='40%'></input></form>");
          client.println("<form method='get' action=''><input type='submit' name='M2_60' value='60%'></input></form>");
          client.println("<form method='get' action=''><input type='submit' name='M2_80' value='80%'></input></form>");
          client.println("<form method='get' action=''><input type='submit' name='M2_100' value='100%'></input></form>");
        if (readString.indexOf("M1_0"))
        {
          // Apply Code to set motor 1 to 0% speed
        }
        
        if (readString.indexOf("M1_20"))
        {
          // Apply Code to set motor 1 to 20% speed
        }
        
        if (readString.indexOf("M1_40"))
        {
          // Apply Code to set motor 1 to 0% speed
        }
        
        if (readString.indexOf("M1_60"))
        {
          // Apply Code to set motor 1 to 0% speed
        }
        
        if (readString.indexOf("M1_80"))
        {
          // Apply Code to set motor 1 to 0% speed
        }
        Serial.println(readString);
        if (readString.indexOf("GET /?M1_100") != -1)
        {
          // Apply Code to set motor 1 to 0% speed
          Serial.println("It works; go home");
          
        }
        readString = "";
          client.println("</html>");
          break;
        }
        if (c == '\n') {
          // you're starting a new line
          currentLineIsBlank = true;
 
        } 
        else if (c != '\r') {
          // you've gotten a character on the current line
          currentLineIsBlank = false;
        }
      }
    }
    // give the web browser time to receive the data
    delay(1);
    // close the connection:
    client.stop();
  }
}

