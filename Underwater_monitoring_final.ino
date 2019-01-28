#include <OneWire.h>
#include <LiquidCrystal.h>

LiquidCrystal lcd(12,11,5,4,3,2);
OneWire  ds(7);             // on pin 7 (a 4.7K resistor is necessary)

// defines pins numbers
const int led=A1;
const int led1=A2;
const int led2=A3;
const int trigPin = 10;
const int echoPin = 9;
const int buzzer = 13;       //buzzer to arduino pin 13

// defines variables
long duration;
int distance;

//For wifi module
#include <SoftwareSerial.h>

#define RX 6
#define TX 8

String AP = "dhruv";        // CHANGE ME
String PASS = "dhruv032";   // CHANGE ME

String API = "9IY1WUIGQ4UMPRED";   // CHANGE ME
String HOST = "api.thingspeak.com";
String PORT = "80";
//String field = "field1";

int countTrueCommand;
int countTimeCommand; 
boolean found = false; 
int valSensor = 1;

SoftwareSerial esp8266(RX,TX);

void setup() 
{
  pinMode(led,OUTPUT);        //set the led pin as output
  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);
  pinMode(buzzer, OUTPUT);    //Set buzzer - pin 13 as an output
  pinMode(trigPin, OUTPUT);   //Sets the trigPin as an Output
  pinMode(echoPin, INPUT);    //Sets the echoPin as an Input
  lcd.begin(16, 2);
  Serial.begin(9600);         // Starts the serial communication
  esp8266.begin(115200);
  sendCommand("AT",5,"OK");
  sendCommand("AT+CWMODE=1",5,"OK");
  sendCommand("AT+CWJAP=\""+ AP +"\",\""+ PASS +"\"",20,"OK");
}
void loop() 
{
  byte i;
  byte present = 0;
  byte type_s;
  byte data[12];
  byte addr[8];
  float celsius, fahrenheit;
  if ( !ds.search(addr))
 {
    ds.reset_search();
 } 
  ds.reset();
  ds.select(addr);
  ds.write(0x44, 1);        // start conversion, with parasite power on at the end
  
  delay(700);   
  present = ds.reset();
  ds.select(addr);    
  ds.write(0xBE);         // Read Scratchpad

  for ( i = 0; i < 9; i++)
  {          
    data[i] = ds.read(); // we need 9 bytes

  }
  int16_t raw = (data[1] << 8) | data[0];
  if (type_s)
  {
    raw = raw << 3; // 9 bit resolution default
    if (data[7] == 0x10)
    {
      // "count remain" gives full 12 bit resolution
      raw = (raw & 0xFFF0) + 12 - data[6];
    }
  }
  else
  {
    byte cfg = (data[4] & 0x60);// at lower res, the low bits are undefined, so let's zero them
    if (cfg == 0x00) raw = raw & ~7;  // 9 bit resolution, 93.75 ms
    else if (cfg == 0x20) raw = raw & ~3; // 10 bit res, 187.5 ms
    else if (cfg == 0x40) raw = raw & ~1; // 11 bit res, 375 ms  
    // default is 12 bit resolution, 750 ms conversion time
  }
  
  //Clears the trigPin
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  
  //Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  //Reads the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(echoPin, HIGH);
  
  //Calculating the distance
  distance= duration*0.034/2;
  
  //Prints the distance on the Serial Monitor
  Serial.print("Distance: ");
  Serial.println(distance);

  // read the input on analog pin 0
  int sensorValue = analogRead(A0);
  
if(distance>=0 && distance<=5)
{
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("WATER LEVEL:HIGH");
  Serial.print("WATER LEVEL: H");
  lcd.setCursor(0,1);
  lcd.print("DISTANCE: ");
  lcd.print(distance);
  digitalWrite(led,HIGH);       //turning led on
  digitalWrite(led1,LOW);       //turning led off
  digitalWrite(led2,LOW);       //turning led off
  digitalWrite(buzzer,HIGH);    // turning buzzer on
  delay(2000);
  
  celsius = (float)raw / 16.0;
  fahrenheit = celsius * 1.8 + 32.0;
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("T= ");
  lcd.print(celsius);
  lcd.print(" Celsius,");
  lcd.setCursor(0,1);
  lcd.print(fahrenheit);
  lcd.println(" Fahrenheit ");
  delay(1000);
  
  // print out the value you read:
  Serial.print(sensorValue);
  Serial.println(" NTU");
  delay(1000);        // delay in between reads for stability
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Turbidity of H2O");
  lcd.setCursor(0,1);
  lcd.print(sensorValue);
  lcd.print(" NTU");
  delay(1000);
}
else if(distance>5 && distance<=10)
{
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("WATER LEVEL: MED");
  Serial.print("WATER LEVEL: M");
  lcd.setCursor(0,1);
  lcd.print("DISTANCE: ");
  lcd.print(distance);
  digitalWrite(led1,HIGH);       //turning led on
  digitalWrite(led,LOW);        //turning led off
  digitalWrite(led2,LOW);       //turning led off
  digitalWrite(buzzer,LOW);    // turning buzzer off
  delay(2000);
  
  celsius = (float)raw / 16.0;
  fahrenheit = celsius * 1.8 + 32.0;
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("T= ");
  lcd.print(celsius);
  lcd.print(" Celsius,");
  lcd.setCursor(0,1);
  lcd.print(fahrenheit);
  lcd.println(" Fahrenheit ");
  delay(1000);
  
  // print out the value you read:
  Serial.print(sensorValue);
  Serial.println(" NTU");
  delay(1000);        // delay in between reads for stability
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Turbidity of H2O");
  lcd.setCursor(0,1);
  lcd.print(sensorValue);
  lcd.print(" NTU");
  delay(1000);
}
else if(distance>10 && distance<=15)
{
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("WATER LEVEL: LOW");
  Serial.print("WATER LEVEL: L");
  lcd.setCursor(0,1);
  lcd.print("DISTANCE: ");
  lcd.print(distance);
  digitalWrite(led2,HIGH);       //turning led on
  digitalWrite(led,LOW);        //turning led off
  digitalWrite(led1,LOW);       //turning led off
  digitalWrite(buzzer,HIGH);    // turning buzzer on
  delay(2000);
  
  celsius = (float)raw / 16.0;
  fahrenheit = celsius * 1.8 + 32.0;
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("T= ");
  lcd.print(celsius);
  lcd.print(" Celsius,");
  lcd.setCursor(0,1);
  lcd.print(fahrenheit);
  lcd.println(" Fahrenheit ");
  delay(1000);
  
  // print out the value you read:
  Serial.print(sensorValue);
  Serial.println(" NTU");
  delay(1000);        // delay in between reads for stability
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Turbidity of H2O");
  lcd.setCursor(0,1);
  lcd.print(sensorValue);
  lcd.print(" NTU");
  delay(1000);
}
  String getData = "GET /update?api_key="+ API +"&field1" +"="+String(celsius)+"&field2" +"="+String(distance)+"&field3" +"="+String(sensorValue);
  sendCommand("AT+CIPMUX=1",5,"OK");
  sendCommand("AT+CIPSTART=0,\"TCP\",\""+ HOST +"\","+ PORT,15,"OK");
  sendCommand("AT+CIPSEND=0," +String(getData.length()+4),4,">");
  esp8266.println(getData);delay(1500);countTrueCommand++;
  sendCommand("AT+CIPCLOSE=0",5,"OK");
}
int getSensorData()
{
  return analogRead(A0); // Replace with 
}

void sendCommand(String command, int maxTime, char readReplay[]) 
{
  Serial.print(countTrueCommand);
  Serial.print(". at command => ");
  Serial.print(command);
  Serial.print(" ");
  while(countTimeCommand < (maxTime*1))
  {
    esp8266.println(command);//at+cipsend
    if(esp8266.find(readReplay))//ok
    {
      found = true;
      break;
    }
  
    countTimeCommand++;
  }
  
  if(found == true)
  {
    Serial.println("OYI");
    countTrueCommand++;
    countTimeCommand = 0;
  }
  
  if(found == false)
  {
    Serial.println("Fail");
    countTrueCommand = 0;
    countTimeCommand = 0;
  }
  
  found = false;
}
