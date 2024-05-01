#include <SPI.h>
#include <MFRC522.h>

/* Using Hardware SPI of NodeMCU */
/* MOSI (D7), MISO (D6), and SCK (D5) are fixed */
/* You can configure SS and RST Pins */
#define SS_PIN 2 /* Slave Select Pin */
#define RST_PIN 0 /* Reset Pin */

MFRC522 mfrc522(SS_PIN, RST_PIN); /* Create an instance of MFRC522 */
MFRC522::MIFARE_Key key; /* Create an instance of MIFARE_Key */

/* Set the block to which we want to write data */
/* Be aware of Sector Trailer Blocks */
int blockNum_Name = 4; // 4th block is used to store the Student ID
int blockNum_ChargeTimeAC = 5; // 5th block is used to store the Name of Student
int blockNum_ChargeTimeDC = 6; // 6th block is used to store the phone number of the student
int blockNum_Port = 8; // 8th block is used to store the room number of the student
int blockNum_Range = 9;

byte blockData_Name [16] = {"Tata Nexon EV"}; // Creating an 16 bytes of array to store Student ID
byte blockData_ChargeTimeAC [16] = {"6H 7.2 kW"}; // Creating an 16 bytes of array to store Name of the student
byte blockData_ChargeTimeDC [16] = {"56 Min-50 kW"}; // Creating an 16 bytes of array to store Phone number of student
byte blockData_Port [16] = {"CCS-II"}; // Creating an 16 bytes of array to store Room number of student
byte blockData_Range [16] = {"465 KM"};
/* This is the actual data which is going to be written into the card */

// In order to reset the data stored inside the RFID Card, Comment the above four Lines and Uncomment the below code
// byte blockData_Name[16] = {0};
// byte blockData_ChargeTimeAC[16] = {0};
// byte blockData_ChargeTimeDC[16] = {0};
// byte blockData_Port[16] = {0};
// byte blockData_Range[16] = {0};

/* Create another arrays to read data from Block */
/* Length of buffer should be 2 Bytes more than the size of Block (16 Bytes) */
byte bufferLen = 18;
byte readBlockData_Name[18];
byte readBlockData_ChargeTimeAC[18];
byte readBlockData_ChargeTimeDC[18];
byte readBlockData_Port[18];
byte readBlockData_Range[18];

MFRC522::StatusCode status;

void setup()
{
  /* Initialize serial communications with the PC */
  Serial.begin(9600);
  /* Initialize SPI bus */
  SPI.begin();
  /* Initialize MFRC522 Module */
  mfrc522.PCD_Init();
  Serial.println("Scan a MIFARE 1K Tag to write data...");
}

