#include <SPI.h>
#include <Ethernet.h>

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ip(192, 168, 0, 200);
EthernetServer server(80);

struct Request
{
  String method;
  String page;
  String parameters;
  String params[3];
};

void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  for (int i = 1; i <= 13; i++) {
    pinMode(i, INPUT);
  }

  // start the Ethernet connection and the server:
  Ethernet.begin(mac, ip);
  server.begin();
  Serial.print("server is at " + Ethernet.localIP());
}


void loop() {
  checkIncommingConnections();
}

void checkIncommingConnections() {
  // listen for incoming clients
  EthernetClient client = server.available();
  if (client) {
    Request request = parseRequest(client);
    printHeaders(client);
    processPage(client, request);

    // give the web browser time to receive the data
    delay(1);
    client.stop();
  }
}

void printHeaders(EthernetClient client) {
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: application/json");
  client.println();
}

void processPage(EthernetClient client, Request request) {
  if (request.page == "") {
    client.println("main page");
  } else if (request.page == "status") {
    showSensorsStatus(client, request.params);
  } else if (request.page == "manage") {
    client.println(request.parameters);
    manageSensors(client, request.params);
  } else {
    client.println("Unknown request");
  }
}

void showSensorsStatus(EthernetClient client, String params[]) {
  client.println("Status requested with parameters: ");
}

void manageSensors(EthernetClient client, String params[]) {
  int pinId = getIntParam(params, "pin");
  bool isDigital = getBooleanParam(params, "digital");
  bool isEnabled = getBooleanParam(params, "enabled");

  if (isDigital) {
    digitalWrite(pinId, isEnabled ? HIGH : LOW);
  } else {
    analogWrite(pinId, isEnabled ? HIGH : LOW);
  }
}

Request parseRequest(EthernetClient client) {
  Request request;
  String requestString = "";
  while (client.connected()) {
    if (client.available()) {
      char c = client.read();

      if (c == '\n') {
        break;
      }

      if (c != '\n' && c != '\r') {
        if (c == ' ') {
          if (requestString == "GET" || requestString == "POST") {
            request.method = requestString;
            requestString = "";
          } else {
            int splitCharIndex = requestString.indexOf("?");
            if (splitCharIndex != -1) {
              request.page = requestString.substring(1, splitCharIndex);
              String params = requestString.substring(splitCharIndex + 1, requestString.length());
              request.parameters = params;
              int key = 0;
              while (params.length() > 0) {
                int splitIndex = params.indexOf("&");
                if (splitIndex != -1) {
                  request.params[key] = params.substring(0, splitIndex);
                  params = params.substring(splitIndex + 1, params.length());
                  key++;
                } else {
                  request.params[key] = params;
                  break;
                }
              }
            } else {
              request.page = requestString.substring(1, requestString.length());;
            }
          }
        } else {
          requestString += c;
        }
      }
    }
  }
  return request;
}

int getIntParam(String params[], String paramName) {
  return getStringParam(params, paramName).toInt();
}

bool getBooleanParam(String params[], String paramName) {
  return getStringParam(params, paramName) == "true";
}

String getStringParam(String params[], String paramName) {
  String result = "";
  for (int i = 0; i < 3; i++) {
    if (params[i].indexOf(paramName + "=") != -1) {
      result = params[i].substring(params[i].indexOf("=") + 1, params[i].length());
    }
  }
  return result;
}

String toJSON() {
  String result = "";


  return '{' + result + '}';
}

