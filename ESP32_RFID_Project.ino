//Importing necessary libraries
#include <SPI.h>
#include <MFRC522.h>
#include <WiFi.h>
#include <HTTPClient.h>

//Initializing variables for Wi-Fi credentials
const char* ssid = "YOUR_SSID"; 
const char* password = "YOUR_PASSWORD";

// Initializing server information for sending log data
const char* serverName = "http://YOUR_DEVICE_IP/access_log.php"; //Initializing variable with server address
String api_key = "secret4321"; //API authentication key for PHP script

//Assigning pins to the corresponding pin numbers
#define SDA_pin 5
#define RST_pin 22
#define ACCESS_GRANTED_PIN 27
#define ACCESS_DENIED_PIN 4

//Creating RFID object to access RFID-related methods
MFRC522 rfid(SDA_pin, RST_pin);

//Defining list of authorized UIDs
const byte authorizedUIDList[][4] = {
  {0x34, 0x93, 0x28, 0x03},
  {0x43, 0x37, 0x2E, 0x4B},
  {0x23, 0xE1, 0xDE, 0x2B}
};

//Identifies number of authorized UIDs (useful for checking if UID is authorized)
const byte num_cards = sizeof(authorizedUIDList) / sizeof(authorizedUIDList[0]);

//-------------------------------------------------------------------------------------------------------------------------------------------------

//Function that compares present UID to the list of authorized UIDs
//and verifies if there is a match
bool verifyUID (byte *currentUID, const byte list[][4], byte numCards){
  for(byte i = 0; i < numCards; i++){
    bool verified = true; //Sets verified to true as default
    for(byte j = 0; j < 4; j++){
      if(currentUID[j] != list[i][j]){
        verified = false; //If at least one byte does not match, the card is considered unauthorized.
        break;
      }
    }
    if(verified)
      return true; //UID is verified as a match if it matches with an authorized UID in the list.
  }
  return false;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------

//Function that converts UID to string for clean output for logging
String uidToString(byte *uid, byte uidSize){
  String uidString = "";
  for (byte i = 0; i < uidSize; i++){
    if (uid[i] < 0x10){
      uidString += "0"; //If number is a single hex digit, add zero in front
    }
    uidString += String(uid[i],HEX);
    if(i < uidSize - 1){
      uidString += " "; //Adds space between bytes if the current byte isn't the last byte 
    }
  }
  uidString.toUpperCase(); //Makes output look clean if some values contain hex letters
  
  return uidString;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------

void send_log(String uidString, String status){
  if(WiFi.status() == WL_CONNECTED){
    HTTPClient http; //Creates HTTPClient object named http to help make HTTP requests

    http.begin(serverName); //Opens HTTP connection to the server
    http.addHeader("Content-Type", "application/x-www-form-urlencoded"); //Adds HTTP header to match how PHP script reads the $_POST request

    //Building the body of the $_POST data request
    String post_data = "uid=" + uidString + "&status=" + status + "&key=" + api_key;

    //Sending the $_POST request
    int httpResponseCode = http.POST(post_data);

    //If response code is negative, the request did not reach the server, so it returns the error code for debugging
    if(httpResponseCode <= 0){
      Serial.print("Error code: ");
      Serial.print(httpResponseCode);
      Serial.println(". The HTTP request did not reach the server.");
    }
    
    http.end(); //Closes HTTP connection
  } else{
    Serial.println("WiFi disconnected.");
  }
}

//-------------------------------------------------------------------------------------------------------------------------------------------------

void setup() {

  Serial.begin(115200); //Set communication channel baud rate
  delay(1400); //Waits for Serial to be completely initialized (1290 sorta works sometimes, 1300 works), 1400 to be safe

  //Initialize SPI bus and RC522 chip, respectively
  SPI.begin();
  rfid.PCD_Init();

  // Read the version register to test communication and print it out
  byte version = rfid.PCD_ReadRegister(MFRC522::VersionReg);
  Serial.print("RC522 Version Register: 0x");
  Serial.println(version, HEX);

  //Check for a valid version number
  if(version == 0x00 || version == 0xFF || version == 0xEE){
    Serial.println("RC522 not detected!");
    exit(0);
  }
  else{
    //If a valid version is read, continue the program in order to read tags
    Serial.println("RFID reader initialized successfully ✅");
    Serial.println("Place your RFID tag near the reader.");
    Serial.println();
  }

  //Setting pin modes for access granted/denied LEDs
  pinMode(27, OUTPUT); // Access granted blue LED
  pinMode(4, OUTPUT); // Access denied red LED

  //Wi-Fi Setup -------------------------------------------------------------------
  Serial.println();
  Serial.println("Connecting to WiFi");
  Serial.println(ssid);

  WiFi.begin(ssid,password);

  while(WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("WiFi connected!");
  Serial.println("IP Address:");
  Serial.println(WiFi.localIP());
  Serial.println();
}

//-------------------------------------------------------------------------------------------------------------------------------------------------

void loop() {
  //Actively look for new cards by returning to loop if card not found
  if(!rfid.PICC_IsNewCardPresent()){
    return;
  }

  //Reads the data from present card (ensures a successfully read card)
  if(!rfid.PICC_ReadCardSerial()){
    return;
  }

  //Creates array to store RFID UID values
  byte UID[rfid.uid.size];

  //Storing UID values into the array
  for(byte i = 0; i < rfid.uid.size; i++){
    UID[i] = rfid.uid.uidByte[i];
  }

  //Convert the UID to string
  String uidString = uidToString(UID, rfid.uid.size);

  //Get elapsed time (in seconds) since the program started using millis function
  unsigned long timestamp = millis()/1000; //Long is used to store potentially large timestamp

  //Printing the timestamp of card scan to the Serial Monitor
  Serial.print("[");
  Serial.print(timestamp);
  Serial.print("s] UID: ");
  Serial.print(uidString);

  //Declaring string that represents access status
  String status;

  //Granting or denying access to the system based on if UID is valid and turning on appropriate LED accordingly
  if(verifyUID(UID, authorizedUIDList, num_cards)){
    status = "Access Granted";
    Serial.println(" → Access granted. Welcome!");
    send_log(uidString, status); //Sends the access status info to the database in order to log it in a table
    digitalWrite(ACCESS_GRANTED_PIN, HIGH);
    delay(2000);
    digitalWrite(ACCESS_GRANTED_PIN, LOW);
  }
  else{
    status = "Access Denied";
    Serial.println(" → Access denied.");
    send_log(uidString, status); //Sends the access status info to the database in order to log it in a table
    digitalWrite(ACCESS_DENIED_PIN, HIGH);
    delay(2000);
    digitalWrite(ACCESS_DENIED_PIN, LOW);
  }
  //Halts further detection of the same access card to prevent card
  //from being processed multiple times
  rfid.PICC_HaltA();
}
