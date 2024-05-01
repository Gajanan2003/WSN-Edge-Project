
#include <ESP8266WiFi.h>
#include <SPI.h>
#include <MFRC522.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <ArduinoJson.h>

#define SS_PIN 2 // Define the SS_PIN for NodeMCU
#define RST_PIN 0 // Define the RST_PIN for NodeMCU

MFRC522 mfrc522(SS_PIN, RST_PIN);  // Create MFRC522 instance.
MFRC522::MIFARE_Key key;

int readsuccess;
byte readcard[4];
char str[32] = "";
String StrUID;
String CarName;
String ChargeTimeAC;
String ChargeTimeDC;
String Range;
String Port;

// Buzzer and LED setup
int buzzerPin = 5;  // Connect the buzzer to pin D1 (GPIO5)
// int ledPin = 4;    // Connect the LED to pin D2 (GPIO4)


// WiFi setup
const char* ssid = "Kedar"; // WiFi SSID
const char* password = "mton8527"; // WiFi Password
const char* serverAddress = "192.168.83.78"; // Server IP address
const int serverPort = 5000; // Server Port
WiFiClient client;


void setup() {
  Serial.begin(9600); // Initialize serial communications with the PC
  SPI.begin();      // Init SPI bus
  mfrc522.PCD_Init(); // Init MFRC522 card
 WiFi.begin(ssid, password); // Connect to WiFi

  // Check WiFi connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP()); // Display the IP Address


  pinMode(buzzerPin, OUTPUT);
  pinMode(ledPin, OUTPUT);

  delay(1000);
}


void sendPostRequest() {

  HTTPClient http; // Declare object of class HTTPClient
  String serverPath = "http://192.168.83.78:5000/data";

  http.begin(client, serverPath); // Specify request destination
  http.addHeader("Content-Type", "application/json"); // Specify content-type header

  // Create JSON document
  StaticJsonDocument<400> doc;
  doc["StrUID"] = StrUID;
  doc["CarName"] = CarName;
  doc["Range"] = Range ;

  String httpRequestData;
  serializeJson(doc, httpRequestData); // Serialize the JSON data
  Serial.println("Sending this JSON:");
  Serial.println(httpRequestData);

  // Send the POST request
  int httpResponseCode = http.POST(httpRequestData);

  if (httpResponseCode > 0) {
    String response = http.getString(); // Get the response
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);
    Serial.println("Response: ");
    Serial.println(response);
  } else {
    Serial.print("Error on sending POST: ");
    Serial.println(httpResponseCode);
  }

  http.end(); // Close connection
}


void loop() {


  while (WiFi.status() == WL_CONNECTED) {
    
    mfrc522.PCD_Init(); // Re-initialize the RFID reader
    
    for (byte i = 0; i < 6; i++) {
      key.keyByte[i] = 0xFF;
    }
    
    if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) {
      // Function to activate Buzzer and LED
      activateBuzzerAndLED();

      for (int i = 0; i < 4; i++) {
        readcard[i] = mfrc522.uid.uidByte[i]; // Storing the UID of the tag in readcard
        array_to_string(readcard, 4, str);
        StrUID = str;
      }

      // Read data from specific blocks (4, 5, 6, 8, 9)
      CarName = readBlock(4);
      ChargeTimeAC = readBlock(5);
      ChargeTimeDC = readBlock(6);
      Range = readBlock(8);
      Port = readBlock(9);

      // Print RFID data to serial monitor
      Serial.println(StrUID + "," + CarName + "," + ChargeTimeAC + "," + ChargeTimeDC + "," + Range + "," + Port);
      sendPostRequest();
      // Halt the card and add a delay after successfully reading a card
      mfrc522.PICC_HaltA();
      delay(3000);
    }
  }
}

// Function to activate Buzzer and LED
void activateBuzzerAndLED() {
  tone(buzzerPin, 3200);  // Turn on the buzzer
  // digitalWrite(ledPin, HIGH);     // Turn on the LED
  delay(400);  // Keep them on for 1 second
  noTone(buzzerPin);   // Turn off the buzzer
  // digitalWrite(ledPin, LOW);      // Turn off the LED
}

// Function to get RFID card ID and read associated data
int getid() {  
  if(!mfrc522.PICC_IsNewCardPresent()){
    return 0;
  }
  if(!mfrc522.PICC_ReadCardSerial()){
    return 0;
  }

  for(int i=0; i<4; i++){
    readcard[i] = mfrc522.uid.uidByte[i]; // Storing the UID of the tag in readcard
    array_to_string(readcard, 4, str);
    StrUID = str;
  }

  // Read data from specific blocks (4, 5, 6, 8, 9)
  CarName = readBlock(4);
  ChargeTimeAC = readBlock(5);
  ChargeTimeDC = readBlock(6);
  Range = readBlock(8);
  Port = readBlock(9);

  mfrc522.PICC_HaltA();
  return 1;  
}

// Function to read data from a specific block
String readBlock(int blockNumber) {
  MFRC522::StatusCode status;
  byte buffer[18];
  byte size = sizeof(buffer);

  status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, blockNumber, &key, &(mfrc522.uid));
  if (status != MFRC522::STATUS_OK) {
    return "";
  }

  status = mfrc522.MIFARE_Read(blockNumber, buffer, &size);
  if (status == MFRC522::STATUS_OK) {
    String data = "";
    for (byte i = 0; i < 16; i++) {
      char c = (char)buffer[i];
      if (c != '\0') {
        data += c;
      }
    }
    return data;
  } else {
    return "";
  }
}

// Function to convert byte array to string
void array_to_string(byte array[], unsigned int len, char buffer[]) {
  for (unsigned int i = 0; i < len; i++) {
    byte nib1 = (array[i] >> 4) & 0x0F;
    byte nib2 = (array[i] >> 0) & 0x0F;
    buffer[i*2+0] = nib1  < 0xA ? '0' + nib1  : 'A' + nib1  - 0xA;
    buffer[i*2+1] = nib2  < 0xA ? '0' + nib2  : 'A' + nib2  - 0xA;
  }
  buffer[len*2] = '\0';
}
