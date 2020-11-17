/****************************************************************
 * Example1_Basics.ino
 * ICM 20948 Arduino Library Demo 
 * Use the default configuration to stream 9-axis IMU data
 * Owen Lyke @ SparkFun Electronics
 * Original Creation Date: April 17 2019
 * 
 * This code is beerware; if you see me (or any other SparkFun employee) at the
 * local, and you've found our code helpful, please buy us a round!
 * 
 * Distributed as-is; no warranty is given.
 ***************************************************************/
//The main purpose of this code is to gather the temp values off of the ICM 20948, 
//put those values into a CSV file with SPIFFS, and send it over FTP

//=================================================
//SPIFFS Indvidual libraries
#include "SPIFFS.h" //For the csv saving protocol
#include <FS.h>
//=================================================
//FTP SERVER Libraries
#ifdef ESP8266
  #include <ESP8266WiFi.h> 
#elif defined ESP32
  #include <WiFi.h>
  #include "SPIFFS.h"
#endif
#include "ESP8266FtpServer.h"
//===SERVER_CREDENTIALS===========================
const char* ssid = "Gianni";
const char* password = "babyboy123";
const char* ftp_user = "esp32";
const char* ftp_password = "esp32";
int TestingSpd = 500;
FtpServer ftpSrv;  
//=================================================

#include "ICM_20948.h"  // Click here to get the library: http://librarymanager/All#SparkFun_ICM_20948_IMU
//#define USE_SPI       // Uncomment this to use SPI

#define SERIAL_PORT Serial

#define SPI_PORT SPI    // Your desired SPI port.       Used only when "USE_SPI" is defined
#define CS_PIN 2        // Which pin you connect CS to. Used only when "USE_SPI" is defined

#define WIRE_PORT Wire  // Your desired Wire port.      Used when "USE_SPI" is not defined
#define AD0_VAL   1     // The value of the last bit of the I2C address. 
                        // On the SparkFun 9DoF IMU breakout the default is 1, and when 
                        // the ADR jumper is closed the value becomes 0
#ifdef AD0_VAL = 1
String SPIFFS_AD0 = "/IDB.csv";
#else
String SPIFFS_AD0 = "/Internal.csv";
#endif

#ifdef USE_SPI
  ICM_20948_SPI myICM;  // If using SPI create an ICM_20948_SPI object
#else
  ICM_20948_I2C myICM;  // Otherwise create an ICM_20948_I2C object
#endif


void setup() {

Serial.begin(115200);
//===FTP_SERVER_CODE=================
WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  Serial.println(F("Inizializing FS..."));
  // FTP Setup, ensure SPIFFS is started before try to use ftp methods  
#ifdef ESP32       
  if (SPIFFS.begin(true)) {
#endif
    Serial.println("SPIFFS opened!");
    ftpSrv.begin(ftp_user, ftp_password);   //username, password for ftp
  
  }
 


//SPIFFS Startup
//===================================================  
  //SPIFFS Mount Code Detection. 

  bool success = SPIFFS.begin();
 
  if (!success) {
    Serial.println("Error mounting the file system");
    return;
  }
 //Setup code to begin the documents and open them for reading and writing.
  File file = SPIFFS.open(SPIFFS_AD0, "a+");
   
  if (!file) {
    Serial.println("Error opening file for writing");
    return;
  }


  
  //End of Spiffs Starter
  //================================================
  //Actaully getting data off the ICM and assigning the allotments
  SERIAL_PORT.begin(115200);
  while(!SERIAL_PORT){};
  
#ifdef USE_SPI
    SPI_PORT.begin();
#else
    WIRE_PORT.begin();
    WIRE_PORT.setClock(400000);
#endif
  
  bool initialized = false;
  while( !initialized ){

#ifdef USE_SPI
    myICM.begin( CS_PIN, SPI_PORT ); 
#else
    myICM.begin( WIRE_PORT, AD0_VAL );
#endif

    ftpSrv.handleFTP();
    file.print( F("Initialization of the sensor returned: ") ); //Using the file.print command to send to the text.csv document
    file.println( myICM.statusString() ); //Sends this directly to the text.csv
    if( myICM.status != ICM_20948_Stat_Ok ){ //|| SPIFFS.exists("/fileRun.txt") == false
      file.println( "Trying again..." );
    if(SPIFFS.exists("/runFile.txt") == false){
      file.println("Insert the runFile.txt to start logging and");    }
      
      ftpSrv.handleFTP();
      delay(500);

//==================================================================
//Starting the text.csv document output to Serial Monitor with Spiffs.

//Running code to constantly read all new updates to the text.csv file by making the command "file2.----()"

File file2 = SPIFFS.open(SPIFFS_AD0, "r"); //Defining that file2 refrences reading by using "r"

  if (!file2) {
    Serial.println("Failed to open file for reading");
    return;
  }
 
Serial.println("File Content:"); //Formatting formality to see the content of text.csv within Serial Monitor.
 
while (file2.available()) {
 
Serial.write(file2.read()); //Reading the document itself.
}
   file2.close();
  file.close();

//======================================================================
    }else{
      initialized = true;
      
    } }
}

