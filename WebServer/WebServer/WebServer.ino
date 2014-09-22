/*
 Web Server 
 A simple web server which controls a remote robot using RS485  

 Circuit:
 * Ethernet shield attached to pins 10, 11, 12, 13
 * RS485 shield RX/TX attached to pins 14,15
 * Motor connected somewhere

 */
 
#include <avr/pgmspace.h>
#include <SPI.h>
#include <Ethernet.h>
#include <String.h>

//VRCSR protocol defines
const char SYNC_REQUEST[] = {0xF5, 0x5F};
const char SYNC_RESPONSE[] = {0x0F,0xF0};
const char PROTOCOL_VRCSR_HEADER_SIZE = 6;
const char PROTOCOL_VRCSR_XSUM_SIZE = 4;
//CSR Address for sending an application specific custom command
const char ADDR_CUSTOM_COMMAND = 0xF0;
/*The command to send.
The Propulsion command has a payload format of:
0xAA R_ID THRUST_0 THRUST_1 THRUST_2 ... THRUST_N
Where:
0xAA is the command byte
R_ID is the NODE ID of the thruster to respond with data
THRUST_X is the thruster power value (-1 to 1) for the thruster with motor id X*/
const char PROPULSION_COMMAND = 0xAA;
//flag for the standard thruster response which contains
const char RESPONSE_THRUSTER_STANDARD = 0x2;
//standard response is the device type followed by 4 32-bit floats and 1 byte
const char RESPONSE_THRUSTER_STANDARD_LENGTH = 1 + 4 * 4 + 1 + 1;
//The proppulsion command packets are typically sent as a multicast to a group ID defined for thrusters
const char THRUSTER_GROUP_ID = 0x81;
//lookup table to speed up checksum computation
static PROGMEM prog_uint32_t crc_table[16] = {
0x00000000, 0x1db71064, 0x3b6e20c8, 0x26d930ac,
0x76dc4190, 0x6b6b51f4, 0x4db26158, 0x5005713c,
0xedb88320, 0xf00f9344, 0xd6d6a3e8, 0xcb61b38c,
0x9b64c2b0, 0x86d3d2d4, 0xa00ae278, 0xbdbdf21c
};

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

float thrust[2] = {0.0,0.0};
char node_id = 100;
boolean excelsior_lyfe = false;

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
  client.println("<br>");
  client.println("<table>");
  client.println("<tr>");
  client.println("<td><form method='get' action=''><input type='submit' name='forward' value='Excelsior!'></input></form></td>");
  client.println("</tr>");
  client.println("</table>");
  
  return;
}

void set_motor_params(char node_id)
{
  
  Serial.println(thrust[0]);
  return;
}

