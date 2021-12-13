#include <Arduino.h>
#include <ESPAsyncWebServer.h>
#include <SPIFFS.h>
//#include "http_server.h"
#include "ArduinoJson.h"

const char *ssid = "myhuawei";
const char *password = "koffi123";
String recus;
const int led = 2;
const int capteurLuminosite = 34;

AsyncWebServer server(80);

void setup()
{
  //----------------------------------------------------Serial
  Serial.begin(115200);
  Serial.println("\n");

  //----------------------------------------------------GPIO
  pinMode(led, OUTPUT);
  digitalWrite(led, LOW);
  pinMode(capteurLuminosite, INPUT);

  //----------------------------------------------------SPIFFS
  if(!SPIFFS.begin())
  {
    Serial.println("Erreur SPIFFS...");
    return;
  }

  File root = SPIFFS.open("/");
  File file = root.openNextFile();

  while(file)
  {
    Serial.print("File: ");
    Serial.println(file.name());
    file.close();
    file = root.openNextFile();
  }

  //----------------------------------------------------WIFI
  WiFi.begin(ssid, password);
  Serial.print("Tentative de connexion...");
  
  while(WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(100);
  }
  
  Serial.println("\n");
  Serial.println("Connexion etablie!");
  Serial.print("Adresse IP: ");
  Serial.println(WiFi.localIP());

  //----------------------------------------------------SERVER
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
  {
    int paramsNr = request->params();
    Serial.println(paramsNr);
    for(int i=0;i<paramsNr;i++){
        AsyncWebParameter* p = request->getParam(i);
        Serial.print("Param name: ");
        Serial.println(p->name());
        Serial.print("Param value: ");
        Serial.println(p->value());
        Serial.println("------");
    }
    request->send(SPIFFS, "/index.html", "text/html");
  });
  
  server.on("/w3.css", HTTP_GET, [](AsyncWebServerRequest *request)
  {
    request->send(SPIFFS, "/w3.css", "text/css");
  });
  server.on("/logo.png", HTTP_GET, [](AsyncWebServerRequest *request)
  {
    request->send(SPIFFS, "/logo.png");
  });
  server.on("/kids.jpg", HTTP_GET, [](AsyncWebServerRequest *request)
  {
    request->send(SPIFFS, "/kids.jpg");
  });
  server.on("/solution.jpg", HTTP_GET, [](AsyncWebServerRequest *request)
  {
    request->send(SPIFFS, "/solution.jpg");
  });
  server.on("/vache.png", HTTP_GET, [](AsyncWebServerRequest *request)
  {
    request->send(SPIFFS, "/vache.png");
  });
  server.on("/chacal.png", HTTP_GET, [](AsyncWebServerRequest *request)
  {
    request->send(SPIFFS, "/chacal.png");
  });
  server.on("/script.js", HTTP_GET, [](AsyncWebServerRequest *request)
  {
    request->send(SPIFFS, "/script.js", "text/javascript");
  });

/*
* server.on("/select2", HTTP_GET, [](AsyncWebServerRequest *request)
  {
    request->send(200, "text/plain", luminosite);
  });
* server.on("/save", HTTP_GET, [](AsyncWebServerRequest *request)
  {
    request->send(200);
  });
*/
  server.on("/lireLuminosite", HTTP_GET, [](AsyncWebServerRequest *request)
  {
    int val = analogRead(capteurLuminosite);
    String luminosite = String(val);
    request->send(200, "text/plain", luminosite);
  });

  server.on("/save",
        HTTP_POST,
        [](AsyncWebServerRequest * request){},
        NULL,
        [](AsyncWebServerRequest * request, uint8_t *data, size_t len,
        size_t index, size_t total) {
            // Here goes the code to manage the post request
            // The data is received on 'data' variable
            // Parse data
            Serial.println("POST RECEIVED"); // Just for debug
            StaticJsonBuffer<50> JSONBuffer; // create a buffer that fits for you
            JsonObject& parsed = JSONBuffer.parseObject(data); //Parse message
            const char* received_data = parsed["words"]; //Get data
            /*for (size_t i = 0; i < (size_t)received_data.size(); i++) {
                Serial.write(received_data[i]);
              }*/
         
            Serial.println(received_data);
            request->send(200);
    });

  /*server.on("/save", HTTP_GET, [](AsyncWebServerRequest *request)
  {
    Serial.println("Envoi de la liste de mots");
    int paramsNr = request->params();
    Serial.println(paramsNr);
    for(int i=0;i<paramsNr;i++){
        AsyncWebParameter* p = request->getParam(i);
        Serial.print("Param name: ");
        Serial.println(p->name());
        Serial.print("Param value: ");
        Serial.println(p->value());
        Serial.println("------");
    }
    request->send(200, "text/plain", "OK");
  });*/

 server.on("/on", HTTP_GET, [](AsyncWebServerRequest *request)
  {
    digitalWrite(led, HIGH);
    request->send(200);
  });
  server.on("/off", HTTP_GET, [](AsyncWebServerRequest *request)
  {
    digitalWrite(led, LOW);
    request->send(200);
  });

  server.begin();
  Serial.println("Serveur actif!");
}

void loop()
{

}
