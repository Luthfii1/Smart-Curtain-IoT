// Blynk authentication settings
#define BLYNK_TEMPLATE_ID "YOUR_BLINK_ID"
#define BLYNK_TEMPLATE_NAME "YOUR_BLINK_NAME"
#define BLYNK_AUTH_TOKEN "YOUR_BLINK_TOKEN"
#define BLYNK_PRINT Serial

#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/timers.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiManager.h>
#include <BlynkSimpleEsp32.h>
#include "time.h"

char auth[] = BLYNK_AUTH_TOKEN;

// Setting ntp server
const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 7*3600;
const int   daylightOffset_sec = 3600;
int prevHour, hour; // To save hour data

// Motor A
const int pin1Motor1 = 27;
const int pin2Motor1 = 26;
const int enaMotor1 = 14;
// LDR Sensor
const int LDRPIN = 32;

// Setting PWM properties
const int freq = 30000;
const int pwmChannel = 0;
const int resolution = 8;
int dutyCycle = 400;
static int taskCoreMotor = 0;
static int taskCoreLDR = 1;
// Timer for 8s
const int endTime = 8000; // Variable to set time open or close for 8 seconds
unsigned long startTime;  // Variable to store the start time

// Task handle for each task
TaskHandle_t motorTaskHandle; // Task handle for motorTask
TaskHandle_t ldrTaskHandle;   // Task handle for ldrTask

// State for open or close curtain
String status = "Undefined";
String prevStatus = "Undefined";
String curtainStatus = "Undefined";
int morningTime = 6;
int dayTime = 7;
int nightTime = 18;
int midNight = 0;
// Check if the motor has already active just for once
bool active;
// Var for check the user choose manual or automatic mode
bool application = false;

// Initialize tasks
void motorTask(void *pvParameters);
void ldrTask(void *pvParameters);
void getLocalTime();
void openMotor();
void closeMotor();
BLYNK_WRITE(V0);

// Setup the pin and the others 
void setup()
{
  // Initialize the serial monitor heartrate
  Serial.begin(115200);

  // WiFiManager
  WiFiManager wm;
  
  // Reset WiFi settings for testing
  wm.resetSettings();

  // Set up WiFiManager
  bool res = wm.autoConnect("Smart Curtain", "12345678");

  if(!res) {
      Serial.println("Failed to connect and hit timeout");
      delay(3000);
      // Reset and try again, or maybe put it to deep sleep
      ESP.restart();
  } 
  else {
      // Connected to WiFi
      Serial.println("Connected to WiFi");
      
      // Initialize connect to Blynk
      Blynk.config(auth);
  }

  // testing
  Serial.println("Testing DC Motor...");
  active = true;
  // Init and get the time
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  getLocalTime();

  // sets the pins as outputs:
  pinMode(pin1Motor1, OUTPUT);
  pinMode(pin2Motor1, OUTPUT);
  pinMode(enaMotor1, OUTPUT);

  // configure LED PWM functionalities
  ledcSetup(pwmChannel, freq, resolution);

  // attach the channel to the GPIO to be controlled
  ledcAttachPin(enaMotor1, pwmChannel);

  // Task for motor
  xTaskCreatePinnedToCore(
      motorTask,        /* Function to implement the task */
      "motorTask",      /* Name of the task */
      10000,             /* Stack size in words */
      NULL,              /* Task input parameter */
      0,                 /* Priority of the task */
      &motorTaskHandle, /* Task handle. */
      taskCoreMotor);    /* Core where the task should run */

  // Task for LDR
  xTaskCreatePinnedToCore(
      ldrTask,          /* Function to implement the task */
      "ldrTask",        /* Name of the task */
      10000,             /* Stack size in words */
      NULL,              /* Task input parameter */
      1,                 /* Priority of the task */
      &ldrTaskHandle,   /* Task handle. */
      taskCoreLDR);     /* Core where the task should run */

  Blynk.virtualWrite(V0, curtainStatus == "Closed" ? 0 : 1);    // Set into the blynk app
  Blynk.virtualWrite(V3, application ? 1 : 0);    // Set into the blynk app
}

// Motor task
void motorTask(void *pvParameters)
{
  while (1)
  {
  // Serial.println("ANEHCOK");
  // Serial.println(application);
  // Serial.println(active);
    if (!application && active){
      Serial.println("TOLONGLAH KO GA JALAN");      
      // // In daylight, it depends on weather
      // if (status == "GELAP"){ // If the LDR in the night, it will close curtain
      //   closeMotor();
      // } else if (status == "TERANG"){ // If the LDR in the day, it will open curtain
      //   openMotor();
      // }
      if ((hour >= midNight && hour < morningTime) || hour >= nightTime){
        Serial.println("Night");
        // To set auto close when it's 6 o'clock
        closeMotor();
      } else if (hour >= morningTime && hour < dayTime){
        Serial.println("Morning");
        // To set auto open when it's between 6 and 7 AM
        openMotor();
      } else if (hour >= dayTime && hour < nightTime){
        Serial.println("Daytime");
        Serial.println(hour);
        Serial.println(status);

        // In daylight, it depends on weather
        if (status == "GELAP"){ // If the LDR in the night, it will close curtain
          closeMotor();
        } else if (status == "TERANG"){ // If the LDR in the day, it will open curtain
          openMotor();
        }
      }
    }
    delay(500);    
  }
}


