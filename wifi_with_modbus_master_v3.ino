#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

/* ----------------------MODBUS CONFIG------------------------------*/
#include <ModbusRTU.h>
#include <SoftwareSerial.h>

SoftwareSerial S(D7, D8);

ModbusRTU mb;

bool cbWrite(Modbus::ResultCode event, uint16_t transactionId, void* data) {
  // Serial.printf_P("Request result: 0x%02X, Mem: %d\n", event, ESP.getFreeHeap());
  return true;
}


bool coils[4];
bool temp_coils[4];
bool discInputs[2];
bool temp_discInputs[2];
bool ledStatus = false;
bool changePoll = false;
bool value = false;
int address = 0;
bool writeBuffer = false;
bool refreshPage = false;


/* ----------------------MODBUS CONFIG------------------------------*/

/* Put your SSID & Password */
const char* ssid = "Automate It";  // Enter SSID here
const char* password = "12345678";  //Enter Password here

/* Put IP Address details */
IPAddress local_ip(192,168,1,1);
IPAddress gateway(192,168,1,1);
IPAddress subnet(255,255,255,0);

ESP8266WebServer server(80);

void setup() {
  // Serial.begin(9600);
  /* ------------------MODBUS CONFIG STARTS-----------------*/
  S.begin(9600, SWSERIAL_8N1);
  mb.begin(&S);
  mb.master();
  for(int i = 0; i < 4; i++) {
    coils[i] = false;
  }
  for(int i = 0; i < 4; i++) {
    temp_coils[i] = false;
  }
  for(int i = 0; i < 2; i++) {
    discInputs[i] = false;
  }
  for(int i = 0; i < 2; i++) {
    temp_discInputs[i] = false;
  }
  // pinMode(16, OUTPUT);
  // digitalWrite(16, HIGH);
  pinMode(2, OUTPUT);
  digitalWrite(2, HIGH);
  /* ------------------MODBUS CONFIG ENDS-----------------*/

  WiFi.softAP(ssid, password);
  WiFi.softAPConfig(local_ip, gateway, subnet);
  delay(100);
  
  server.on("/", handle_OnConnect);
  server.on("/Relay1on", handle_Relay1on);
  server.on("/Relay1off", handle_Relay1off);
  server.on("/Relay2on", handle_Relay2on);
  server.on("/Relay2off", handle_Relay2off);
  server.on("/Relay3on", handle_Relay3on);
  server.on("/Relay3off", handle_Relay3off);
  server.on("/Relay4on", handle_Relay4on);
  server.on("/Relay4off", handle_Relay4off);
  
  server.onNotFound(handleNotFound);
  
  server.begin();
}

void loop() {
  server.handleClient();

  if(!writeBuffer) {
    if(changePoll) {
      changePoll = false;
      if (!mb.slave()) {
        mb.readCoil(1, 0, coils, 4, cbWrite);
        delay(100);
        while(mb.slave()) { // Check if transaction is active
          mb.task();
          delay(10);
        }
      }
    if(refreshPage) {
      refreshPage = false;
      server.send(200, "text/html", SendHTML());
    }
    }
    else {
      changePoll = true;
      if (!mb.slave()) {
        mb.readIsts(1, 0, discInputs, 2, cbWrite);
        while(mb.slave()) { // Check if transaction is active
          mb.task();
          delay(10);
        }
        delay(100);
      }
    }
  }
  else {
    writeBuffer = false;
    if (!mb.slave()) {
      mb.writeCoil(1, address, value, cbWrite);
      while(mb.slave()) { // Check if transaction is active
        mb.task();
        delay(10);
      }
      delay(100);
    }
  }

  // mb.task();
  // yield();
}

void handle_OnConnect() {
  server.send(200, "text/html", SendHTML());
}

void handle_Relay1on() {
  address = 0;
  value = false;
  // coils[0] = false;
  writeBuffer = true;
  refreshPage = true;
  // server.send(200, "text/html", SendHTML());
}

void handle_Relay1off() {
  address = 0;
  value = true;
  // coils[0] = true;
  writeBuffer = true;
  refreshPage = true;
  // server.send(200, "text/html", SendHTML());
}

void handle_Relay2on() {
  address = 1;
  value = false;
  // coils[1] = false;
  writeBuffer = true;
  refreshPage = true;
  // server.send(200, "text/html", SendHTML());
}

