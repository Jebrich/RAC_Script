#ifdef REMOTE
#include <math.h>
#include <esp_now.h>
#include "esp_wifi.h"
#include <WiFi.h>
#include <batteryMonitor.h>
#include <ledUtility.h>
#include "esp_log.h"
#include "mac.h"

static const char *TAG = "MAIN";
//------------ turn on generic serial printing

#define DEBUG_PRINTS
//data that will be sent to the receiver

typedef struct {
  int16_t speedmotorLeft;
  int16_t speedmotorRight;
  int16_t packetArg1;
  int16_t packetArg2;
  int16_t packetArg3;
}
packet_t;


packet_t sentData;
packet_t recData;


//---------------------------------------ESP_NOW Variables


String success;
esp_now_peer_info_t peerInfo;
// Callback when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  if (status == 0) {
    success = "Delivery Success :)";
  }
  else {
    success = "Delivery Fail :(";
  }
}

// Callback when data is received
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  memcpy(&recData, incomingData, sizeof(recData));
}




//---------------------------------------HARDWARE DEPENDANT Variables
// one ifdef case per hardware to speed up modularity of the code

//RAC standard remote
const int steerPot = 7;
const int accPot = 10;
const int leverPot = 8;
//const int trimPot = 39;

const int rightBtn = 2;
const int leftBtn = 4;
const int topBtn = 5;
//const int lowSwitch = 32;
//const int topSwitch = 25;
LedUtility Led(21);

//customisable vars
int analogRes = 10;
int analogReadMax = (1 << analogRes)-1;


//variables for the sketch
int leverValue = 0;

unsigned long current_time = 0;


void setup() {
  //store_values(); // uncomment only to initialize mem
  analogReadResolution(analogRes);
  analogSetAttenuation(ADC_11db);
  pinMode(rightBtn, INPUT_PULLUP);
  pinMode(leftBtn, INPUT_PULLUP);
  pinMode(topBtn, INPUT_PULLUP);
  //pinMode(lowSwitch, INPUT_PULLUP);
  //pinMode(topSwitch, INPUT_PULLUP);
  Led.init();
  Led.setBlinks(1,150);
  delay(2000);
#ifdef DEBUG_PRINTS
  Serial.begin(115200);
  Serial.println("RAC GENERIC BOT");
#endif


  //---------------------------------------ESP NOW setup
  WiFi.mode(WIFI_STA);
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  esp_now_register_send_cb(OnDataSent);
  memcpy(peerInfo.peer_addr, robotAddress, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;
  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Failed to add peer");
    return;
  }
  char macStr[18];
  Serial.print("Packet from: ");
  // Copies the sender mac address to a string
  snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
           peerInfo.peer_addr[0], peerInfo.peer_addr[1], peerInfo.peer_addr[2], peerInfo.peer_addr[3], peerInfo.peer_addr[4], peerInfo.peer_addr[5]);
  Serial.print("sending to: ");
  Serial.println(macStr);
  esp_now_register_recv_cb(OnDataRecv);
  Led.setBlinks(0);
  Led.ledOn();
}



