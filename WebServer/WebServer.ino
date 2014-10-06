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
//#include <String.h>
#include <ros.h>
#include <std_msgs/String.h>

//ROS defines
//ros::NodeHandle nh;
//std_msgs::String str_msg;
//ros::Publisher motor_response("motor_response", &str_msg);


//setup ros node and publisher
//ros::Publisher pub_humidity("humidity_sensor", &humidity_msg);
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
const static PROGMEM prog_uint32_t crc_table[16] = {
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
String motor_response = String();
String motor_speed = String(10);
float thrust[2] = {0.0,0.0};
//char node_id = 100;
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
  client.println("<td><form method='get' action=''><input type='submit' name='inv_M1_100' value='-100%'></input></form></td>");
  client.println("<td><form method='get' action=''><input type='submit' name='inv_M1_80' value='-80%'></input></form></td>");
  client.println("<td><form method='get' action=''><input type='submit' name='inv_M1_60' value='-60%'></input></form></td>");
  client.println("<td><form method='get' action=''><input type='submit' name='inv_M1_40' value='-40%'></input></form></td>");
  client.println("<td><form method='get' action=''><input type='submit' name='inv_M1_20' value='-20%'></input></form></td>");
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
  client.println("<td><form method='get' action=''><input type='submit' name='inv_M2_100' value='-100%'></input></form></td>");
  client.println("<td><form method='get' action=''><input type='submit' name='inv_M2_80' value='-80%'></input></form></td>");
  client.println("<td><form method='get' action=''><input type='submit' name='inv_M2_60' value='-60%'></input></form></td>");
  client.println("<td><form method='get' action=''><input type='submit' name='inv_M2_40' value='-40%'></input></form></td>");
  client.println("<td><form method='get' action=''><input type='submit' name='inv_M2_20' value='-20%'></input></form></td>");
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
  client.println("<td><form method='get' action=''><input type='submit' name='stop' value='stop'></input></form></td>");
  client.println("<td><form method='get' action=''><input type='submit' name='reverse' value='reverse'></input></form></td>");
  client.println("</tr>");
  client.println("</br>"); /*
  client.println("<form action=''>");
  client.println('First name: <input type="text" name="fname"><br>');
  client.println('Last name: <input type="text" name="lname"><br>');
  client.println('<input type="submit" value="Submit">');
  client.println("</form>"); */
  client.println("</table>");
  client.println("</html>");
  return;
}

boolean check_httpcontents(String readString)
{
  
  if (readString.indexOf("GET /?M1_0") != -1)
  {
    // Apply Code to set motor 1 to 0% speed
    thrust[0] = 0.00;
  }
        
  else if (readString.indexOf("GET /?M1_20")!= -1)
  {
    // Apply Code to set motor 1 to 20% speed
    thrust[0] = .1*.2;
  }
      
  else if (readString.indexOf("GET /?M1_40")!= -1)
  {
    // Apply Code to set motor 1 to 40% speed
    thrust[0] = .1*.4;
  }
        
  else if (readString.indexOf("GET /?M1_60")!= -1)
  {
    // Apply Code to set motor 1 to 60% speed
    thrust[0] = .1*.6;
  }
        
  else if (readString.indexOf("GET /?M1_80") != -1)
  {
    // Apply Code to set motor 1 to 80% speed
    thrust[0] = .1*.8;
  }
       
  else if (readString.indexOf("GET /?M1_100") != -1)
  {
    // Apply Code to set motor 1 to 100% speed
    thrust[0] = .1;
  }
  
  else if (readString.indexOf("GET /?M2_0") != -1)
  {
    // Apply Code to set motor 2 to 0% speed
    thrust[1] = 0.00;
  }
  
  else if (readString.indexOf("GET /?M2_20") != -1)
  {
    // Apply Code to set motor 2 to 20% speed
    thrust[1] = .1*.2;
  }
  
  else if (readString.indexOf("GET /?M2_40") != -1)
  {
    // Apply Code to set motor 2 to 40% speed
    thrust[1] = .1*.4;  
  }
  
  else if (readString.indexOf("GET /?M2_60") != -1)
  {
    // Apply Code to set motor 2 to 60% speed
    thrust[1] = .1*.6;  
  }
  
  else if (readString.indexOf("GET /?M2_80") != -1)
  {
    // Apply Code to set motor 2 to 80% speed
    thrust[1] = .1*.8;
  }
  
  else if (readString.indexOf("GET /?M2_100") != -1)
  {
    // Apply Code to set motor 2 to 100% speed
    thrust[1] = .1;  
  }
  
  else if (readString.indexOf("GET /?forward") != -1)
  {
      T_HTTP = millis();
      excelsior_lyfe = true;
      set_motors_thrust(0,thrust,sizeof(thrust));
  }

  else if (readString.indexOf("GET /?stop") != -1)
  {
      thrust[0] = 0;
      thrust[1] = 0;  
  }
       else if (readString.indexOf("GET /?inv_M1_20") != -1)
  {
    // Apply Code to set motor 1 to -20% speed
    thrust[0] = -1*.1*.2;
  }
  
  else if (readString.indexOf("GET /?inv_M1_40") != -1)
  {
    // Apply Code to set motor 1 to -40% speed
    thrust[0] = -1*.1*.4;  
  }
  
  else if (readString.indexOf("GET /?inv_M1_60") != -1)
  {
    // Apply Code to set motor 1 to -60% speed
    thrust[0] = -1*.1*.6;  
  }
  
  else if (readString.indexOf("GET /?inv_M1_80") != -1)
  {
    // Apply Code to set motor 1 to -80% speed
    thrust[0] = -1*.1*.8;
  }
  
  else if (readString.indexOf("GET /?inv_M1_100") != -1)
  {
    // Apply Code to set motor 1 to -100% speed
    thrust[0] = -1*.1;  
  }
  
    else if (readString.indexOf("GET /?inv_M2_20") != -1)
  {
    // Apply Code to set motor 2 to -20% speed
    thrust[1] = -1*.1*.2;
  }
  
  else if (readString.indexOf("GET /?inv_M2_40") != -1)
  {
    // Apply Code to set motor 2 to -40% speed
    thrust[1] = -1*.1*.4;  
  }
  
  else if (readString.indexOf("GET /?inv_M2_60") != -1)
  {
    // Apply Code to set motor 2 to -60% speed
    thrust[1] = -1*.1*.6;  
  }
  
  else if (readString.indexOf("GET /?inv_M2_80") != -1)
  {
    // Apply Code to set motor 2 to -80% speed
    thrust[1] = -1*.1*.8;
  }
  
  else if (readString.indexOf("GET /?inv_M2_100") != -1)
  {
    // Apply Code to set motor 2 to -100% speed
    thrust[1] = -1*.1;  
  }
 
  else if (readString.indexOf("GET /?reverse") != -1)
  {
    if (thrust[0] == 0 && thrust[1] == 0)  
    {
      thrust[0] = -.08;
      thrust[1] = -.08;
    }
    else
    {
      thrust[0] = -1 * thrust[0];
      thrust[1] = -1 * thrust[1];
    }
  }
  
  else if(readString.indexOf("/pythoninfo") != -1)
  {
    return true;
  }
  
  return false;
}

