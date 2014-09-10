#include <SoftwareSerial.h>
#include <avr/pgmspace.h>
 
//VRCSR protocol defines
const char SYNC_REQUEST[]  = {0x5F, 0xF5};
const char SYNC_RESPONSE[] =  {0x0F,0xF0};
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

static PROGMEM prog_uint32_t crc_table[16] = {
    0x00000000, 0x1db71064, 0x3b6e20c8, 0x26d930ac,
    0x76dc4190, 0x6b6b51f4, 0x4db26158, 0x5005713c,
    0xedb88320, 0xf00f9344, 0xd6d6a3e8, 0xcb61b38c,
    0x9b64c2b0, 0x86d3d2d4, 0xa00ae278, 0xbdbdf21c
};

unsigned long crc_update(unsigned long crc, byte data)
{
    byte tbl_idx;
    tbl_idx = crc ^ (data >> (0 * 4));
    crc = pgm_read_dword_near(crc_table + (tbl_idx & 0x0f)) ^ (crc >> 4);
    tbl_idx = crc ^ (data >> (1 * 4));
    crc = pgm_read_dword_near(crc_table + (tbl_idx & 0x0f)) ^ (crc >> 4);
    return crc;
}

unsigned long crc_string(char *s)
{
  unsigned long crc = ~0L;
  while (*s)
    crc = crc_update(crc, *s++);
  crc = ~crc;
  return crc;
}

SoftwareSerial mySerial(3,2);

void setup() {
  mySerial.begin (9600);
  Serial.begin(9600);
}
 
void loop() {
  //group id for the request packet
  char group_id = THRUSTER_GROUP_ID;
  //motor from which to get response. motors
  char node_id = 0;
  //thrust. is a percentage of power from -1 to 1. only 2 motors on bus.
  float thrust[2];
  char flag, CSR_address, payload_length;
  
  //header for packet to be sent. Includes checksum as last 4 bytes
  char header[6];
  
  //checksum and header holders
  unsigned long header_checksum=0;

  //Create the custom command packet for setting the power level to a group of thrusters
  //generate the header
  flag = RESPONSE_THRUSTER_STANDARD;
  CSR_address = ADDR_CUSTOM_COMMAND;
  payload_length = 2 + sizeof(thrust);
  
  //create the header
  header[0] = SYNC_REQUEST[0];
  header[1] = SYNC_REQUEST[1];
  header[2] = group_id;
  header[3] = flag;
  header[4] = CSR_address;
  header[5] = payload_length;
  
  header_checksum = crc_string(header); 
 /* 
  header[6] = header_checksum >> 24;
  header[7] = (header_checksum & 0xff0000)>> 16;
  header[8] = (header_checksum & 0xff00)>> 8;
  header[9] = header_checksum & 0xff;
  //header_checksum = bytearray(struct.pack('I', binascii.crc32(header))) 
 */
  mySerial.println ("hello Linksprite!");
  Serial.println (header_checksum,HEX);

}
