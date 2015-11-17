#include <LiquidCrystal_I2C.h>
#include <Wire.h> 
#include <dht.h>
#define uchar unsigned char
#define uint unsigned int
//
//    FILE: dht11_test.ino
//  AUTHOR: Rob Tillaart
// VERSION: 0.1.01
// PURPOSE: DHT library test sketch for DHT11 && Arduino
//     URL:
//
// Released to the public domain
//



dht DHT;

#define DHT11_PIN 2
LiquidCrystal_I2C lcd(0x27,16,2);
int tem=0,hum=0;

//PM data from KM-100
int PM_SETPin = 5;
int PM_RESETPin = 4;
uchar inByte = 0;         // incoming serial byte from PM2.5
uchar recData[24];
int PM1_Update = 0;
int PM25_Update = 0;
int PM10_Update = 0;
int PM1_Last = 0;
int PM25_Last = 0;
int PM10_Last = 0;
boolean RecStar = false;
void setup()
{
  Serial.begin(9600);
  Serial.println("Beginning...");
  Serial.println();
  Serial.println("Type,\tstatus,\tHumidity (%),\tTemperature (C)");
  
  pinMode(PM_SETPin,OUTPUT);
  pinMode(PM_RESETPin, OUTPUT);
  digitalWrite(PM_SETPin, HIGH);
  digitalWrite(PM_RESETPin, HIGH);
  
   lcd.init();
  delay(100);
  lcd.clear();
  lcd.init();
  lcd.backlight();  

  lcd.setCursor(0,0);
  lcd.print(" SWJTU_E.M.S ");
  lcd.setCursor(0,1);
  lcd.print("CD Site IoT Team");
  delay(3000);  
  
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Pls Wait For 10s");
  lcd.setCursor(0,1);
  lcd.print("Sensor Stabling ");
  lcd.backlight();  
  delay(10000);  
  
}

void loop()
{
  // READ DATA
  Serial.print("DHT11, \t");
  int chk = DHT.read11(DHT11_PIN);
  switch (chk)
  {
    case DHTLIB_OK:  
		//Serial.print("OK,\t"); 
		break;
    case DHTLIB_ERROR_CHECKSUM: 
		Serial.print("Checksum error,\t"); 
		break;
    case DHTLIB_ERROR_TIMEOUT: 
		Serial.print("Time out error,\t"); 
		break;
    case DHTLIB_ERROR_CONNECT:
        Serial.print("Connect error,\t");
        break;
    case DHTLIB_ERROR_ACK_L:
        Serial.print("Ack Low error,\t");
        break;
    case DHTLIB_ERROR_ACK_H:
        Serial.print("Ack High error,\t");
        break;
    default: 
		Serial.print("Unknown error,\t"); 
		break;
  }
  // DISPLAY DATA
  hum =DHT.humidity;
  tem=DHT.temperature;
  Serial.print(DHT.humidity, 1);
  Serial.print(",\t");
  Serial.println(DHT.temperature, 1);

  delay(2000);
  Sensor_PM();
   refresh_display();
}
void refresh_display()
{

  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("  SWJTU_EMC  ");
  
  lcd.setCursor(0,1);
  lcd.print("PM2.5:");
  lcd.setCursor(7,1);
  lcd.print(PM25_Update);
  lcd.setCursor(11,1);
  lcd.print("ug/m3");
    delay(3000);
    
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Temperature:");
  lcd.setCursor(12,0);
  lcd.print(tem);
  lcd.print("C");
  
  lcd.setCursor(0,1);
  lcd.print("Humidity:");
  lcd.setCursor(12,1);
  lcd.print(hum);
  lcd.print("%");
  delay(3000);

    
    
  

}
void Sensor_PM()
{
  read_PM();
  boolean flag = verify_PM();
 flag = true;
  //check PM device recvice data CRC result
  if(flag == true)
  {
    PM1_Update = recData[4]*256 + recData[5];
    PM25_Update = recData[6]*256 + recData[7];
    PM10_Update = recData[8]*256 + recData[9];
    Serial.print("PM2.5 data:");
    Serial.println(PM25_Update);
      //pass;
  }
  else
  {
    lcd.setCursor(0,0);
    lcd.print(" Data Err, Drop ");
    delay(1000);
    PM1_Update = PM1_Last; 
    PM25_Update = PM25_Last; 
    PM10_Update = PM10_Last;
    Serial.println("wrong PM");
  }
  memset(recData,0x00,24);
}
void read_PM()
{
  int i=0;

  //digitalWrite(PM_SETPin, HIGH);
  //delay (800);
  
 // Serial.println("PM Sensor Data:");
 while (Serial.available() > 0) 
  {
      // get incoming byte:
      inByte = Serial.read();
      // read first analog input:
      if (inByte == 0x42)
      {
        RecStar = true;
      }  
      if (RecStar == true)
      {
        recData[i]=inByte;
        //Serial.print(recData[i], HEX);
//        Serial.print(0x0F & (inByte>>4), HEX);
//        Serial.print(0x0F & inByte,HEX);
//        Serial.print("-");

        i++;
        if (i==24)
        {
          i=0;
          RecStar = false;
         break;
        }
      }
  }
 
  //digitalWrite(PM_SETPin,LOW);
  //delay(200);
}
boolean verify_PM()
{
  uchar i=0;
  int CRC = 0;
  
  if (recData[0]==0x42 && recData[1]==0x4d){
   // Serial.println("CRC data:");
    for (i=0;i<22;i++){
      CRC+=recData[i];
//      Serial.println(CRC);
      delay(5);
     
    }
    //Serial.print(CRC, HEX);
  }  
  
  if (CRC == (recData[22]*256 + recData[23])){
    return true;
  }
  else{
    return false;
  }
}
//
// END OF FILE
//
