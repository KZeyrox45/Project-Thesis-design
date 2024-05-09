
/*------------------WHAT'S IN THIS PROGRAM-----------------------------
1) This shows a live human Heartbeat Pulse.
2) Live visualization in Arduino's Cool "Serial Plotter".
3) Blink an LED on each Heartbeat.
4) This is the direct Pulse Sensor's Signal.
5) A great first-step in troubleshooting your circuit and connections.
6) "Human-readable" code that is newbie friendly."
---------------------------------------------------------------------*/

/*-------------MADE BY STUDENTS OF BACK KHOA UNIVERSITY----------------
1. Nguyen Trung Hieu - 2113357
2. Pham Ngoc Khai    - 2113650
3. Thai Anh Khuong   - 2113806
---------------------------------------------------------------------*/

// WiFi to connect to the server
#include <WiFi.h>

// Firebase is connected through WiFi to read/write data
#include <Firebase_ESP_Client.h>

// Provide the token generation process info.
#include <addons/TokenHelper.h>

// Provide the RTDB payload printing info and other helper functions.
#include <addons/RTDBHelper.h>

// For WiFi, both of these variables must be specified each time we test
// SSID -> Name of WiFi
#define WIFI_SSID "Ping Pong"
#define WIFI_PASSWORD "xiaoping999"

// For Firebase, URL is for Real-time Database and API_KEY, both of them are required to connect Firebase
#define DATABASE_URL "https://bpm-reader-8ba0b-default-rtdb.firebaseio.com/"
#define API_KEY "AIzaSyCd0OmslyNI1jpijjhQ5tC785v-BA_PbwE"

FirebaseData fbdo;

FirebaseAuth auth;
FirebaseConfig config;

unsigned long sendDataPrevMillis = 0;

// Sign up anonymously, also required to connect to Firebase
bool signUpOK = false;

//  Variables
int PulseSensorPurplePin = 36;        // Pulse Sensor PURPLE WIRE connected to ANALOG PIN 0
int LED2 = 2;                         //  The on-board Arduion LED

int Signal;                           // holds the incoming raw data. Signal value can range from 0-1024
int Threshold = 550;                  // Determine which Signal to "count as a beat", and which to ingore.

/* Set up to connect WiFi */
void beginWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();
}

/* Configure Firebase to read/write data */
void setUpFirebase() {
  Serial.printf("Firebase Client v%s\n\n", FIREBASE_CLIENT_VERSION);

  /* Assign the api key (required) */
  config.api_key = API_KEY;

  /* Assign the RTDB URL (required) */
  config.database_url = DATABASE_URL;

  /* Sign up */
  if(Firebase.signUp(&config, &auth, "", "")){
    Serial.println("ok");
    signUpOK = true;
  }else{
    Serial.printf("%s\n", config.signer.signupError.message.c_str());
  }

  /* Assign the callback function for the long running token generation task */
  config.token_status_callback = tokenStatusCallback; //see addons/TokenHelper.h

  Firebase.begin(&config, &auth);

  //Comment or pass false value when WiFi reconnection will control by your code or third party library
  Firebase.reconnectWiFi(true);
}

// The SetUp Function:
void setup() {
  pinMode(LED2, OUTPUT);         // pin that will blink to your heartbeat!
  Serial.begin(115200);         // Set's up Serial Communication at certain speed.

  // Call beginWiFi function to connect WiFi
  beginWiFi();

  /*
    ESP32 analogRead defaults to 13 bit resolution
    PulseSensor Playground library works with 10 bit
  */
  analogReadResolution(10);

  // Call function to set up to connect to Firebase
  setUpFirebase();
}

// The Main Loop Function
void loop() {

  Signal = analogRead(PulseSensorPurplePin);
  
  Serial.println(Signal);

  // Check the led status by comparing the threshold
  if(Signal > Threshold){
    digitalWrite(LED2, HIGH);
  } else {
    digitalWrite(LED2, LOW);
  }

  if(Firebase.ready() && signUpOK && (millis() - sendDataPrevMillis > 1000 || sendDataPrevMillis == 0)){
    // If Firebase is connected successfully, send the message and wait for 1 seconds to proceed
    sendDataPrevMillis = millis();

    // If we can send data to Firebase successfully, print the lines in if statement, otherwise print in else statement
    if(Firebase.RTDB.setInt(&fbdo, "BPM_Reader/BPM", Signal)){
      Serial.println("SENT SUCCESSFULLY");
      Serial.println("PATH: " + fbdo.dataPath());
      Serial.println("TYPE OF VALUE: " + fbdo.dataType());
    }else{
      Serial.println("FAILED TO SENT DATA");
      Serial.println("REASON: " + fbdo.errorReason());
    }
  }

  delay(20);
}