void get_requests(EthernetClient client)
{
  int string_iterator;
  String readString = String(100);
  boolean currentLineIsBlank = true; 
  boolean python_info = false;
  
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
         Serial.println("check_httpcontents");
         Serial.println(readString);
         python_info = check_httpcontents(readString);
         // If /pythoninfo was accessed 
         if (python_info) 
         {
           // Print each element of the string to the client
           client.println(thrust[0]);
           client.println(thrust[1]);
           for (string_iterator = 0; string_iterator < motor_response.length(); string_iterator++)
           {
             client.println(motor_response[string_iterator],HEX);
           }
            
         }
         // Otherwise, render the homepage 
         else
         {
         render_mainpage(client);
         }
         readString = "";
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
     set_motors_thrust(0,thrust,sizeof(thrust));
  } 
}

//asks both motor controllers for information and prints output
void get_motor_condition(){
  char node_id[]={0,1};
  char response;
  int t_int_1 = int(thrust[0]*100);
  int t_int_2 = int(thrust[1]*100);
  
  motor_response = ""; 
  //get information from motor controller node 0
  set_motors_thrust(node_id[0],thrust,sizeof(thrust));
  delay(100);
  
  if (Serial3.available())
  {
    Serial.println("Data from motorC node 0");
    while (Serial3.available()>0)
    {
        response= Serial3.read();	//read Serial        
        Serial.print(response, HEX);
        motor_response.concat(response);
    }
  }
  
  //get information from motor controller node 1
  set_motors_thrust(node_id[1],thrust,sizeof(thrust));
  
  delay(100);
  if (Serial3.available())
  {
    Serial.println("Data from motorC node 1");
    while (Serial3.available()>0)
    {
        response= Serial3.read();	//read Serial        
        Serial.println(response, HEX);
        // Write response to global storage variable
        motor_response.concat(response);
    }
  }
  Serial.println("New stored array is: ");
  int ite;
  for (ite = 0; ite<motor_response.length();ite++)
  {
  Serial.print(motor_response[ite],HEX);
  Serial.print('-');
  }
  Serial.print("\n");

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
 
  //begin ros node
//  nh.initNode();
  //begin ros topic
//  nh.advertise(motor_response);
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
  if (excelsior_lyfe) get_motor_condition();
/*  str_msg.data="hello";
  motor_response.publish(&str_msg);
  nh.spinOnce(); */
  // give the web browser time to receive the data
  delay(10);
  client.stop();
  // close the connection:
  
}

//checksum calculation
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
  Serial3.write(packet,sizeof(packet));
  
  //1 if true, in future expand for error checking?
  return 1;

}
