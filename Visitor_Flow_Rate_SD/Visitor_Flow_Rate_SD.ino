 /*

 The circuit:
 * analog sensors on analog ins 0, 1, and 2
 * SD card attached to SPI bus as follows:
 ** MOSI - pin 11
 ** MISO - pin 12
 ** CLK - pin 13
 ** CS - pin 4

  OLED          arduino
     D0-----------10
     D1-----------9
     RST----------5
     DC-----------6
     VCC----------5V
     GND----------GND


     */
     #include <avr/sleep.h>
     
     #include <SPI.h>
     #include <SD.h>
     #include "OLED.h"
     #include "DS3231.h"

DS3231 RTC; //Create the DS3231 object
//#include <Adafruit_ssd1306syp.h>
//#define SDA_PIN_SD 8
//#define SCL_PIN_SD 9
//Adafruit_ssd1306syp display(SDA_PIN_SD,SCL_PIN_SD);
#include <Wire.h>
const int chipSelect = 4;
char Director1 = 0;
int VisitorsIn = 0;
int VisitorsOut = 0;
int count = 0;
char flag = 0;

void setup()
{
	
	
	Serial.begin(9600);
	 Wire.begin();        //启动I2C通信
	 // LEDPIN_Init();
	 // LED_Init();
	 // LED_P6x8Str(20, 0, "Vistor_Flow_Rate:");
	 // LED_P6x8Str(0, 2, "PeopleIn:");
	 // LED_P6x8Str(0, 4, "PeopleOut:");
	 // LED_PrintValueI(60, 2, VisitorsIn);
	 // LED_PrintValueI(60, 4, VisitorsOut);
	 if (!SD.begin(chipSelect)) {
	 	Serial.println("Card failed, or not present");
   // don't do anything more:
   return;
}
Serial.println("card initialized.");
//  attachInterrupt(0, Detect1, FALLING  );//下降沿说明有
 // attachInterrupt(1, Detect2, FALLING );//下降沿说明人
delay(1000);
	display.initialize();
}


void Detect1()
{
	noInterrupts();
	if (Director1 == 0) //means people is go in and not yet pass
	{
		Director1 = 1;
		Serial.println("detect1_0");
	}
  if (Director1 == 2) //means people is going out
  {
  	Director1 = 0;
  	VisitorsOut ++;
  	count++;
  	flag = 1;
  }
}
void Detect2()
{
	noInterrupts();
	if (Director1 == 0) //means people is go out and not yet pass
	{
		Director1 = 2;
		Serial.println("detect2_0");
	}
	if (Director1 == 1)
	{
		Director1 = 0;
		VisitorsIn ++;
		count++;
		flag = 2;
	}
}


void SaveData()
{
	String dataString =" ";
	int buffer[10];
  DateTime now = RTC.now(); //get the current date-time
  buffer[0] = now.year();
  buffer[1] = now.month();
  buffer[2] = now.date();
  buffer[3] = now.hour();
  buffer[4] = now.minute();
  buffer[5] = now.second();
  buffer[6] = VisitorsIn;
  buffer[7] = VisitorsOut;
  //save count
  dataString += String(buffer[0]);  //year
  dataString += ".";
  dataString += String(buffer[1]);  //month
  dataString += ".";
  dataString += String(buffer[2]);  //date
  dataString += "-";
  dataString += String(buffer[3]);	//hour
  dataString += ":";
  dataString += String(buffer[4]);  //minute
  dataString += ":";
  dataString += String(buffer[5]);  //second
  dataString += "  PeopleIn:";
  dataString += String(buffer[6]);
  dataString += "  PeopleOut:";
  dataString += String(buffer[7]);
  // File dataFile = SD.open("datalog.txt", FILE_WRITE);
  // if (dataFile) {
  // 	dataFile.println(dataString);
  // 	dataFile.close();
  //   // print to the serial port too:
  //   Serial.println(dataString);
//}


// LED_PrintValueI(60, 2, VisitorsIn);
// LED_PrintValueI(60, 4, VisitorsOut);
interrupts();
}





void loop()
{
	switch (flag) {
		case 1:

      // do something
      SaveData();
     // printSaveData();
     flag = 0;
     break;
     case 2:
      // do something
      SaveData();
    //  printSaveData();
    flag = 0;
    break;
    default:;
      // do something
  }
   display.drawLine(0, 0, 127, 63,WHITE);
  display.update();
  delay(1000);
  display.clear();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  display.println("Hello, world!");
  display.setTextColor(BLACK, WHITE); // 'inverted' text
  display.println(3.141592);
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.print("0x"); display.println(0xDEADBEEF, HEX);
  display.update();
  delay(2000);
  display.clear();
}