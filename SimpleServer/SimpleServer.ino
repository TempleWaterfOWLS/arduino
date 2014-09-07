/*
 * A simple sketch that uses WiServer to serve a web page
 */

#include <WiShield.h>
#include <WiServer.h>

#define WIRELESS_MODE_INFRA	1
#define WIRELESS_MODE_ADHOC	2

// Wireless configuration parameters ----------------------------------------
unsigned char local_ip[] = {192,168,43,217};	// IP address of WiShield
unsigned char gateway_ip[] = {192,168,43,1};	// router or gateway IP address
unsigned char subnet_mask[] = {255,255,255,0};	// subnet mask for the local network
const prog_char ssid[] PROGMEM = {"Zack"};		// max 32 bytes

unsigned char security_type = 3;	// 0 - open; 1 - WEP; 2 - WPA; 3 - WPA2

// WPA/WPA2 passphrase
const prog_char security_passphrase[] PROGMEM = {"yoloswag"};	// max 64 characters

// WEP 128-bit keys
// sample HEX keys
prog_uchar wep_keys[] PROGMEM = { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d,	// Key 0
				  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,	// Key 1
				  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,	// Key 2
				  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00	// Key 3
				};

// setup the wireless mode
// infrastructure - connect to AP
// adhoc - connect to another WiFi device
 unsigned char wireless_mode = WIRELESS_MODE_INFRA;
// unsigned char wireless_mode = WIRELESS_MODE_ADHOC;

unsigned char ssid_len;
unsigned char security_passphrase_len;
// End of wireless configuration parameters ----------------------------------------


// This is our page serving function that generates web pages
boolean sendMyPage(char* URL) {
  
    // Check if the requested URL matches "/"
    if (strcmp(URL, "/") == 0) {
        // Use WiServer's print and println functions to write out the page content
        WiServer.print("<html>");
        WiServer.print("Hello World!");
        WiServer.print("</html>");
        
        // URL was recognized
        return true;
    }
    // URL not found
    return false;
}


void setup() {
  // Initialize WiServer and have it use the sendMyPage function to serve pages
  WiServer.init(sendMyPage);
  
  // Enable Serial output and ask WiServer to generate log messages (optional)
  Serial.begin(57600);
  WiServer.enableVerboseMode(true);
}

void loop(){
  WifiClient client = server.available();
  
  if(client){
    // an http request ends with a blank line
    boolean current_line_is_blank = true;
    while (client.connected()) {
     
      if(client.available()) {
      
        char c = client.read();
        // if we've gotten to the end of the line (received a newline
        // character) and the line is blank, the http request has ended,
        // so we can send a reply
        if (inString.length() < 35) {
            inString.concat(c);
         } 
        if (c == '\n' && current_line_is_blank) {
                    
          // send a standard http response header
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          client.println();
          client.println("<html><body><form method=get>");
          client.println("<p>Led controller</p>");
         
         for(int i=1;i < (numofleds + 1) ;i++){ 
           Led = String("led") + i;
           
          if(inString.indexOf(Led+"=on")>0 || inString.indexOf("all=on")>0){
            Serial.println(Led+"on");
            digitalWrite(led[i], HIGH);
            value[i] = "off"; 
          }else if(inString.indexOf(Led+"=off")>0 || inString.indexOf("all=off")>0 ){          
            Serial.println(Led+"on");
            digitalWrite(led[i], LOW);
            value[i] = "on";
          }
           client.println("<br>"+Led+"  <input type=submit name="+Led+" value="+value[i]+">");
         }
         client.println("<br>All <input type=submit name=all value=on><input type=submit name=all value=off>");
         client.println("</from></html></body>");
          break;
        }
        if (c == '\n') {
          // we're starting a new line
          current_line_is_blank = true;
        } else if (c != '\r') {
          // we've gotten a character on the current line
          current_line_is_blank = false;
        }
      }
    }
    // give the web browser time to receive the data
    delay(1);
    inString = "";
    client.stop();
  }

  // Run WiServer
  WiServer.server_task();
 
  delay(10);
}

