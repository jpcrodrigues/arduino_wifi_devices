#include <DHT.h>
#include <ESP8266WiFi.h>

#define DHTPIN D12
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);
WiFiServer server(80);

const char* ssid = "wifi_ssid";
const char* password = "your_wiki_password";
const boolean serialPrint = false;

void setup()
{
  Serial.begin(115200);
  delay(10);
  dht.begin();

  connectToWifi();
  startServer();
}

void loop()
{
  WiFiClient client = server.available();

  float aTemperature = dht.readTemperature();
  float aHumidity = dht.readHumidity();

  if (serialPrint && (isnan(aTemperature) || isnan(aHumidity)))
  {
    Serial.println("============ Serial Debug =============");
    Serial.println("= Error while reading DHT sensor data =");
    Serial.println("=======================================");
    return;
  }

  if (serialPrint) {
    Serial.println("====== Serial Debug =======");
    Serial.print("=> Temperature: ");
    Serial.println(aTemperature);
    Serial.print("=> Humidity: ");
    Serial.println(aHumidity);
    Serial.println("===========================");
  }
  
  delay(500);

  printResponse(client, aTemperature , aHumidity);
}

/*
 * This method is reponsible to connect to the wifi network 
 */
void connectToWifi() {
  WiFi.begin(ssid, password);

  Serial.print("Connecting to: ");
  Serial.print(ssid);

  WiFi.begin(ssid, password);

  if (serialPrint) {    
    Serial.print("Connecting");
    while (WiFi.status() != WL_CONNECTED)
    {
      delay(500);
      Serial.print(".");
    }

    Serial.println("");
    Serial.println("WiFi connected");
    Serial.print("IP: ");
    Serial.print(WiFi.localIP());
    Serial.println();
  }

}

/*
 * This method is used to start the server 
 */
void startServer() {
  server.begin();
  Serial.println("Server Started");
}

/*
 * Print the http json response to the client 
 */
void printResponse(WiFiClient theClient, float theTemperature, float theHumidity) {
  if (!theClient) {
    return;
  }

  // Wait until the client sends some data
  while (!theClient.available()) {
    delay(1);
  }

  // Read the first line of the request
  String request = theClient.readStringUntil('\r');
  //Serial.println(request);
  theClient.flush();

  theClient.println("HTTP/1.1 200 OK");
  theClient.println("Content-Type: application/json");
  theClient.println("Access-Control-Allow-Origin: *");
  theClient.println("Access-Control-Allow-Headers: Access-Control-Allow-Headers, Origin, X-Requested-With, Content-Type, Accept, Authorization");
  theClient.println("");
  theClient.println("{");
  theClient.println("\"temperature\": ");
  theClient.println(theTemperature);
  theClient.println(",");
  theClient.println("\"humidity\": ");
  theClient.println(theHumidity);
  theClient.println("}");
}
