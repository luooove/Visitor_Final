#include <AT24CXXX.h>
#include <Wire.h>
AT24CXXX ROM_512;
void setup()
{
	Wire.begin();
	ROM_512.Memory_Size(512);
	Serial.begin(9600);

}

void loop()
{
	unsigned int  i;
	unsigned int  count;
	byte buffer[10];
	count = ROM_512.ReadByte(0x57 ,0) + ROM_512.ReadByte(0x57 ,1) *255;
	Serial.println("read...");
	Serial.println(ROM_512.ReadByte(0x57 ,0));
	Serial.println(ROM_512.ReadByte(0x57 ,1));
	Serial.println(count);
	
	for(i=1;i<=count;i++)
	{
	buffer[0] = ROM_512.ReadByte(0x57 ,2 + i * 10);
	buffer[1] = ROM_512.ReadByte(0x57 ,2 + i * 10 + 1);
	buffer[2] = ROM_512.ReadByte(0x57 ,2 + i * 10 + 2);
	buffer[3] = ROM_512.ReadByte(0x57 ,2 + i * 10 + 3);
	buffer[4] = ROM_512.ReadByte(0x57 ,2 + i * 10 + 4);
	buffer[5] = ROM_512.ReadByte(0x57 ,2 + i * 10 + 5);

	buffer[6] = ROM_512.ReadByte(0x57 ,2 + i * 10 + 6);
	buffer[7] = ROM_512.ReadByte(0x57 ,2 + i * 10 + 7);

	buffer[8] = ROM_512.ReadByte(0x57 ,2 + i * 10 + 8);
	buffer[9] = ROM_512.ReadByte(0x57 ,2 + i * 10 + 9);

//	Serial.println((ROM_512.ReadByte(0x57 ,0))+((ROM_512.ReadByte(0x57 ,1))<<8),DEC);
    Serial.print(i);
    Serial.print(": ");
    Serial.print(2000 + buffer[0], DEC);  //year
    Serial.print('.');
    Serial.print(buffer[1], DEC);//month
    Serial.print('.');
    Serial.print(buffer[2], DEC);//day
    Serial.print(' ');

    Serial.print(buffer[3], DEC);//hour
    Serial.print(':');
    Serial.print(buffer[4], DEC);//min
    Serial.print(':');
    Serial.print(buffer[5], DEC);//sec
    Serial.print("  PeopleIn: ");

    int temp = buffer[6] + buffer[7] * 255;
    Serial.print(temp, DEC);
    Serial.print("  PeopleIn: ");
    int temp2 = buffer[8] + buffer[9] * 255;
    Serial.print(temp2, DEC);
    Serial.println();
	}
	
    delay(10);


}