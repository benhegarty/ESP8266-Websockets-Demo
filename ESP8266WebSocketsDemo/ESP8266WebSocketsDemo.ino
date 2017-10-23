#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <WebSocketsServer.h> 
#include <Hash.h>
#include <FS.h>
#include <Servo.h>

// Update to contain your network information :)
const char *ssid = "Your Network Name";
const char *password = "Your Network Password";

// A few variables which we'll need throughout the program
int16_t thisRead = 0; // The current pot value
int16_t lastRead = 0; // The last pot value (this is used to prevent sending duplicate values)
uint8_t counter = 0;  // Used to limit how often we send pot updates via websockets

// Initialise websockets, web server and servo
WebSocketsServer webSocket = WebSocketsServer(81);
ESP8266WebServer server(80);
Servo myservo;

void setup(void){
  // Wait a second before we jump into starting everything
  delay(1000);
  
  Serial.begin(115200);
  Serial.println();
  Serial.print("Configuring wifi...");

  // If you'd like to use the ESP as a wifi access point instead of a client, 
  // comment out everything between HERE...

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  IPAddress myIP = WiFi.localIP();

  // AND HERE... Then uncomment these 2 lines:
  // WiFi.softAP(ssid);
  // IPAddress myIP = WiFi.softAPIP();

  // Print out the IP address
  Serial.print("IP address: ");
  Serial.println(myIP);

  // Configure our servo
  myservo.attach(D2);

  // Begin access to our file system (which stores the HTML)
  SPIFFS.begin();

  // Start the websockets and link the events function (defined below, under the loop)
  webSocket.begin();
  webSocket.onEvent(webSocketEvent);

  // Configure web server to host HTML files
  server.onNotFound([](){
    if(!handleFileRead(server.uri()))
      server.send(404, "text/plain", "FileNotFound");
  });
  server.begin();
  Serial.println("HTTP server started");
}

void loop(void) {
  // Process any incoming HTTP or WebSockets requests
  webSocket.loop();
  server.handleClient();

  // Every 256 times we run the loop, read the pot and send out the value (if it's changed)
  counter++;
  if (counter == 255) {
    counter = 0; // Reset the counter

    // Read in the pot value and map it so that it is between 0 and 100
    thisRead = map(analogRead(A0), 900, 20, 0, 100);

    // If it is out of range, nudge it back into range
    if (thisRead > 100) thisRead = 100;
    if (thisRead < 0) thisRead = 0;

    // If what we read last time ISN'T what we read last time, send off the new value via WebSockets!
    if (thisRead != lastRead) {
      String message = String(thisRead);
      webSocket.broadcastTXT(message);
    }

    // Update the last read variable
    lastRead = thisRead;
  }
  
}

// A function to handle our incoming sockets messages
void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t lenght) {

  switch(type) {

    // Runs when a user disconnects
    case WStype_DISCONNECTED: {
      Serial.printf("User #%u - Disconnected!\n", num);
      break;
    }
    
    // Runs when a user connects
    case WStype_CONNECTED: {
      IPAddress ip = webSocket.remoteIP(num);
      Serial.printf("--- Connection. IP: %d.%d.%d.%d Namespace: %s UserID: %u\n", ip[0], ip[1], ip[2], ip[3], payload, num);
      
      // Send last pot value on connect
      String message = String(lastRead);
      webSocket.broadcastTXT(message);
      break;
    }

    // Runs when a user sends us a message
    case WStype_TEXT: {
      String incoming = "";
      for (int i = 0; i < lenght; i++) {
        incoming.concat((char)payload[i]);
      }
      uint8_t deg = incoming.toInt();
      myservo.write(deg);  
      break;
    }
    
  }

}

// A function we use to get the content type for our HTTP responses
String getContentType(String filename){
  if(server.hasArg("download")) return "application/octet-stream";
  else if(filename.endsWith(".htm")) return "text/html";
  else if(filename.endsWith(".html")) return "text/html";
  else if(filename.endsWith(".css")) return "text/css";
  else if(filename.endsWith(".js")) return "application/javascript";
  else if(filename.endsWith(".png")) return "image/png";
  else if(filename.endsWith(".gif")) return "image/gif";
  else if(filename.endsWith(".jpg")) return "image/jpeg";
  else if(filename.endsWith(".ico")) return "image/x-icon";
  else if(filename.endsWith(".xml")) return "text/xml";
  else if(filename.endsWith(".pdf")) return "application/x-pdf";
  else if(filename.endsWith(".zip")) return "application/x-zip";
  else if(filename.endsWith(".gz")) return "application/x-gzip";
  return "text/plain";
}

// Takes a URL (for example /index.html) and looks up the file in our file system,
// Then sends it off via the HTTP server!
bool handleFileRead(String path){
  #ifdef DEBUG
    Serial.println("handleFileRead: " + path);
  #endif
  if(path.endsWith("/")) path += "index.html";
  if(SPIFFS.exists(path)){
    File file = SPIFFS.open(path, "r");
    size_t sent = server.streamFile(file, getContentType(path));
    file.close();
    return true;
  }
  return false;
}
