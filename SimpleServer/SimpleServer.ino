#include <WiServer.h>
#include <string.h>

#define WIRELESS_MODE_INFRA   1
#define WIRELESS_MODE_ADHOC   2

#define ledPin1 5
#define ledPin2 6
#define ledPin3 7


// Wireless configuration parameters ----------------------------------------
unsigned char local_ip[] = {192,168,43,217};	// IP address of WiShield
unsigned char gateway_ip[] = {192,168,43,1};	// router or gateway IP address
unsigned char subnet_mask[] = {255,255,255,0};	// subnet mask for the local network
const prog_char ssid[] PROGMEM = {"Zack"};	// max 32 bytes
unsigned char security_type = 3;	// 0 - open; 1 - WEP; 2 - WPA; 3 - WPA2
// WPA/WPA2 passphrase
const prog_char security_passphrase[] PROGMEM = {"yoloswag"};	// max 64 characters

// WEP 128-bit keys
// sample HEX keys
prog_uchar wep_keys[] PROGMEM = {
  0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d,   // Key 0
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,   // Key 1
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,   // Key 2
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00   // Key 3
};

// setup the wireless mode
// infrastructure - connect to AP
// adhoc - connect to another WiFi device
unsigned char wireless_mode = WIRELESS_MODE_INFRA;

unsigned char ssid_len;
unsigned char security_passphrase_len;

// End of wireless configuration parameters ----------------------------------------


// This is our page serving function that generates web pages
boolean sendPage(char* URL) {
  Serial.print("URL Requested: ");
  Serial.println(URL);
  
  //Check button press
  if (URL[1] == '?' && URL[2] == 'M') //url has a leading /
  {
    // Get which button is pressed:
    Serial.println("Motor Speed: ");
    switch (URL[3])
    {
      case '1':
      // Apply 0% Motor Code
       Serial.println("0%");
       break;
      
      case '2':
      // Apply 20% Motor Code
        Serial.println("20%");
        break;
       
      case '3':
      // Apply 40% Motor Code
       Serial.println("40%");
       break;
     
      case '4':
      // Apply 60% Motor Code
        Serial.println("60%");
        break;
      
      case '5':
      // Apply 80% Motor Code
        Serial.println("80%");
        break;
        
      case '6':
      // Apply 100% Motor Code
        Serial.println("100%");  
        break;
             
    }

    //After executing button code, return the user to the index page.
    WiServer.print("<HTML><HEAD><meta http-equiv='REFRESH' content='0;url=/'></HEAD></HTML>");
    return true;
  }
  
  if (strcmp(URL, "/") == 0)
   {
      WiServer.print("<html><head><title>Boat Mechanics Zack Is The Coolest</title></head>");
   
      WiServer.print("<body><center>Please select motor speed:\n\n ");
      
      // Setup buttons:
      WiServer.print("<form method='get' action=''><input type='submit' name='M1' value='0%'></input></form>");
      WiServer.print("<form method='get' action=''><input type='submit' name='M2' value='20%'></input></form>");
      WiServer.print("<form method='get' action=''><input type='submit' name='M3' value='40%'></input></form>");
      WiServer.print("<form method='get' action=''><input type='submit' name='M4' value='60%'></input></form>");
      WiServer.print("<form method='get' action=''><input type='submit' name='M5' value='80%'></input></form>");
      WiServer.print("<form method='get' action=''><input type='submit' name='M6' value='100%'></input></form>");
      WiServer.print("</center>");
      WiServer.print("</html> ");
      return true;
   }
}

void setup() 
{
  // Initialize Server
  Serial.begin(9600);
  WiServer.init(sendPage);
  WiServer.enableVerboseMode(true);
}

void loop()
{
  // Run WiServer
  WiServer.server_task();
  delay(10);
}