void handle_Relay2off() {
  address = 1;
  value = true;
  // coils[1] = true;
  writeBuffer = true;
  refreshPage = true;
  // server.send(200, "text/html", SendHTML());
}

void handle_Relay3on() {
  address = 2;
  value = false;
  // coils[2] = false;
  writeBuffer = true;
  refreshPage = true;
  // server.send(200, "text/html", SendHTML());
}

void handle_Relay3off() {
  address = 2;
  value = true;
  // coils[2] = true;
  writeBuffer = true;
  refreshPage = true;
  // server.send(200, "text/html", SendHTML());
}

void handle_Relay4on() {
  address = 3;
  value = false;
  // coils[3] = false;
  writeBuffer = true;
  refreshPage = true;
  // server.send(200, "text/html", SendHTML());
}

void handle_Relay4off() {
  address = 3;
  value = true;
  // coils[3] = true;
  writeBuffer = true;
  refreshPage = true;
  // server.send(200, "text/html", SendHTML());
}

void handleNotFound() {
  server.send(404, "text/plain", "Not found");
}

String SendHTML() {
  String html = "<!DOCTYPE html><html>\n";
  html += "<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, user-scalable=no\">\n";
  html += "<title>Automate It</title>\n";
  html += "<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}\n";
  html += "body{margin-top: 50px;} h1 {color: #444444;margin: 50px auto 30px;} h3 {color: #444444;margin-bottom: 50px;}\n";
  html += ".button {display: block;width: 80px;background-color: #1abc9c;border: none;color: white;padding: 13px 30px;text-decoration: none;font-size: 25px;margin: 0px auto 35px;cursor: pointer;border-radius: 4px;}\n";
  html += ".button-on {background-color: #1abc9c;}\n";
  html += ".button-on:active {background-color: #16a085;}\n";
  html += ".button-off {background-color: #34495e;}\n";
  html += ".button-off:active {background-color: #2c3e50;}\n";
  html += "p {font-size: 14px;color: #888;margin-bottom: 10px;}\n";
  html += "</style>\n";
  html += "</head>\n";
  html += "<body>\n";
  html += "<h1>Automate It</h1>\n";
  html += "<h3>Relay Status</h3>\n";
  
  // Button for Relay 1
  html += "<p>Relay 1</p>";
  if (!coils[0]) {
    html += "<a class=\"button button-on\" href=\"/Relay1off\">OFF</a>\n";
    html += "<p>Relay 1 is ON</p>";
  } else {
    html += "<a class=\"button button-off\" href=\"/Relay1on\">ON</a>\n";
    html += "<p>Relay 1 is OFF</p>";
  }

  // Button for Relay 2
  html += "<p>Relay 2</p>";
  if (!coils[1]) {
    html += "<a class=\"button button-on\" href=\"/Relay2off\">OFF</a>\n";
    html += "<p>Relay 2 is ON</p>";
  } else {
    html += "<a class=\"button button-off\" href=\"/Relay2on\">ON</a>\n";
    html += "<p>Relay 2 is OFF</p>";
  }

  // Button for Relay 3
  html += "<p>Relay 3</p>";
  if (!coils[2]) {
    html += "<a class=\"button button-on\" href=\"/Relay3off\">OFF</a>\n";
    html += "<p>Relay 3 is ON</p>";
  } else {
    html += "<a class=\"button button-off\" href=\"/Relay3on\">ON</a>\n";
    html += "<p>Relay 3 is OFF</p>";
  }

  // Button for Relay 4
  html += "<p>Relay 4</p>";
  if (!coils[3]) {
    html += "<a class=\"button button-on\" href=\"/Relay4off\">OFF</a>\n";
    html += "<p>Relay 4 is ON</p>";
  } else {
    html += "<a class=\"button button-off\" href=\"/Relay4on\">ON</a>\n";
    html += "<p>Relay 4 is OFF</p>";
  }
  
  html += "<h3>Input Signal Status</h3>\n";
  
  // Button for Input Signal 1
  html += "<p>Input Signal 1</p>";
  if (discInputs[0]) {
    html += "<p>Input Signal 1 is ON</p>";
  } else {
    html += "<p>Input Signal 1 is OFF</p>";
  }

  // Button for Input Signal 2
  html += "<p>Input Signal 2</p>";
  if (discInputs[1]) {
    html += "<p>Input Signal 2 is ON</p>";
  } else {
    html += "<p>Input Signal 2 is OFF</p>";
  }
  
  html += "</body>\n";
  html += "</html>\n";
  
  return html;
}