// LDR sensor Task
void ldrTask(void *pvParameters)
{
  while (1)
  {
    int val = analogRead(LDRPIN);     // Membaca input sensor LDR dan disimpan ke dalam variable val
    Serial.print("LDR Value : ");
    Serial.println(val);

    // Kalo terang kasih state buka gorden
    if(val < 1000){
      status = "TERANG";
    } else {
      status = "GELAP";    // Kalo gelap (malem) kasih state gelap
    }
    
    Blynk.virtualWrite(V2, status);           // Set into the blynk app about the weather status
    getLocalTime();

    if(prevStatus != status && !application) {
      Serial.print("prev : "); Serial.println(prevStatus);
      Serial.print("status : "); Serial.println(status);
      prevStatus = status;
      if(hour >= dayTime && hour < nightTime){
        active = true;   // If status has changed, set active
        Serial.println("Bug di status");
      }
    }


    delay(1000); // Add a delay if needed 
  }
}

// To get real-time from esp32
void getLocalTime(){
  char timeHour[3];
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
    return;
  }
  strftime(timeHour,3, "%H", &timeinfo);                        // Get string hour and copied to timeHour
  hour = atoi(timeHour);                                        // Convert string to integer
  Serial.print("Jam : "); Serial.println(hour);                 // Check jam
  Serial.print("Prev Jam : "); Serial.println(prevHour);        // Check prev jam
  Serial.print("Application : "); Serial.println(application);  // Check is application true or false

  if(prevHour != hour){
    prevHour = hour;
    if((hour == morningTime || hour == dayTime || hour == nightTime)){
      active = true;
      Serial.println("Bug di time");
    } 
  }
  Serial.print("Is active : "); Serial.println(active);
  Serial.println();
}

// Function to open the curtain
void openMotor(){
  Serial.println("Opening Motor");
// ... (in openMotor function)

  curtainStatus = "Open";                   // Set the curtain status for open
  Blynk.virtualWrite(V1, curtainStatus);    // Set into the blynk app
  Blynk.virtualWrite(V0, 1);                // Make sure the input button precisely correct
  digitalWrite(pin1Motor1, LOW);            // Set motor 1 LOW
  digitalWrite(pin2Motor1, HIGH);           // Set motor 1 HIGH, so the motor will not-activate
  
  startTime = millis();                     // Record the start time
  while (millis() - startTime <= endTime) ledcWrite(pwmChannel, dutyCycle); // Will active for 8 seconds

  digitalWrite(pin1Motor1, LOW);            // Set motor 1 LOW, so the motor will not-activate
  digitalWrite(pin2Motor1, LOW);            // Set motor 2 LOW, so the motor will active forward
  active = false;                    // Set active to false, so it will active just for once
}

// Function to close the curtain
void closeMotor(){
  Serial.println("Closing Motor");
// ... (in closeMotor function)

  curtainStatus = "Closed";                 // Set the curtain status for close
  Blynk.virtualWrite(V1, curtainStatus);    // Set into the blynk app
  Blynk.virtualWrite(V0, 0);                // Make sure the input button precisely correct
  digitalWrite(pin1Motor1, HIGH);           // Set motor 1 HIGH
  digitalWrite(pin2Motor1, LOW);            // Set motor 2 LOW, so the motor will active forward

  startTime = millis();                     // Record the start time
  while (millis() - startTime <= endTime) ledcWrite(pwmChannel, dutyCycle);  // Will active for 8 seconds
    
  digitalWrite(pin1Motor1, LOW);            // Set motor 2 LOW, so the motor will active forward
  digitalWrite(pin2Motor1, LOW);            // Set motor 2 LOW, so the motor will active forward
  active = false;                    // Set active to false, so it will active just for once
}

// Variable blynk for datastream V0
BLYNK_WRITE(V0){
  int button_state = param.asInt();
  Serial.print("Button State: ");
  Serial.println(button_state);

  if (button_state == 1 && application){
    Serial.println("Button Open Pressed");
    openMotor();
  } else if (button_state == 0 && application){
    Serial.println("Button Close Pressed");
    closeMotor();
  }

  Blynk.virtualWrite(V0, curtainStatus == "Closed" ? 0 : 1);    // Set into the blynk app
}

// Variable blynk for datastream V3
BLYNK_WRITE(V3){
  application = param.asInt();
  Serial.print("Mode : ");
  Serial.println(application == 1? "Manual Application" : "Auto");

  // If mode change into automatic (not using application) it will check the weather and curtainStatus
  if(!application){
    if((hour >= morningTime && hour < dayTime) && curtainStatus != "Open") openMotor();        // Jam 6-7 wajib terbuka
    if(((hour >= midNight && hour < morningTime) || hour >= nightTime) && curtainStatus != "Closed") closeMotor();
    if(hour >= dayTime && hour < nightTime) {
      if(status == "TERANG" && curtainStatus == "Closed")   openMotor();  // If the status TERANG but the curtain is closed, the curtain should be opened
      else if(status == "GELAP" && curtainStatus == "Open") closeMotor(); // If the status GELAP but the curtain is opened, the curtain should be closed  
    }    
  }
}

// Function loop doing nothing
void loop()
{
  // Do nothing
  Blynk.run();
}