void check_httpcontents(String readString)
{
  
  excelsior_lyfe = false;
  if (readString.indexOf("GET /?M1_0") != -1)
  {
    // Apply Code to set motor 1 to 0% speed
    thrust[0] = 0.00;
    set_motor_params(node_id);
  }
        
  else if (readString.indexOf("GET /?M1_20")!= -1)
  {
    // Apply Code to set motor 1 to 20% speed
    thrust[0] = .1*.2;
    set_motor_params(node_id);
  }
      
  else if (readString.indexOf("GET /?M1_40")!= -1)
  {
    // Apply Code to set motor 1 to 40% speed
    thrust[0] = .1*.4;
    set_motor_params(node_id);
  }
        
  else if (readString.indexOf("GET /?M1_60")!= -1)
  {
    // Apply Code to set motor 1 to 60% speed
    thrust[0] = .1*.6;
    set_motor_params(node_id);
  }
        
  else if (readString.indexOf("GET /?M1_80") != -1)
  {
    // Apply Code to set motor 1 to 80% speed
    thrust[0] = .1*.8;
    set_motor_params(node_id);  
  }
       
  else if (readString.indexOf("GET /?M1_100") != -1)
  {
    // Apply Code to set motor 1 to 100% speed
    thrust[0] = .1;
    set_motor_params(node_id);
  }
  
  else if (readString.indexOf("GET /?M2_0") != -1)
  {
    // Apply Code to set motor 2 to 0% speed
    thrust[1] = 0.00;
    set_motor_params(node_id);
  }
  
  else if (readString.indexOf("GET /?M2_20") != -1)
  {
    // Apply Code to set motor 2 to 20% speed
    thrust[1] = .1*.2;
    set_motor_params(node_id);
  }
  
  else if (readString.indexOf("GET /?M2_40") != -1)
  {
    // Apply Code to set motor 2 to 40% speed
    thrust[1] = .1*.4;
    set_motor_params(node_id);   
  }
  
  else if (readString.indexOf("GET /?M2_60") != -1)
  {
    // Apply Code to set motor 2 to 60% speed
    thrust[1] = .1*.6;
    set_motor_params(node_id);   
  }
  
  else if (readString.indexOf("GET /?M2_80") != -1)
  {
    // Apply Code to set motor 2 to 80% speed
    thrust[1] = .1*.8;
    set_motor_params(node_id); 
  }
  
  else if (readString.indexOf("GET /?M2_100") != -1)
  {
    // Apply Code to set motor 2 to 100% speed
    thrust[1] = .1;
    set_motor_params(node_id);  
  }
  
  else if (readString.indexOf("GET /?forward") != -1)
  {
      excelsior_lyfe = true;
      Serial.println("FORWARD WAS PRESSED");
      set_motors_thrust(node_id,thrust,sizeof(thrust));
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
+         readString = "";
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
   
  if(ELAPSED >= 100 && excelsior_lyfe)
   {
     T_HTTP = T_CUR;
     Serial.println("TIME EXCEEDED, UPDATE MOTOR");
     Serial.print("Thrust[0] = ");
     Serial.println(thrust[0]);
     Serial.print("Thrust[1] = ");
     Serial.println(thrust[1]);
     Serial.println(node_id);
     set_motors_thrust(node_id,thrust,sizeof(thrust));

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
  Serial3.begin(115200);
  // start the Ethernet connection and the server: 
  Ethernet.begin(mac, ip);
  server.begin();
  Serial.print("server is at ");
  Serial.println(Ethernet.localIP());
}

void loop() {
  // Get current time
  T_CUR = millis();
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

//checsum calculation
unsigned long crc_update(unsigned long crc, byte data)
{
byte tbl_idx;
tbl_idx = crc ^ (data >> (0 * 4));
crc = pgm_read_dword_near(crc_table + (tbl_idx & 0x0f)) ^ (crc >> 4);
tbl_idx = crc ^ (data >> (1 * 4));
crc = pgm_read_dword_near(crc_table + (tbl_idx & 0x0f)) ^ (crc >> 4);
return crc;
}
//checksum
unsigned long crc_string(char *s,char len)
{
unsigned long crc = ~0L;
for(char i=0; i < len; i++){
crc = crc_update(crc, *s++);
}
crc = ~crc;
return crc;
}
byte set_motors_thrust(char node_id_respond, float thrust[], int thrust_length) {
//group id for the request packet
char group_id = THRUSTER_GROUP_ID;
unsigned long thrust_long[2];
char flag, CSR_address, payload_length;
//header for packet to be sent. Includes checksum as last 4 bytes
char header[6];
char header_xsum[4];
char payload_xsum[4];
char payload[10];
byte packet[6+4+4+10];
char index= 0;
//checksum and header holders
unsigned long checksum=0;
//Create the custom command packet for setting the power level to a group of thrusters
//generate the header
flag = RESPONSE_THRUSTER_STANDARD;
CSR_address = ADDR_CUSTOM_COMMAND;
payload_length = 2 + thrust_length;
//create the header
header[0] = SYNC_REQUEST[0];
header[1] = SYNC_REQUEST[1];
header[2] = group_id;
header[3] = flag;
header[4] = CSR_address;
header[5] = payload_length;
checksum = crc_string(&header[0], sizeof(header));
header_xsum[3] = checksum >> 24;
header_xsum[2] = (checksum & 0xff0000)>> 16;
header_xsum[1] = (checksum & 0xff00)>> 8;
header_xsum[0] = checksum & 0xff;
payload[0] = PROPULSION_COMMAND;
payload[1] = node_id_respond;
// thrust[0] = thrust_1;
// thrust[1] = thrust_2;
thrust_long[0] = *(long*)&thrust[0];
thrust_long[1] = *(long*)&thrust[1];
payload[5] = thrust_long[0] >> 24;
payload[4] = (thrust_long[0] & 0xff0000)>> 16;
payload[3] = (thrust_long[0] & 0xff00)>> 8;
payload[2] = thrust_long[0] & 0xff;
payload[9] = thrust_long[1] >> 24;
payload[8] = (thrust_long[1] & 0xff0000)>> 16;
payload[7] = (thrust_long[1] & 0xff00)>> 8;
payload[6] = thrust_long[1] & 0xff;
checksum = crc_string(&payload[0], sizeof(payload));
payload_xsum[3] = checksum >> 24;
payload_xsum[2] = (checksum & 0xff0000)>> 16;
payload_xsum[1] = (checksum & 0xff00)>> 8;
payload_xsum[0] = checksum & 0xff;
for(char i=0; i<6; i++){
packet[index] = header[i];
index++;
}
for(char i=0; i<4; i++){
packet[index] = header_xsum[i];
index++;
}
for(char i=0; i<10; i++){
packet[index] = payload[i];
index++;
}
for(char i=0; i<4; i++){
packet[index] =payload_xsum[i];
index++;
}
//send packet
// for (char i=0; i<24; i++){
Serial3.write(packet,sizeof(packet));
//1 if true, in future expand for error checking?
return 1;
// delay(100);
// }
// Serial.println("Start of packet");
// Serial.write(packet[1]);
/* Serial.println(thrust_long[0], HEX);
Serial.println(thrust_long[1], HEX);
for (char i=0; i<24; i++ ){
Serial.print("packet[");
Serial.print(i, HEX);
Serial.print("]\n");
Serial.println(packet[i], HEX);
}
*/
}