void loop()
{

  /* Prepare the key for authentication */
  /* All keys are set to FFFFFFFFFFFFh at chip delivery from the factory */
  for (byte i = 0; i < 6; i++)
  {
    key.keyByte[i] = 0xFF;
  }
  /* Look for new cards */
  /* Reset the loop if no new card is present on RC522 Reader */
  if (!mfrc522.PICC_IsNewCardPresent())
  {
    return;
  }

  /* Select one of the cards */
  if (!mfrc522.PICC_ReadCardSerial())
  {
    return;
  }
  Serial.print("\n");
  Serial.println("**Card Detected**");
  /* Print UID of the Card */
  Serial.print(F("Card UID:"));
  for (byte i = 0; i < mfrc522.uid.size; i++)
  {
    Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
    Serial.print(mfrc522.uid.uidByte[i], HEX);
  }
  Serial.print("\n");
  /* Print type of card (for example, MIFARE 1K) */
  Serial.print(F("PICC type: "));
  MFRC522::PICC_Type piccType = mfrc522.PICC_GetType(mfrc522.uid.sak);
  Serial.println(mfrc522.PICC_GetTypeName(piccType));

  /* Call 'WriteDataToBlock' function, which will write data to the block */
  Serial.print("\n");
  Serial.println("Writing to Data Block...");
  WriteDataToBlock(blockNum_Name, blockData_Name);
  WriteDataToBlock(blockNum_ChargeTimeAC, blockData_ChargeTimeAC);
  WriteDataToBlock(blockNum_ChargeTimeDC, blockData_ChargeTimeDC);
  WriteDataToBlock(blockNum_Port, blockData_Port);
  WriteDataToBlock(blockNum_Range, blockData_Range);

  /* Read data from the same block */
  Serial.print("\n");
  Serial.println("Reading from Data Block...");
  ReadDataFromBlock(blockNum_Name, readBlockData_Name);
  ReadDataFromBlock(blockNum_ChargeTimeAC, readBlockData_ChargeTimeAC);
  ReadDataFromBlock(blockNum_ChargeTimeDC, readBlockData_ChargeTimeDC);
  ReadDataFromBlock(blockNum_Port, readBlockData_Port);
  ReadDataFromBlock(blockNum_Range, readBlockData_Range);
  /* If you want to print the full memory dump, uncomment the next line */
  // mfrc522.PICC_DumpToSerial(&(mfrc522.uid));

  /* Print the data read from block */
  Serial.print("\n");
  Serial.print("Data in Block:");
  Serial.print(blockNum_Name);
  Serial.print(" --> ");
  for (int j = 0; j < 16; j++)
  {
    Serial.write(readBlockData_Name[j]);
  }
  Serial.print("\n");

  Serial.print("\n");
  Serial.print("Data in Block:");
  Serial.print(blockNum_ChargeTimeAC);
  Serial.print(" --> ");
  for (int j = 0; j < 16; j++)
  {
    Serial.write(readBlockData_ChargeTimeAC[j]);
  }
  Serial.print("\n");

  Serial.print("\n");
  Serial.print("Data in Block:");
  Serial.print(blockNum_ChargeTimeDC);
  Serial.print(" --> ");
  for (int j = 0; j < 16; j++)
  {
    Serial.write(readBlockData_ChargeTimeDC[j]);
  }
  Serial.print("\n");

  Serial.print("\n");
  Serial.print("Data in Block:");
  Serial.print(blockNum_Port);
  Serial.print(" --> ");
  for (int j = 0; j < 16; j++)
  {
    Serial.write(readBlockData_Port[j]);
  }
  Serial.print("\n");

    Serial.print("\n");
  Serial.print("Data in Block:");
  Serial.print(blockNum_Range);
  Serial.print(" --> ");
  for (int j = 0; j < 16; j++)
  {
    Serial.write(readBlockData_Range[j]);
  }
  Serial.print("\n");
}

void WriteDataToBlock(int blockNum, byte blockData[])
{
  /* Authenticating the desired data block for write access using Key A */
  status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, blockNum, &key, &(mfrc522.uid));
  if (status != MFRC522::STATUS_OK)
  {
    Serial.print("Authentication failed for Write: ");
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  }
  else
  {
    Serial.println("Authentication success");
  }

  /* Write data to the block */
  status = mfrc522.MIFARE_Write(blockNum, blockData, 16);
  if (status != MFRC522::STATUS_OK)
  {
    Serial.print("Writing to Block failed: ");
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  }
  else
  {
    Serial.println("Data was written into Block successfully");
  }
}

void ReadDataFromBlock(int blockNum, byte readBlockData[]) {
  /* Authenticating the desired data block for Read access using Key A */
  MFRC522::StatusCode status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, blockNum, &key, &(mfrc522.uid));

  if (status != MFRC522::STATUS_OK) {
    Serial.print("Authentication failed for Read: ");
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  } else {
    Serial.println("Authentication success");
  }

  /* Reading data from the Block */
  status = mfrc522.MIFARE_Read(blockNum, readBlockData, &bufferLen);
  if (status != MFRC522::STATUS_OK) {
    Serial.print("Reading failed: ");
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  } else {
    Serial.println("Block was read successfully");
  }
}
