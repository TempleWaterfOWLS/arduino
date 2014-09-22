/*
 Web Server 
 A simple web server which controls a remote robot using RS485  

 Circuit:
 * Ethernet shield attached to pins 10, 11, 12, 13
 * RS485 shield RX/TX attached to pins 14,15
 * Motor connected somewhere

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
long T_CUR = millis();
long T_HTTP = millis();

long ELAPSED;
String motor_speed = String(10);

void render_mainpage(EthernetClient client)
{
  // send a standard http response header
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html");
  client.println();
  client.println("<!DOCTYPE HTML>");
  // Set title
  client.println("<html><head><title>Boat Mechanics Zack Is The Coolest</title>");
  //Declare table style
  client.println("<style>");
  client.println("table,th,td");
  client.println("{");
  client.println("border:1px solid black;");
  client.println("}");
  client.println("</style></head>");
  //Set up body
  client.println("<body><center>Please select motor speed: </center>");
  // Set up buttons:
  client.println("<center>MOTOR 1:");
  client.println("<table>");
  client.println("<tr>");
  // Motor 1 table
  client.println("<td><form method='get' action=''><input type='submit' name='M1_0' value='0%'></input></form></td>");
  client.println("<td><form method='get' action=''><input type='submit' name='M1_20' value='20%'></input></form></td>");
  client.println("<td><form method='get' action=''><input type='submit' name='M1_40' value='40%'></input></form></td>");
  client.println("<td><form method='get' action=''><input type='submit' name='M1_60' value='60%'></input></form></td>");
  client.println("<td><form method='get' action=''><input type='submit' name='M1_80' value='80%'></input></form></td>");
  client.println("<td><form method='get' action=''><input type='submit' name='M1_100' value='100%'></input></form></td>");
  client.println("</tr>");
  client.println("</table>");
  client.println("MOTOR 2: ");
  client.println("<table>");
  client.println("<tr>");
  // Motor 2 table
  client.println("<td><form method='get' action=''><input type='submit' name='M2_0' value='0%'></input></form></td>");
  client.println("<td><form method='get' action=''><input type='submit' name='M2_20' value='20%'></input></form></td>");
  client.println("<td><form method='get' action=''><input type='submit' name='M2_40' value='40%'></input></form></td>");
  client.println("<td><form method='get' action=''><input type='submit' name='M2_60' value='60%'></input></form></td>");
  client.println("<td><form method='get' action=''><input type='submit' name='M2_80' value='80%'></input></form></td>");
  client.println("<td><form method='get' action=''><input type='submit' name='M2_100' value='100%'></input></form></td>");
  client.println("</tr>");
  client.println("</table>");
  // Forward Button
  client.println("<form method='get' action=''><input type='submit' name='forward' value='excelsior!'></input></form>");
  
  
  return;
}

void set_motor_params(String motor_speed)
{
  Serial.println(motor_speed);
  return;
}

void check_httpcontents(String readString)
{

  if (readString.indexOf("GET /?M1_0") != -1)
  {
    // Apply Code to set motor 1 to 0% speed
    motor_speed = "M1_0";
    set_motor_params(motor_speed);
  }
        
  else if (readString.indexOf("GET /?M1_20")!= -1)
  {
    // Apply Code to set motor 1 to 20% speed
    motor_speed = "M1_20";
    set_motor_params(motor_speed);
  }
      
  else if (readString.indexOf("GET /?M1_40")!= -1)
  {
    // Apply Code to set motor 1 to 40% speed
    motor_speed = "M1_40";
    set_motor_params(motor_speed);
  }
        
  else if (readString.indexOf("GET /?M1_60")!= -1)
  {
    // Apply Code to set motor 1 to 60% speed
    motor_speed = "M1_60";
    set_motor_params(motor_speed);
  }
        
  else if (readString.indexOf("GET /?M1_80") != -1)
  {
    // Apply Code to set motor 1 to 80% speed
    motor_speed = "M1_80";
    set_motor_params(motor_speed);    
  }
       
  else if (readString.indexOf("GET /?M1_100") != -1)
  {
    // Apply Code to set motor 1 to 100% speed
    motor_speed = "M1_100";
    set_motor_params(motor_speed);
  }
  
  else if (readString.indexOf("GET /?M2_0") != -1)
  {
    // Apply Code to set motor 2 to 0% speed
    motor_speed = "M2_0";
    set_motor_params(motor_speed);
  }
  
  else if (readString.indexOf("GET /?M2_20") != -1)
  {
    // Apply Code to set motor 2 to 20% speed
    motor_speed = "M2_20";
    set_motor_params(motor_speed); 
  }
  
  else if (readString.indexOf("GET /?M2_40") != -1)
  {
    // Apply Code to set motor 2 to 40% speed
    motor_speed = "M2_40";
    set_motor_params(motor_speed);    
  }
  
  else if (readString.indexOf("GET /?M2_60") != -1)
  {
    // Apply Code to set motor 2 to 60% speed
    motor_speed = "M2_60";
    set_motor_params(motor_speed);    
  }
  
  else if (readString.indexOf("GET /?M2_80") != -1)
  {
    // Apply Code to set motor 2 to 80% speed
    motor_speed = "M2_80";
    set_motor_params(motor_speed);    
  }
  
  else if (readString.indexOf("GET /?M2_100") != -1)
  {
    // Apply Code to set motor 2 to 100% speed
    motor_speed = "M2_100";
    set_motor_params(motor_speed);    
  }
  return;
}

void get_requests(EthernetClient client)
{
  String readString = String(100);
  boolean currentLineIsBlank = true;
  while (client.connected()) {
    if (client.available()) {
      char c = client.read();  
       if (readString.length() < 100)
       {
         readString.concat(c); 
       }
       // if you've gotten to the end of the line (received a newline
       // character) and the line is blank, the http request has ended,
       // so you can send a reply
       if (c == '\n' && currentLineIsBlank) 
        {
         render_mainpage(client);
         Serial.println("check_httpcontents");
         Serial.println(readString);
         check_httpcontents(readString);
         T_HTTP = millis();
         readString = "";
         client.println("</html>");
         break;
        }
        if (c == '\n') 
        {
          // you're starting a new line
          currentLineIsBlank = true;
 
        } 
        else if (c != '\r') 
        {
          // you've gotten a character on the current line
          currentLineIsBlank = false;
        }
      }
    }
    return;
}

void update_motors(void)
{
  ELAPSED = T_CUR - T_HTTP;
   
  if(ELAPSED >= 800)
   {
     T_HTTP = T_CUR;
     set_motor_params(motor_speed);
     Serial.println("TIME EXCEEDED, UPDATE MOTOR");
   } 
  else if(T_HTTP == T_CUR)
  {
    return;
  }
  else
  {
    return;
  }
}

void setup()
{
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  // Initialize motor RX/TX pins
  Serial3.begin(9600);
  // start the Ethernet connection and the server: 
  Ethernet.begin(mac, ip);
  server.begin();
  Serial.print("server is at ");
  Serial.println(Ethernet.localIP());
}

void loop() {
  // Get current time
  T_CUR = millis()
  // listen for incoming clients
  EthernetClient client = server.available();
  if (client) 
  {
    Serial.println("new client");
    get_requests(client);
  }
  update_motors();
  // give the web browser time to receive the data
  delay(1);
  // close the connection:
  client.stop();
}


