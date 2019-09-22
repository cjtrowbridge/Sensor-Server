#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#ifndef STASSID
#define STASSID "Your Wifi SSID"
#define STAPSK  "Your Wifi Password"
#endif

const char* ssid = STASSID;
const char* password = STAPSK;

ESP8266WebServer server(80);

// Data wire is plugged into port 2 on the NodeMCU v3 Arduino. The IO numbers do not line up correctly to the Pins on the board.
#define ONE_WIRE_BUS 4
#define TEMPERATURE_PRECISION 9

OneWire oneWire(ONE_WIRE_BUS);       // Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
DallasTemperature sensors(&oneWire); // Pass our oneWire reference to Dallas Temperature.
DeviceAddress t0, t1, t2, t3, t4;    // arrays to hold device addresses

// function to print a device address
void printAddress(DeviceAddress deviceAddress)
{
  for (uint8_t i = 0; i < 8; i++)
  {
    // zero pad the address if necessary
    if (deviceAddress[i] < 16) Serial.print("0");
    Serial.print(deviceAddress[i], HEX);
  }
  Serial.println();
}


// function to return the temperature from a specified device
float getTemp(DeviceAddress deviceAddress)
{
  float tempC = sensors.getTempC(deviceAddress);
  return DallasTemperature::toFahrenheit(tempC);
}

String getTemps(){
  sensors.requestTemperatures();
  
  String message = "\"";
  message += getTemp(t0);
  message += "\",\"";
  message += getTemp(t1);
  message += "\",\"";
  message += getTemp(t2);
  message += "\",\"";
  message += getTemp(t3);
  message += "\",\"";
  message += getTemp(t4);
  message += "\"";
  return message;
}

void handleRoot() {

  String data = getTemps();
  
  Serial.println(data);
  
  server.send(200, "text/plain", data);
  
}

void handleNotFound() {
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);

}

void setup(void) {
  Serial.begin(9600);

  Serial.println("Setting up sensors...");
  
  // Start up the library
  sensors.begin();

  // Search for devices on the bus and assign based on an index. Ideally,
  // you would do this to initially discover addresses on the bus and then
  // use those addresses and manually assign them (see above) once you know
  // the devices on your bus (and assuming they don't change).
  //
  // method 1: by index
  if (!sensors.getAddress(t0, 0)) Serial.println("Unable to find address for Device 0");
  if (!sensors.getAddress(t1, 1)) Serial.println("Unable to find address for Device 1");
  if (!sensors.getAddress(t2, 2)) Serial.println("Unable to find address for Device 2");
  if (!sensors.getAddress(t3, 3)) Serial.println("Unable to find address for Device 3");
  if (!sensors.getAddress(t4, 4)) Serial.println("Unable to find address for Device 4");

  // show the addresses we found on the bus
  Serial.print("Device 0 Address: ");
  printAddress(t0);

  Serial.print("Device 1 Address: ");
  printAddress(t1);

  Serial.print("Device 2 Address: ");
  printAddress(t2);

  Serial.print("Device 3 Address: ");
  printAddress(t3);

  Serial.print("Device 4 Address: ");
  printAddress(t4);

  // set the resolution to 9 bit per device
  sensors.setResolution(t0, TEMPERATURE_PRECISION);
  sensors.setResolution(t1, TEMPERATURE_PRECISION);
  sensors.setResolution(t2, TEMPERATURE_PRECISION);
  sensors.setResolution(t3, TEMPERATURE_PRECISION);
  sensors.setResolution(t4, TEMPERATURE_PRECISION);

  Serial.println(getTemps());
  
  Serial.println("Done setting up sensors.");




  //Web server setup stuff
  Serial.println("Setting up web server...");
  
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.print("Connecting to wifi...");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  if (MDNS.begin("esp8266")) {
    Serial.println("MDNS responder started");
  }

  server.on("/", handleRoot);

  server.on("/reset", []() {
    
    //server.send(200, "text/plain", "this works as well");
  });

  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");
  Serial.println("Done setting up web server.");
}

void loop(void) {
  server.handleClient();
  MDNS.update();
}
