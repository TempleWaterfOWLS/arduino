#include <SoftwareSerial.h>

//VRCSR protocol defines
#define SYNC_REQUEST  =  0x5FF5;
#define SYNC_RESPONSE =  0x0FF0;
#define PROTOCOL_VRCSR_HEADER_SIZE = 6;
#define PROTOCOL_VRCSR_XSUM_SIZE   = 4;

//CSR Address for sending an application specific custom command
#define ADDR_CUSTOM_COMMAND  = 0xF0;

/*The command to send.
The Propulsion command has a payload format of:
 0xAA R_ID THRUST_0 THRUST_1 THRUST_2 ... THRUST_N 
 Where:
   0xAA is the command byte
   R_ID is the NODE ID of the thruster to respond with data
   THRUST_X is the thruster power value (-1 to 1) for the thruster with motor id X*/
#define PROPULSION_COMMAND   = 0xAA;

//flag for the standard thruster response which contains 
# define RESPONSE_THRUSTER_STANDARD = 0x2;
//standard response is the device type followed by 4 32-bit floats and 1 byte
#define RESPONSE_THRUSTER_STANDARD_LENGTH = 1 + 4 * 4 + 1 + 1; 

//The proppulsion command packets are typically sent as a multicast to a group ID defined for thrusters
#define THRUSTER_GROUP_ID    = 0x81;

SoftwareSerial mySerial(3,2);


void setup() {
  mySerial.begin (9600);
  Serial.begin(9600);
   
  //group id for the request packet
  int group_id;
  group_id = THRUSTER_GROUP_ID;

 /* //motor from which to get response. motors
  int node_id; */
}
 
void loop() {
  mySerial.println ("hello Linksprite!");
  Serial.println ("hello Linksprite!");
  delay(1000);
}
