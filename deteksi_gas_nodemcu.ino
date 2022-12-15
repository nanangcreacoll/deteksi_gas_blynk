#define BLYNK_PRINT Serial
#include <SimpleTimer.h>
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <SoftwareSerial.h>

#define BLYNK_TEMPLATE_ID "TMPLHEW_HmIo"
#define BLYNK_DEVICE_NAME "Deteksi Gas IoT"
#define BLYNK_AUTH_TOKEN "SWpaRiBcdCGps8VeoIrGot5y5Kl70SNY"

char auth[] = "SWpaRiBcdCGps8VeoIrGot5y5Kl70SNY";

char ssid[] = "realme 5i";
char pass[] = "qwerty123";

int dataSwitch;

SimpleTimer timer;

String myString;
char rdata;

long int rLPG,rCH4,rCO,rButton;

void myTimerEvent(){
  Blynk.virtualWrite(V3,millis()/1000);
}

void setup() {
  Serial.begin(115200);
  Blynk.begin(auth,ssid,pass,"blynk.cloud",80);
  timer.setInterval(1000L,LPGval);
  timer.setInterval(1000L,CH4val);
  timer.setInterval(1000L,COval);
  timer.setInterval(1000L,ButtonVal);
}

void loop() {
  if(Serial.available() == 0){
    Blynk.run();
    timer.run();
  }
  if(Serial.available() > 0){
    rdata = Serial.read();
    myString = myString+rdata;
    Serial.print(rdata);
    if(rdata = '\n'){
      String l = getValue(myString,',',0);
      String m = getValue(myString,',',1);
      String n = getValue(myString,',',2);
      String o = getValue(myString,',',3);

      rLPG = l.toInt();
      rCH4 = m.toInt();
      rCO = n.toInt();
      rButton = o.toInt();      

      myString = "";
    }
  }
}

void LPGval(){
  int sdata = rLPG;
  Blynk.virtualWrite(V0, sdata);
}

void CH4val(){
  int sdata = rCH4;
  Blynk.virtualWrite(V1, sdata);
}

void COval(){
  int sdata = rCO;
  Blynk.virtualWrite(V2, sdata);
}

void ButtonVal(){
  int sdata = rButton;
  Blynk.virtualWrite(V10, sdata);
}

String getValue(String data, char separator, int index)
{
    int found = 0;
    int strIndex[] = { 0, -1 };
    int maxIndex = data.length() - 1;

    for (int i = 0; i <= maxIndex && found <= index; i++) {
        if (data.charAt(i) == separator || i == maxIndex) {
            found++;
            strIndex[0] = strIndex[1] + 1;
            strIndex[1] = (i == maxIndex) ? i+1 : i;
        }
    }
    return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}

BLYNK_WRITE(V10){
  dataSwitch = param.asInt();
  Serial.print(dataSwitch);
}