void loop() {
  //read pots values
  int strValue = analogRead(steerPot);
  delay(3);
  int accValue = analogRead(accPot);
  delay(3);
  int leverValue = analogRead(leverPot);
  delay(3);
  current_time = millis(); 
  bool rightValue = !digitalRead(rightBtn);
  bool leftValue = !digitalRead(leftBtn);
  bool topValue = !digitalRead(topBtn);
  
  // vvvv ----- YOUR AWESOME CODE HERE ----- vvvv //

//!!!!!!!!!!! BUTTON VARIABLES !!!!!!!!!!\\

int topButton = 0;
int rightButton = 0;
int leftButton = 0;

//!!!!!!!!!!! LEVER AXIS !!!!!!!!!!\\

//!!!!!!!!!!! ANALOG !!!!!!!!!!\\

// ! Check if accValue falls within the dead zone range
	if (accValue >= 422 && accValue <= 495) {
	// ! If it does, set accValue to zero
	accValue = 512;
	}

	if (strValue >= 399 && strValue <= 479) {
	// ! If it does, set strValue to zero
	strValue = 512;
	}

/* int up = map(accValue, 0, 511, 512, 0);
int down = map(accValue, 512, 1023, 0, -512);  // Modified mapping range for backward movement
int dx = map(strValue, 513, 1023, 0, 512);
int sx = map(strValue, 0, 511, -512, 0);
dx = constrain(dx, 0, 512);
sx = constrain(sx, -512, 0);
up = constrain(up, 0, 512);
down = constrain(down, -512, 0); */

int axis_y = map(accValue, 0, 1023, -512, 512);
int axis_x = map(strValue, 0, 1023, 512, -512);
axis_y = constrain(axis_y, -512, 512);
axis_x = constrain(axis_x, -512, 512);





/*Serial.print("Sterzo: ");
Serial.println(strValue);
Serial.print("Acceleratore: ");
Serial.println(accValue);*/

// todo: make motors go ahead and backward	



sentData.speedmotorLeft = axis_x + axis_y;
sentData.speedmotorRight = axis_x - axis_y;

Serial.print("SpeedmotorLeft: ");
Serial.println(sentData.speedmotorLeft);
Serial.print("SpeedmotorRight: ");
Serial.println(sentData.speedmotorRight);	

//!!!!!!!!!!! BUTTONS !!!!!!!!!!\\

// TODO RIGHT BUTTON

/* rightButton = digitalRead(rightBtn);

if (rightButton == LOW) {
delay(50); //! Delay for button debouncing
if (digitalRead(rightBtn) == LOW) {
// Button is pressed, perform action
Serial.println("Right button is pressed");
// Additional code...
}
if (digitalRead(rightBtn) == LOW) {
// todo Button is pressed, perform action
// todo Additional code...
}
} 

// TODO LEFT BUTTON

leftButton = digitalRead(leftBtn);

if (leftButton == LOW) {
delay(50); //! Delay for button debouncing
leftButton = 0;
if (digitalRead(leftBtn) == LOW) {
// Button is pressed, perform action
Serial.println("Left button is pressed");
// Additional code...
}
if (digitalRead(leftBtn) == LOW) {
// todo Button is pressed, perform action
// todo Additional code...
}
}

// TODO TOP BUTTON

topButton = digitalRead(topBtn);

if (topButton == LOW) {
delay(50); //! Delay for button debouncing
if (digitalRead(topBtn) == LOW) {
// Button is pressed, perform action
Serial.println("Top button is pressed");
// Additional code...
}
if (digitalRead(topBtn) == LOW) {
// todo Button is pressed, perform action
// todo Additional code...
}
}

//!!!!!!!!!!! LEVER !!!!!!!!!!\\



 if (leverValue >= deadZoneMin && leverValue <= deadZoneMax) {
leverValue = 0; // Set idle position to 0
} else {
leverValue = map(leverValue, 0, 1023, 1023, -1023);
leverValue = constrain(leverValue, -512, 512);
} */

// + ------------------------------ +

/* Serial.println("--------------------");
Serial.print("right motor: ");
Serial.println(sentData.speedmotorRight);
Serial.print("left motor: ");
Serial.println(sentData.speedmotorLeft);
Serial.print("RIGHT BUTTON: ");
Serial.println(rightButton);
Serial.print("LEFT BUTTON: ");
Serial.println(leftButton);
Serial.print("TOP BUTTON: ");
Serial.println(topButton);
Serial.print("LEVA Value: ");
Serial.println(leverValue);
Serial.println("--------------------"); */


// ! -------------------------------------------- //

  esp_err_t result = -1;
  result = esp_now_send(robotAddress, (uint8_t *) &sentData, sizeof(sentData));
  if (result == ESP_OK) {
    //Serial.println("Sent with success");
  } else {
    //Serial.println("Error sending the data");
  }
}
#endif