void loop() {
//Spiffs requires the document to be opened in order to make changes, this needs to exist in every | void(){} |, along with a file.close;
 
File file = SPIFFS.open(SPIFFS_AD0, "a+");//Spiffs requirement to open up the document for appending.


ftpSrv.handleFTP(); //Server code, making sure the SSID and Pass are correct for FTP.
  
  if( myICM.dataReady() && SPIFFS.exists("/runFile.txt") == true){
    ftpSrv.handleFTP();
    Serial.println("Currently logging all ICM data");
    myICM.getAGMT();                // The values are only updated when you call 'getAGMT'
//    printRawAGMT( myICM.agmt );     // Uncomment this to see the raw values, taken directly from the agmt structure
    printScaledAGMT( myICM.agmt);   // This function takes into account the sclae settings from when the measurement was made to calculate the values with units
    ftpSrv.handleFTP();
    delay(TestingSpd);
    }else{
   file.println("Waiting for data");
    delay(1000);
  }
  
  //Spiffs close
  file.close();


}


// Below here are some helper functions to print the data nicely!

void printPaddedInt16b( int16_t val ){
  
  //Spiffs requirement double check
  File file = SPIFFS.open(SPIFFS_AD0, "a+");

  // end spiffs


  
  if(val > 0){
    file.print(" ");
    if(val < 10000){ file.print("0"); }
    if(val < 1000 ){ file.print("0"); }
    if(val < 100  ){ file.print("0"); }
    if(val < 10   ){ file.print("0"); }
  }else{
    file.print("-");
    if(abs(val) < 10000){ file.print("0"); }
    if(abs(val) < 1000 ){ file.print("0"); }
    if(abs(val) < 100  ){ file.print("0"); }
    if(abs(val) < 10   ){ file.print("0"); }
  }
  file.print(abs(val));
   //Spiffs close
  file.close();
}

void printRawAGMT( ICM_20948_AGMT_t agmt){
  //Spiffs requirement double check
  File file = SPIFFS.open(SPIFFS_AD0, "a+");
  // end spiffs
 /* 
  file.print("RAW. Acc [ ");
  printPaddedInt16b( agmt.acc.axes.x );
  file.print(", ");
  printPaddedInt16b( agmt.acc.axes.y );
  file.print(", ");
  printPaddedInt16b( agmt.acc.axes.z );
  file.print(" ], Gyr [ ");
  printPaddedInt16b( agmt.gyr.axes.x );
  file.print(", ");
  printPaddedInt16b( agmt.gyr.axes.y );
  file.print(", ");
  printPaddedInt16b( agmt.gyr.axes.z );
  file.print(" ], Mag [ ");
  printPaddedInt16b( agmt.mag.axes.x );
  file.print(", ");
  printPaddedInt16b( agmt.mag.axes.y );
  file.print(", ");
  printPaddedInt16b( agmt.mag.axes.z );
  */
  file.print(" ], Tmp [ ");
  printPaddedInt16b( agmt.tmp.val );
  file.print(" ]");
  file.println();
   //Spiffs close
  file.close();
}


void printFormattedFloat(float val, uint8_t leading, uint8_t decimals){
  //Spiffs requirement double check
  File file = SPIFFS.open(SPIFFS_AD0, "a+");
  // end spiffs
  
  float aval = abs(val);
  if(val < 0){
    file.print("-");
  }else{
    file.print(" ");
  }
  for( uint8_t indi = 0; indi < leading; indi++ ){
    uint32_t tenpow = 0;
    if( indi < (leading-1) ){
      tenpow = 1;
    }
    for(uint8_t c = 0; c < (leading-1-indi); c++){
      tenpow *= 10;
    }
    if( aval < tenpow){
      file.print("0");
    }else{
      break;
    }
  }
  if(val < 0){
    file.print(-val, decimals);
  }else{
    file.print(val, decimals);
  }

  
   //spiffs close
  file.close();
}

void printScaledAGMT( ICM_20948_AGMT_t agmt){ // This chunk is what is actually outputted within the code, please not this has not been formally tested to its fullest extent.
  //Spiffs requirement double check
  File file = SPIFFS.open(SPIFFS_AD0, "a+");
  // end spiffs
/*  
  file.print("Scaled. Acc (mg) [ ");
  printFormattedFloat( myICM.accX(), 5, 2 );
  file.print(", ");
  printFormattedFloat( myICM.accY(), 5, 2 );
  file.print(", ");
  printFormattedFloat( myICM.accZ(), 5, 2 );
  file.print(" ], Gyr (DPS) [ ");
  printFormattedFloat( myICM.gyrX(), 5, 2 );
  file.print(", ");
  printFormattedFloat( myICM.gyrY(), 5, 2 );
  file.print(", ");
  printFormattedFloat( myICM.gyrZ(), 5, 2 );
  file.print(" ], Mag (uT) [ ");
  printFormattedFloat( myICM.magX(), 5, 2 );
  file.print(", ");
  printFormattedFloat( myICM.magY(), 5, 2 );
  file.print(", ");
  printFormattedFloat( myICM.magZ(), 5, 2 );
  */
  //file.print(" ], Tmp (C) [ ");
  printFormattedFloat( myICM.temp(), 5, 2 );
  //file.print(" ]");
  file.println();

  
   //Spiffs close
  file.close();
}
