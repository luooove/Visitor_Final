#include <avr/sleep.h>
#include <AT24CXXX.h>
#include <Wire.h>
AT24CXXX ROM_512;
#include "DS3231.h"
DS3231 RTC; //Create the DS3231 object
const int theDeviceAddress = 0x57;
#include "OLED.h"
#include <MsTimer2.h>               //定时器库的 头文件
char Director1 = 0;
int VisitorsIn = 0;
int VisitorsOut = 0;
int count = 0;
char flag = 0;
const int buttonPin = 4;     // the number of the pushbutton pin
int buttonState = 0;         // variable for reading the pushbutton status
//-----------------------初始化------------------------------
void setup()
{
  pinMode(buttonPin, INPUT); //

  Wire.begin();        //启动I2C通信
  LEDPIN_Init();       //初始化液晶
  LED_Init();
  LED_P6x8Str(20, 0, "Vistor_Flow_Rate:");
  LED_P6x8Str(0, 2, "PeopleIn:");
  LED_P6x8Str(0, 4, "PeopleOut:");

  ROM_512.Memory_Size(512);     //例化存储器
  Serial.begin(9600);           //初始化串口
  //读取外部存储器中的总人数，进去人数和出来人数
  count = ROM_512.ReadByte(0x57 , 0) + ROM_512.ReadByte(0x57 , 1) * 255;
  VisitorsIn = ROM_512.ReadByte(0x57 , 2) + ROM_512.ReadByte(0x57 , 3) * 255;
  VisitorsOut = ROM_512.ReadByte(0x57 , 4) + ROM_512.ReadByte(0x57 , 5) * 255;
  //把进去和出来的人数显示
  LED_PrintValueI(60, 2, VisitorsIn);
  LED_PrintValueI(60, 4, VisitorsOut);
  //使能外部中断
  attachInterrupt(0, Detect1, FALLING  );//下降沿说明有
  attachInterrupt(1, Detect2, FALLING );//下降沿说明人
}


