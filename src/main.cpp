#define BLYNK_TEMPLATE_ID "TMPL6rbpNEx98"
#define BLYNK_TEMPLATE_NAME "MyBlynk"
#define BLYNK_AUTH_TOKEN "55vW5J3jEBPpGpfOebRqMbz2BBdb6d7V"

#include <Arduino.h>
#include <WiFi.h>
#include <Wire.h>
#include <Preferences.h>
#include <LiquidCrystal_I2C.h>
#include <BlynkSimpleEsp32.h>
#include "HX711.h"

const int trigPin = 25;
const int echoPin = 26;
const int trigPin2 = 17;
const int echoPin2 = 16;
const int LOADCELL_DOUT_PIN = 18;
const int LOADCELL_SCK_PIN = 19;

#define VPIN_tb V0
#define VPIN_bb V1
#define VPIN_kg V2

char ssid[] = "Redmi Note 10 Pro";
char pass[] = "1sampai8";

HX711 scale;  
LiquidCrystal_I2C lcd(0x27, 16, 2);

BlynkTimer timer;
char auth[] = BLYNK_AUTH_TOKEN;
// put function declarations here:
long duration, distance;
long duration2, distance2;
float tera = 0;
int berat,x,jarak,tinggi,statusnya;
float fix;
int sp = 30;
float kg;
float weight; 
float calibration_factor = 211000; // for me this vlaue works just perfect 211000  
float BMI;

void connect (){
  while (WiFi.status() != WL_CONNECTED) {
    for (int i = 0; i < 4; i++) {
      lcd.setCursor(11 + i, 0); // posisi titik mulai dari kolom ke-11
      lcd.print(".");
      delay(650); // delay untuk efek animasi
    }
    
    lcd.setCursor(11, 0);
    lcd.print("    "); // hapus semua titik

    delay(250); // delay sebelum memulai ulang animasi
  }
}

void checkBlynkStatus()
{ // called every 2 seconds by SimpleTimer

  bool isconnected = Blynk.connected();
  if (isconnected == false)
  {
    Serial.println("Blynk Not Connected");
  }
  if (isconnected == true)
  {
    Serial.println(" Blynk IoT Connected ");
    // Blynk.virtualWrite(VPIN_Judul, "Monitoring Air Quality and Smoke Emissions at Urban Bus Stops");
  }

}

void measureweight(){
 scale.set_scale(calibration_factor); //Adjust to this calibration factor
  // Serial.print("Reading: ");
  weight = scale.get_units(5); 
    if(weight<0)
  {
    weight=0.00;
    }
  //Serial.print(scale.get_units(), 2);
 // Serial.print(" lbs"); //Change this to kg and re-adjust the calibration factor if you follow SI units like a sane person
  // Serial.print("Kilogram:");
  // Serial.print( weight); 
  // Serial.print(" Kg");
  // Serial.print(" calibration_factor: ");
  // Serial.print(calibration_factor);
  // Serial.println();
  // Delay before repeating measurement
  delay(100);
}


void usonic(){
  digitalWrite(trigPin, LOW);  // Added this line
  delayMicroseconds(2); // Added this line
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10); // Added this line
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  distance = (duration/2) / 29.1;


  // Serial.print(distance);
  // Serial.println(" cm");
  

  digitalWrite(trigPin2, LOW);  // Added this line
  delayMicroseconds(2); // Added this line
  digitalWrite(trigPin2, HIGH);
  delayMicroseconds(10); // Added this line
  digitalWrite(trigPin2, LOW);
  duration2 = pulseIn(echoPin2, HIGH);
  distance2 = (duration2/2) / 29.1;

  // Serial.print(distance2);
  // Serial.println(" cm");
} 
void result(){
  usonic();
  measureweight();
  tinggi = 55 - distance - distance2;

  // Pastikan tinggi dalam meter untuk perhitungan BMI
  float tinggi_m = tinggi / 100.0;
  BMI = weight / (tinggi_m * tinggi_m);
  
  lcd.setCursor(0,0);
  lcd.print("W/H:");
  lcd.print(weight);
  Blynk.virtualWrite(VPIN_bb,weight);
  lcd.print("/");
  lcd.print(tinggi);
  Blynk.virtualWrite(VPIN_tb,tinggi);
  lcd.print(" cm");

  lcd.setCursor(11,1);
  lcd.print(BMI); 

  if (BMI < 17){
    statusnya = 1;
    lcd.setCursor(0,1);
    lcd.print(" KURUS   ");
    Blynk.virtualWrite(VPIN_kg," KURUS ");
  } else if (BMI >= 17 && BMI < 23){
    statusnya = 2;
    lcd.setCursor(0,1);
    lcd.print(" NORMAL  ");  
    Blynk.virtualWrite(VPIN_kg," NORMAL ");
  } else if (BMI >= 23 && BMI < 30){
    statusnya = 3;
    lcd.setCursor(0,1);
    lcd.print(" GEMUK   ");  
    Blynk.virtualWrite(VPIN_kg," GEMUK ");
  } else if (BMI >= 30){
    statusnya = 4;
    lcd.setCursor(0,1);
    lcd.print(" OBES   ");  
    Blynk.virtualWrite(VPIN_kg," OBESITAS ");
  }

}


void setup() {
  Serial.begin(9600);
  
  lcd.begin();
  lcd.clear();
  lcd.noCursor();
  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(trigPin2, OUTPUT);
  pinMode(echoPin2, INPUT);

  WiFi.begin(ssid, pass);
  lcd.setCursor(0,0);
  lcd.print("Loading ...");
  delay(1000);
  lcd.clear();
  scale.set_scale();
  scale.tare();
  long zero_factor = scale. read_average();

  Blynk.begin(auth, ssid, pass, "blynk.cloud",80);
}

void loop() {
  Blynk.run();
  timer.run();
  if(!WiFi.isConnected()){
    connect();
  } 
  result();
}
