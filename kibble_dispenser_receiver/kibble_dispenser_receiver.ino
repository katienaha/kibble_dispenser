

/* Kibble dispenser receiver
 *  
 * When messages arrive from controller, play a sound and move servo to dispense a dog treat.
 *  
 */

#include <esp_now.h>
#include <WiFi.h>
#include <ESP32Servo.h>
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

// Structure example to send data
// Must match the receiver structure
typedef struct struct_message {
  int dispenseTreat;
  int playMusic;
} struct_message;

// Create a struct_message called myData
struct_message myData;

// callback function that will be executed when data is received
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  memcpy(&myData, incomingData, sizeof(myData));
  
  int dispenseTreat = myData.dispenseTreat;
  int playMusic = myData.playMusic;
  
  Serial.print("dispense treat: "); Serial.print(dispenseTreat); Serial.println();
  Serial.print("play music: "); Serial.print(playMusic); Serial.println();

  if (playMusic > 0) {
    Serial.println("Playing music!");
    // Create music objects
    XT_Wav_Class Sound(atc_22050hz_wav);   
    XT_DAC_Audio_Class DacAudio(audioPin,0);    
  
    // Play the music!
    for (int i = 0; i < 3620; i++) {
      DacAudio.FillBuffer();                
      if(Sound.Playing==false)       
        DacAudio.Play(&Sound);       
      Serial.print(i++);         
    }
    Serial.println();
  }

  if (dispenseTreat > 0) {
    Serial.println("Dispensing treat!");
    // Move the wheel to spit out some treats!
    for (int pos = startAngle; pos >= dispenseAngle; pos -= 1) { // goes from starting angle to dispensing angle
       myservo.write(pos);    // tell servo to go to position in variable 'pos'
       delay(35);             // waits 35ms for the servo to reach the position
     }
  
      // Wait for treats to fall!
     delay(50);               
  
      // Move wheel back to starting position!
     for (int pos = dispenseAngle; pos <= startAngle; pos += 1) { // goes from dispensing angle back to starting angle
       myservo.write(pos);    // tell servo to go to position in variable 'pos'
       delay(35);             // waits 35ms for the servo to reach the position
     }
    delay(150);   
  }
  
}

void setup() {
  // Initialize Serial Monitor
  Serial.begin(115200);


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



  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  
  // Once ESPNow is successfully Init, we will register for recv CB to
  // get recv packer info
  esp_now_register_recv_cb(OnDataRecv);
}

void loop() {

}
