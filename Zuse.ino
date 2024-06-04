#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>

// Définition des constantes
#ifndef STASSID
#define STASSID "wifi"
#define STAPSK  "test1234"
#endif

const char* ssid = STASSID;
const char* password = STAPSK;

const int led = BUILTIN_LED;
const int relayPin = D1;
const int Led_avertissement = D2;
const int pompe1 = D2;
const int vanne1 = D1;
const int pompe3 = D5;

// Le serveur Web "écoute" sur le port 80 (port HTTP par défaut)
ESP8266WebServer server(80);

/****************************************************************************/
void handleRoot() {
  int val_led;
  int val_pompe1;
  int val_vanne1;
  int val_pompe3;
  val_led = digitalRead(led);
  val_pompe1 = digitalRead(pompe1);
  val_vanne1 = digitalRead(vanne1);
  val_pompe3 = digitalRead(pompe3);

  // HTML content for the custom webpage
  String s = "<!DOCTYPE html>\r\n<html lang=\"fr\">\r\n<head>\r\n";
  s += "    <meta charset=\"UTF-8\">\r\n";
  s += "    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\r\n";
  s += "    <title>Zuse</title>\r\n";
  s += "    <style>\r\n";
  s += "        body {\r\n";
  s += "            font-family: Arial, sans-serif;\r\n";
  s += "            text-align: center;\r\n";
  s += "            margin: 50px;\r\n";
  s += "        }\r\n";
  s += "        h1 {\r\n";
  s += "            color: #333;\r\n";
  s += "        }\r\n";
  s += "        .button-container {\r\n";
  s += "            display: flex;\r\n";
  s += "            flex-wrap: wrap;\r\n";
  s += "            justify-content: center;\r\n";
  s += "        }\r\n";
  s += "        button {\r\n";
  s += "            padding: 10px 20px;\r\n";
  s += "            font-size: 16px;\r\n";
  s += "            margin: 10px;\r\n";
  s += "            cursor: pointer;\r\n";
  s += "            background-color: #4CAF50;\r\n";
  s += "            color: #fff;\r\n";
  s += "            border: none;\r\n";
  s += "            border-radius: 4px;\r\n";
  s += "        }\r\n";
  s += "        button:hover {\r\n";
  s += "            background-color: #45a049;\r\n";
  s += "        }\r\n";
  s += "    </style>\r\n";
  s += "</head>\r\n<body>\r\n";
  s += "    <h1>Zuse</h1>\r\n";
  s += "<br/><br/><div><a style=\"display:block; height:60px; background-color:";
  s += (val_led)?"blue":"DarkBlue";   // La couleur du bouton change selon l'Ã©tat du relais
  s += "; color:white; text-align:center; padding:20px; margin:20px; font-size:250%; text-decoration: none;\" href=\"http://";
  s += WiFi.localIP().toString();
  s += "/btn1/";
  s += (val_led)?"0":"1";
  s += "\">Btn1</a></div>\r\n";
  s += "<br/><br/><div><a style=\"display:block; height:60px; background-color:";
  s += (val_led)?"blue":"DarkBlue";   // La couleur du bouton change selon l'Ã©tat du relais
  s += "; color:white; text-align:center; padding:20px; margin:20px; font-size:250%; text-decoration: none;\" href=\"http://";
  s += WiFi.localIP().toString();
  s += "/btn2/";
  s += (val_led)?"0":"1";
  s += "\">Btn2</a></div>\r\n";
  s += "<br/><br/><div><a style=\"display:block; height:60px; background-color:";
  s += (val_led)?"blue":"DarkBlue";   // La couleur du bouton change selon l'Ã©tat du relais
  s += "; color:white; text-align:center; padding:20px; margin:20px; font-size:250%; text-decoration: none;\" href=\"http://";
  s += WiFi.localIP().toString();
  s += "/btn3/";
  s += (val_led)?"0":"1";
  s += "\">Btn3</a></div>\r\n";
  s += "</body>\r\n</html>\r\n";

  // Envoie de la page (Le code HTTP 200 indique que la requête a été traitée avec succès
  server.send(200, "text/html", s);
}

/****************************************************************************/
void handleNotFound() {
  digitalWrite(led, 1);
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
  digitalWrite(led, 0);
}

/****************************************************************************/
void setup(void) {
  // Initialisation GPIO led et relais en sorties
  pinMode(led, OUTPUT);
  pinMode(relayPin, OUTPUT);
  pinMode(Led_avertissement, OUTPUT);
  pinMode(pompe1, OUTPUT);
  pinMode(vanne1, OUTPUT);
  pinMode(pompe3, OUTPUT);

  digitalWrite(led, LOW);
  digitalWrite(relayPin, LOW);
  digitalWrite(Led_avertissement, LOW);
  digitalWrite(pompe1, LOW);
  digitalWrite(vanne1, LOW);
  digitalWrite(pompe3, LOW);

  // Initialisation port série à  115 200 bauds
  Serial.begin(115200);

  // Connection au point d'accès WiFi
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  // Affichage des paramètres de connexion (point d'accès et IP obtenue par DHCP)
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  // Quand un client se connecte sur l'URL / on appelle la fonction handleRoot()
  server.on("/", handleRoot);

  server.on("/btn1/1", []() {
    digitalWrite(pompe1, HIGH);
    delay(300);
    digitalWrite(vanne1, HIGH);
    delay(5500);
    digitalWrite(vanne1, LOW);
    delay(300);
    digitalWrite(pompe1, LOW);
    handleRoot();
  });

  server.on("/btn2/1", []() {
    digitalWrite(pompe1, HIGH);
    delay(300);
    digitalWrite(vanne1, HIGH);
    delay(10000);
    digitalWrite(vanne1, LOW);
    delay(300);
    digitalWrite(pompe1, LOW);
    handleRoot();
  });

  server.on("/btn3/1", []() {
    digitalWrite(led, LOW);
    delay(1000);
    digitalWrite(pompe3, HIGH);
    digitalWrite(led, HIGH);
    delay(1000);
    digitalWrite(led, LOW);
    delay(1000);
    digitalWrite(led, HIGH);
    delay(1000);
    digitalWrite(led, LOW);
    delay(1000);
    digitalWrite(led, HIGH);
    delay(1000);
    digitalWrite(led, LOW);
    delay(1000);
    digitalWrite(led, HIGH);
    delay(1000);
    digitalWrite(led, LOW);
    delay(1000);
    digitalWrite(led, HIGH);
    delay(1000);
    digitalWrite(led, LOW);
    delay(1000);
    digitalWrite(pompe3, LOW);
    handleRoot();
  });

  // Si le client se connecte sur une URL non programmée
  server.onNotFound(handleNotFound);  // Appel de la fonction handleNotFound()

  // Démarrage du serveur
  server.begin();
  Serial.println("HTTP server started");
}

/****************************************************************************/
void loop(void) {
  server.handleClient();  // Attente de la connexion d'un client
}