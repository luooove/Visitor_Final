#include <AT24CXXX.h>
#include <Wire.h>
#include "DS3231.h"
AT24CXXX ROM_512;
DS3231 RTC; //Create the DS3231 object
DateTime dt(2015, 8, 29, 14, 51, 0, 5);
void setup()
{
	Wire.begin();
	ROM_512.Memory_Size(512);
	RTC.begin();
    RTC.adjust(dt); //Adjust date-time as defined 'dt' above 
	Serial.begin(9600);

}

void loop()
{
	int unsigned i;
	for(i=0;i<1000;i++)
	{
		Serial.println(i);
		ROM_512.WriteByte(0x57,i,0x00);	
	}


	for(i=0;i<1000;i++)
	{
		Serial.println((ROM_512.ReadByte(0x57 , i )), HEX);
	}
}