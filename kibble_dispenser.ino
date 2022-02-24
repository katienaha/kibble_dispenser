/* Kibble dispenser
 *  
 * When messages arrive on specified topic, play a sound and move servo to dispense a dog treat.
 *  
 */
 
#include <WiFiClientSecure.h>
#include <PubSubClient.h> 
#include <ESP32Servo.h>
#include "secrets.h"
#include "atc_22050hz.h"
#include "XT_DAC_Audio.h"  

// List which GPIO pins for which purposes
int servoPin = 17; 
int audioPin = 25;

// Servo angles for starting position and dispensing position
int startAngle = 169;
int dispenseAngle = 100;

// Create servo object
Servo myservo;  

// Create wifi client
WiFiClientSecure wiFiClient;

// When message is received on topic
void msgReceived(char* topic, byte* payload, unsigned int len);

// Create pub sub client
PubSubClient pubSubClient(awsEndpoint, 8883, msgReceived, wiFiClient); 

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

  // Allow allocation of all timers
  ESP32PWM::allocateTimer(0);
  ESP32PWM::allocateTimer(1);
  ESP32PWM::allocateTimer(2);
  ESP32PWM::allocateTimer(3);

  // Move servo to starting position
  myservo.setPeriodHertz(50);    // standard 50 hz servo
  myservo.attach(servoPin, 700, 2400); // attaches the servo on pin 18 to the servo object
  myservo.write(startAngle);    // tell servo to go to position in variable 'pos'
  delay(150);             // waits 15ms for the servo to reach the position
  // myservo.detach();
}

/* 
 * Loop continuously, checking for messages on the topic subscribed to
 */
void loop() {
  pubSubCheckConnect();
}

/* 
 * When a message is received on the topic subscribed to
 */
void msgReceived(char* topic, byte* payload, unsigned int length) {

  // Create music data object
  XT_Wav_Class Sound(atc_22050hz_wav);   
  
  // Create audio object
  XT_DAC_Audio_Class DacAudio(audioPin,0);    

  // Play music
  for (int i = 0; i < 3620; i++) {
    DacAudio.FillBuffer();                
    if(Sound.Playing==false)       
      DacAudio.Play(&Sound);       
    Serial.println(i++);         
  }

  Serial.print("Message received on "); Serial.print(topic); Serial.print(": ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  // Dispense treats
  for (int pos = startAngle; pos >= dispenseAngle; pos -= 1) { // goes from starting angle to dispensing angle
     myservo.write(pos);    // tell servo to go to position in variable 'pos'
     delay(35);             // waits 35ms for the servo to reach the position
   }

   // Wait for treats to fall
   delay(50);               

   // Move back to original starting position
   for (int pos = dispenseAngle; pos <= startAngle; pos += 1) { // goes from dispensing angle back to starting angle
     myservo.write(pos);    // tell servo to go to position in variable 'pos'
     delay(35);             // waits 35ms for the servo to reach the position
   }
  delay(150);             

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
    pubSubClient.subscribe("inTopic");
  }
  pubSubClient.loop();
}