//-----------------------中断服务程序，检测人进出------------------------------
void Detect1()
{
  sleep_disable();
  if (Director1 == 0) //means people is go in and not yet pass
  {
    Director1 = 1;
    Serial.println("timestart");
    MsTimer2::set(2000, wait);        // 中断设置函数，每 1s 进入一次中断
    MsTimer2::start();
    //	Serial.println("detect1_0");
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
  sleep_disable();
  if (Director1 == 0) //means people is go out and not yet pass
  {
    Director1 = 2;
    Serial.println("timestart");
    MsTimer2::set(2000, wait);        // 中断设置函数，每 1s 进入一次中断
    MsTimer2::start();
    //	Serial.println("detect2_0");
  }
  if (Director1 == 1)
  {
    Director1 = 0;
    VisitorsIn ++;
    count++;
    flag = 2;
  }
}
//-----------------------定时器中断服务程序，如果2秒内人还未通过就清除标志------------------------------
void wait()
{
  Serial.println("timeout");
  if ((Director1 == 1))
  {
    Director1 = 0;
    MsTimer2::stop();
  }
  if ((Director1 == 2))
  {
    Director1 = 0;
    MsTimer2::stop();
  }
  MsTimer2::stop();
}

//-----------------------保存数据，并显示在ＯＬＥＤ上------------------------------
//-----------------------保存的格式为：年 月 日，进入人数，出来人数------------------------------
void SaveData()
{
  byte buffer[10];
  DateTime now = RTC.now(); //get the current date-time
  buffer[0] = now.year() - 2000;
  buffer[1] = now.month();
  buffer[2] = now.date();
  buffer[3] = now.hour();
  buffer[4] = now.minute();
  buffer[5] = now.second();
  buffer[6] = VisitorsIn % 255;
  buffer[7] = (VisitorsIn >> 8) % 255; //

  buffer[8] = VisitorsOut % 255;
  buffer[9] = (VisitorsOut >> 8) % 255;
  //save count
  ROM_512.WriteByte(0x57 , 0 , (byte)count % 255);
  ROM_512.WriteByte(0x57 , 1 , (byte)(count >> 8) % 255);

  ROM_512.WriteByte(0x57 , 2 , (byte)VisitorsIn % 255);
  ROM_512.WriteByte(0x57 , 3 , (byte)(VisitorsIn >> 8) % 255);
  ROM_512.WriteByte(0x57 , 4 , (byte)VisitorsOut % 255);
  ROM_512.WriteByte(0x57 , 5 , (byte)(VisitorsOut >> 8) % 255);

  ROM_512.WriteByte(0x57 , 6 + count * 10 , buffer[0]);
  ROM_512.WriteByte(0x57 , 6 + count * 10 + 1 , buffer[1]);
  ROM_512.WriteByte(0x57 , 6 + count * 10 + 2 , buffer[2]);
  ROM_512.WriteByte(0x57 , 6 + count * 10 + 3 , buffer[3]);
  ROM_512.WriteByte(0x57 , 6 + count * 10 + 4 , buffer[4]);
  ROM_512.WriteByte(0x57 , 6 + count * 10 + 5 , buffer[5]);
  ROM_512.WriteByte(0x57 , 6 + count * 10 + 6 , buffer[6]);
  ROM_512.WriteByte(0x57 , 6 + count * 10 + 7 , buffer[7]);
  ROM_512.WriteByte(0x57 , 6 + count * 10 + 8 , buffer[8]);
  ROM_512.WriteByte(0x57 , 6 + count * 10 + 9 , buffer[9]);



  LED_PrintValueI(60, 2, VisitorsIn);
  LED_PrintValueI(60, 4, VisitorsOut);
}

//-----------------------读取数据并打印------------------------------
void printSaveData()
{ byte buffer[10];

  buffer[0] = ROM_512.ReadByte(0x57 , 6 + count * 10);
  buffer[1] = ROM_512.ReadByte(0x57 , 6 + count * 10 + 1);
  buffer[2] = ROM_512.ReadByte(0x57 , 6 + count * 10 + 2);
  buffer[3] = ROM_512.ReadByte(0x57 , 6 + count * 10 + 3);
  buffer[4] = ROM_512.ReadByte(0x57 , 6 + count * 10 + 4);
  buffer[5] = ROM_512.ReadByte(0x57 , 6 + count * 10 + 5);

  buffer[6] = ROM_512.ReadByte(0x57 , 6 + count * 10 + 6);
  buffer[7] = ROM_512.ReadByte(0x57 , 6 + count * 10 + 7);

  buffer[8] = ROM_512.ReadByte(0x57 , 6 + count * 10 + 8);
  buffer[9] = ROM_512.ReadByte(0x57 , 6 + count * 10 + 9);

  Serial.println((ROM_512.ReadByte(0x57 , 0)) + ((ROM_512.ReadByte(0x57 , 1)) << 8), DEC);

  Serial.print(2000 + buffer[0], DEC);  //year
  Serial.print('-');
  Serial.print(buffer[1], DEC);//month
  Serial.print('-');
  Serial.print(buffer[2], DEC);//day
  Serial.print('/');

  Serial.print(buffer[3], DEC);//hour
  Serial.print(':');
  Serial.print(buffer[4], DEC);//min
  Serial.print(':');
  Serial.print(buffer[5], DEC);//sec
  Serial.print('#');

  int temp = buffer[6] + buffer[7] * 255;
  Serial.print(temp, DEC);
  Serial.print('/');
  int temp2 = buffer[8] + buffer[9] * 255;
  Serial.print(temp2, DEC);
  Serial.println();
  delay(10);

}
void ClearData()
{
  ROM_512.WriteByte(0x57 , 0 , 0);
  ROM_512.WriteByte(0x57 , 1 , 0);

  ROM_512.WriteByte(0x57 , 2 , 0);
  ROM_512.WriteByte(0x57 , 3 , 0);
  ROM_512.WriteByte(0x57 , 4 , 0);
  ROM_512.WriteByte(0x57 , 5 , 0);
}

//-----------------------主循环------------------------------
void loop()
{

  //主函数，因为把服务程序放在中断服务程序中处理会出错，因此通过flag的方式来进行中断服务程序的处理。
  switch (flag) {
    case 1:

      // do something
      SaveData();
      //   printSaveData();
      flag = 0;
      break;
    case 2:
      // do something
      SaveData();
      //   printSaveData();
      flag = 0;
      break;
    default:;
      // do something
  }
  buttonState = digitalRead(buttonPin);
  if (buttonState == LOW) {
    ClearData();
  }

  //   disable ADC
  ADCSRA = 0;

  set_sleep_mode (SLEEP_MODE_PWR_DOWN);
  sleep_enable();

  // Do not interrupt before we go to sleep, or the
  // ISR will detach interrupts and we won't wake.
  noInterrupts ();
  // turn off brown-out enable in software
  // BODS must be set to one and BODSE must be set to zero within four clock cycles
  MCUCR = bit (BODS) | bit (BODSE);
  //  // The BODS bit is automatically cleared after three clock cycles
  MCUCR = bit (BODS);

  // We are guaranteed that the sleep_cpu call will be done
  // as the processor executes the next instruction after
  // interrupts are turned on.
  interrupts ();  // one cycle
  sleep_cpu ();   // one cycle
}
