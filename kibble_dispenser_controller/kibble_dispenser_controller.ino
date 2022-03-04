
/* Kibble dispenser controller
 *  
 * When messages arrive on specified topic, send message to receiver to play a sound and move servo to dispense a dog treat.
 *  
 */
 
#include <WiFiClientSecure.h>
#include <PubSubClient.h> 
#include <ArduinoJson.h>
#include <esp_now.h>
#include <WiFi.h>


#include "secrets.h"

// MAC address of receiver ESP32 inside kibble dispenser
uint8_t broadcastAddress[] = {0xEC, 0x94, 0xCB, 0x70, 0x2B, 0xF0};

// Create wifi client
WiFiClientSecure wiFiClient;

// When message is received on topic
void msgReceived(char* topic, byte* payload, unsigned int len);

// Create pub sub client
PubSubClient pubSubClient(awsEndpoint, 8883, msgReceived, wiFiClient); 

// Structure example to send data
// Must match the receiver structure
typedef struct struct_message {
  int dispenseTreat;
  int playMusic;
} struct_message;

// Create a struct_message called myData
struct_message myData;

esp_now_peer_info_t peerInfo;

// callback when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

/* 
 * Run once at startup
 */
void setup() {
  
  Serial.begin(115200); delay(50); Serial.println();

  // Connect to wifi
  Serial.print("Connecting to "); Serial.print(ssid);
  WiFi.begin(ssid, password);
  WiFi.waitForConnectResult();
  Serial.print(", WiFi connected, IP address: "); Serial.println(WiFi.localIP());

  // Set up certs
  wiFiClient.setCACert(rootCA);
  wiFiClient.setCertificate(certificate_pem_crt);
  wiFiClient.setPrivateKey(private_pem_key);

  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Once ESPNow is successfully Init, we will register for Send CB to
  // get the status of Trasnmitted packet
  esp_now_register_send_cb(OnDataSent);

  // Register peer
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;
  
  // Add peer        
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }
}

/* 
 * Loop continuously, checking for messages on the topic subscribed to
 */
void loop() {
  pubSubCheckConnect();
}

/* 
 * When a message is received 
 */
void msgReceived(char* topic, byte* payload, unsigned int length) {

  Serial.print("Message received on "); Serial.print(topic); Serial.print(": ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  StaticJsonDocument<256> doc;  
  deserializeJson(doc, payload, length);

  int timesDispense = doc["timesDispense"];
  int timesPlayMusic = doc["timesPlayMusic"];
  
  // if dispensing more than once
  if (timesDispense > 1) {
    Serial.print("Dispensing more than once"); Serial.println();
    int secondsDispense = doc["secondsDispense"];
    int secondsWaitMin = doc["secondsWaitMin"];
    int secondsWaitMax = doc["secondsWaitMax"];
    
    randomIntervalDispensing(secondsDispense, secondsWaitMin, secondsWaitMax, timesPlayMusic); // ex. For 60 seconds, treat randomly every 10-20 seconds
  }
  else {
    Serial.print("Dispensing only once"); Serial.println();
    dispenseTreat(timesPlayMusic);
  }

}

void dispenseTreat(int timesPlayMusic) {

  myData.dispenseTreat = 1;
  myData.playMusic = timesPlayMusic;
  
  // Send message via ESP-NOW
  esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &myData, sizeof(myData));
   
  if (result == ESP_OK) {
    Serial.println("Sent with success");
  }
  else {
    Serial.println("Error sending the data");
  }


          
}

/*
 * For a specified number of seconds, wait a random amount of time, treat, wait a random amount of time, treat, etc.
 */
void randomIntervalDispensing(int secondsDispense, int secondsWaitMin, int secondsWaitMax, bool playMusic) {
  int startTime = millis();
  int checkTime = millis();
  int millisDispense = secondsDispense * 1000;
  while(checkTime - startTime < millisDispense) {
    Serial.print("Dispensing at random"); Serial.println();
    int millisWait = random(secondsWaitMin*1000, secondsWaitMax*1000);
    delay(millisWait);
    dispenseTreat(playMusic);
    checkTime = millis();
  }
}

/* 
 * Connect to AWS endpoint, subscribe to topic to listen for messages
 */
void pubSubCheckConnect() {
  if ( ! pubSubClient.connected()) {
    Serial.print("PubSubClient connecting to: "); Serial.print(awsEndpoint);
    while ( ! pubSubClient.connected()) {
      Serial.print(".");
      pubSubClient.connect(THINGNAME);
      delay(1000);
    }
    Serial.println(" connected");
    pubSubClient.subscribe("esp32/kibbleDispenser");
  }
  pubSubClient.loop();
}
