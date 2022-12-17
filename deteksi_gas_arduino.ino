//library
#include <LiquidCrystal_I2C.h>
#include <MQUnifiedsensor.h>
#include <SoftwareSerial.h>

//definisi board untuk lib MQ sensor
#define Board "Arduino UNO"

//definisi pin
#define Pin_MQ2 A0
#define Pin_MQ4 A2
#define Pin_MQ7 A1

//definisi tipe mq
#define Type_MQ2 "MQ-2"
#define Type_MQ4 "MQ-4"
#define Type_MQ7 "MQ-7"

//definisi resolusi tegangan
#define Voltage_Resolution 5

//definisi ADC
#define ADC_Bit_Resolution 10

//definisi rasio udara bersih MQ sensor
#define RatioMQ2CleanAir 9.83 // RS/R0 = 9.83 ppm
#define RatioMQ4CleanAir 4.4 // RS/R0 = 4.4 ppm
#define RatioMQ7CleanAir 27.5 // RS/R0 = 27.5 ppm

//masukkan data spesifikasi untuk library setiap sensor
MQUnifiedsensor MQ2(Board, Voltage_Resolution, ADC_Bit_Resolution, Pin_MQ2, Type_MQ2);
MQUnifiedsensor MQ4(Board, Voltage_Resolution, ADC_Bit_Resolution, Pin_MQ4, Type_MQ4);
MQUnifiedsensor MQ7(Board, Voltage_Resolution, ADC_Bit_Resolution, Pin_MQ7, Type_MQ7);

//definisi pin komponen
#define buzzer 2
#define led_hijau 3
#define led_merah 4
#define fan1 5
#define fan2 6
#define button_on 7
#define button_off 8

//definisi data sensor
long int sLPG = 0;
long int sCH4 = 0;
long int sCO = 0;

//int dataSwitch; //data switch dari blynk melalui node mcu
int dataButton; //data button
String sdata; //data yang dikirimkan ke node mcu

void alarmOn();
void alarmOff();

SoftwareSerial espSerial(9,10); //pin serial
LiquidCrystal_I2C lcd(0x27, 16, 2); //alamat dan ukuran lcd

void setup() {
  lcd.begin();
  Serial.begin(9600);
  espSerial.begin(115200);
  
  pinMode(buzzer,OUTPUT);
  pinMode(led_hijau,OUTPUT);
  pinMode(led_merah,OUTPUT);
  pinMode(fan1,OUTPUT);
  pinMode(fan2,OUTPUT);
  pinMode(button_on,INPUT);
  pinMode(button_off,INPUT);

  MQ2.setRegressionMethod(1);
  MQ2.setA(574.25); MQ2.setB(-2.222); //konfigurasi untuk LPG
  MQ4.setRegressionMethod(1);
  MQ4.setA(1012.7); MQ4.setB(-2.786); //konfigurasi untuk CH4 (metana)
  MQ7.setRegressionMethod(1);
  MQ7.setA(99.042); MQ7.setB(-1.518); //konfigurasi untuk CO (karbon monoksida)

  MQ2.init();
  MQ4.init();
  MQ7.init();
  
  //kalibrasi Sensor MQ
  float calcR0_MQ2 = 0;
  float calcR0_MQ4 = 0;
  float calcR0_MQ7 = 0;
  for(int i = 1; i<=10; i ++)
  {
    MQ2.update();
    calcR0_MQ2 += MQ2.calibrate(RatioMQ2CleanAir);
    MQ4.update();
    calcR0_MQ4 += MQ4.calibrate(RatioMQ4CleanAir);
    MQ7.update();
    calcR0_MQ7 += MQ7.calibrate(RatioMQ2CleanAir);
  }
  
  MQ2.setR0(calcR0_MQ2/10);
  MQ4.setR0(calcR0_MQ4/10);
  MQ7.setR0(calcR0_MQ7/10);
}

void loop() {
  //update nilai sensor MQ
  MQ2.update();
  MQ4.update();
  MQ7.update();
  delay(1000);

  //baca nilai sensor MQ
  sLPG = MQ2.readSensor();
  sCH4 = MQ4.readSensor();
  sCO = MQ7.readSensor();

  if((digitalRead(button_on) == HIGH)){
    dataButton = 1;
  }else if((digitalRead(button_off) == HIGH)){
    dataButton = 0;
  }

  //Tampilkan ke LCD
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("LPG=");
  lcd.setCursor(4,0);
  lcd.print(sLPG);

  lcd.setCursor(0,1);
  lcd.print("CH4=");
  lcd.setCursor(4,1);
  lcd.print(sCH4);

  lcd.setCursor(10,0);
  lcd.print("CO=");
  lcd.setCursor(13,0);
  lcd.print(sCO);

  if(sLPG >= 1000 || sCH4 >= 1000 || sCO >= 50 || dataButton == 1){
    alarmOn();      
  }else if(sLPG < 1000 || sCH4 < 1000 || sCO < 50 || dataButton == 0){
    alarmOff();
  }

  if(espSerial.available() == 0){
    sdata = sdata + sLPG + "," + sCH4 + "," + sCO;
    espSerial.println(sdata);
    Serial.println(sdata);
    delay(1000);
    sdata = "";
  }
  /*if(espSerial.available() > 0){
    dataSwitch = espSerial.parseInt();
    delay(500);
    if(dataSwitch == 1){
      alarmOn();
    }else if(dataSwitch == 0){
      alarmOff();
    }*/
}

void alarmOn(){ //fungsi untuk alarm hidup
  digitalWrite(fan1,HIGH);
  digitalWrite(fan2,HIGH);
  digitalWrite(led_hijau,LOW);
  digitalWrite(led_merah,HIGH);
  digitalWrite(buzzer,HIGH);
  lcd.setCursor(10,1);
  lcd.print("on");
}
void alarmOff(){ //fungsi untuk alarm mati
  digitalWrite(fan1,LOW);
  digitalWrite(fan2,LOW);
  digitalWrite(led_hijau,HIGH);
  digitalWrite(led_merah,LOW);
  digitalWrite(buzzer,LOW);
  lcd.setCursor(10,1);
  lcd.print("off");
}  
