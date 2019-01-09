/smart drug dispenser
//created by Archana,ankith and Anisha

#include<Stepper.h>
#include <SPI.h>;
#include <Ethernet.h>;
#include <Wire.h>
#include "RTClib.h"
#include "pitches.h"
#include <LiquidCrystal.h>
#define led 13
int ByteReceived;
int j = 0;
String pill[5] = {"", "", "", "", ""};
int nopill[5];
int h[5], m[5];
int no, i, k;
int inPin = 7;   // choose the input pin (for a pushbutton)
int val = 0;// variable for reading the pin status
int flag = 0;
int steps = 200;
int mot = 8;
Stepper motor(steps, 10, 11, 12, 13);
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);
RTC_DS1307 rtc;

#if defined(ARDUINO_ARCH_SAMD)
// for Zero, output on USB Serial console, remove line below if using programming port to program the Zero!
#define Serial SerialUSB
#endif

// Arduino network information
byte mac[] = {  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xEE  };
EthernetClient client;
char smtpServer[] = "smtpcorp.com";



void setup()
{
  Serial.begin(9600);
  while (!Serial)
  {
    ;
  }
  in();
  Wire.begin();
  rtc.begin();

  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }

  if (! rtc.isrunning())
  {
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }
  setupComm();
  pinMode(mot, OUTPUT);


  //pinMode(led, OUTPUT);
}

void check()          //function to check if tablet is taken
{
  val = digitalRead(inPin);  // read input value
  if (val == HIGH)
  {
    flag = 1;
    digitalWrite(led, LOW);
    Serial.println(" thank you ");
  }
  else
    return;

}

void play()
{
  int melody[] =
  { NOTE_C4, NOTE_G3, NOTE_G3, NOTE_A3, NOTE_G3, 0, NOTE_B3, NOTE_C4};
  int noteDurations[] = {4, 8, 8, 4, 4, 4, 4, 4};
  for (int thisNote = 0; thisNote < 8; thisNote++)
  {
    int noteDuration = 1000 / noteDurations[thisNote];
    tone(9, melody[thisNote], noteDuration);
    int pauseBetweenNotes = noteDuration * 1.30;
    delay(pauseBetweenNotes);
    noTone(9);
  }
}


//  ethernet shield connection init
void setupComm()
{
  Serial.println("Trying to connect");
  if (!Ethernet.begin(mac)) {
    Serial.println("Failed to DHCP");
    // verifyng connection
    while (true);
  }
  delay(10000);
  // IP address is:
  Serial.print("IP address: ");
  for (byte thisByte = 0; thisByte < 4; thisByte++) {
    Serial.print(Ethernet.localIP()[thisByte], DEC);
    Serial.print(".");
  }
  Serial.println();
}
// now I send email
bool email(char* text)
{
  bool success = false;
  Serial.println("Sending email...");
  if (client.connect(smtpServer, 2525)) {           //2525 is SMTP Server port
    Serial.println("connected");
    delay(100);
    client.println("EHLO arduino");
    for (int i = 0; i < 999; ++i) {
      if (client.read() > 0)
        break;
    }
    Serial.println("responded");
    client.println("AUTH LOGIN");                     //see "http://base64-encoder-online.waraxe.us/"
    client.println("bWVkZGlzcDcyNw==");                    //Type your username  and encode it
    client.println("bWVkaWNpbmVkaXNwZW5zZXI=");                   //Type your password  and encode it</p>
    // Put your "from" email address here
    client.println("MAIL FROM:<ankith.guzz@gmail.com>"); //Seems you can write what you want here...
    for (int i = 0; i < 999; ++i) {
      if (client.read() > 0)
        break;
    }
    client.println("RCPT TO:<ankith.uzz@gmail.com>");
    for (int i = 0; i < 999; ++i) {
      if (client.read() > 0)
        break;
    }
    client.println("DATA");
    for (int i = 0; i < 999; ++i) {
      if (client.read() > 0)
        break;
    }
    //Sender
    client.println("from: ankith.guzz@gmail.com"); //Sender address
    client.println("to: ankith.uzz@gmail.com");  //Receiver address
    client.println("SUBJECT: From your arduino");
    client.println("");
    client.println(text);
    client.println(".");
    client.println("QUIT");
    for (int i = 0; i < 999; ++i) {
      if (i > 998) {
        Serial.println("error: No response");
      }
      if (client.read() > 0)
        break;
    }
    success = true;
    client.println();
    Serial.println("end");
  }
  else {
    Serial.println("Failed");
    client.println("QUIT"); //Disconnection
  }
  client.stop();
  return success;
}




char in()           //to initially take the inputs
{

  Serial.println("welcome to the medicine dispenser");
  Serial.println("please enter number of types of pills(range between 1-5)");
  while (Serial.available() == 0) {   }
  no = Serial.parseInt();
  Serial.print("you have to enter the details of ");
  Serial.print(no);
  Serial.println(" tablets");
  for (k = 1; k <= no; k++)
  {
    Serial.print("enter name of the pill in slot ");
    Serial.println(k);
    while (Serial.available() == 0) {   }
    pill[k] = Serial.readString();
    Serial.println("enter no of pills");
    while (Serial.available() == 0) {   }
    nopill[k] = Serial.parseInt();
    Serial.println("enter time to take table");
    Serial.println("enter the hour");
    while (Serial.available() == 0) {   }
    h[k] = Serial.parseInt();
    //h[k]=now.hour();
    Serial.println("enter the minutes");
    while (Serial.available() == 0) {   }
    m[k] = Serial.parseInt();
    Serial.println("slot no.: \t");
    Serial.print(k);
    Serial.println("name: \t");
    Serial.print(pill[k]);
    Serial.print("no.of tablets: \t");
    Serial.println(nopill[k]);
    Serial.print("time: \t");
    Serial.print(h[k]);
    Serial.print(m[k]);
  }
}




void re(int z)          //to decrement tablets
{
  if (flag == 1)
  {
    z--;
    if (z < 5)
    {
      Serial.print("please refil your tablet ");
    }
    return;
  }
}

void bro()
{
  do
  {
    digitalWrite(led, HIGH);
    play();
    delay(3000);
    Serial.println("hello,please take your tablet");
    lcd.print("Hello! please take your tablet ");
    for (int positionCounter = 0; positionCounter < 35; positionCounter++)
    {
      // scroll one position left:
      lcd.scrollDisplayLeft();
      // wait a bit:
      delay(150);
    }
    if (Serial.available() > 0)
    {
      check();
    }

    else if (! Serial.available())
    {

      digitalWrite(led, HIGH);
      for (int i = 1; i < 6; i++)
      {
        delay(10000);
        play();
        Serial.println();
        Serial.print("please take your tablet!!!! this is your ");
        Serial.print(i);
        Serial.print("th reminder");
        lcd.print("Hello! please take your tablet this is your next reminder");
        for (int positionCounter = 0; positionCounter < 67; positionCounter++)
        {
          // scroll one position left:
          lcd.scrollDisplayLeft();
          // wait a bit:
          delay(150);
        }
        j++;
        delay(10000);
        check();
      }
    }
  }
  while (j < 5);
  if (j == 5)
  {
    Serial.println("sending a message to the client");
    email("The elderly under your care has failed to take his dose of tablet after 5 reminders. PLEASE DO CHECK IF HE/SHE IS ALRIGHT");
  }
  return;
}



void loop()
{
  int speed;
  int rpm;

  DateTime now = rtc.now();

  Serial.println(now.hour(), DEC);
  Serial.println(now.minute(), DEC);

  if (h[1] == now.hour() && m[1] == now.minute())
  {
    //rotate stepper motor
    speed = analogRead(A0);
    rpm = map(speed, 0, 1023, 0, 100);
    digitalWrite(mot, LOW);
    motor.step(1);
    motor.setSpeed(rpm);
    delay(100);
    bro();
    re(nopill[1]);

  }
  else if (h[2] == now.hour() && m[2] == now.minute())
  {
    //rotate stepper motor
    speed = analogRead(A0);
    rpm = map(speed, 0, 1023, 0, 100);
    digitalWrite(mot, LOW);
    motor.step(1);
    motor.setSpeed(rpm);
    delay(100);
    bro();
    re(nopill[2]);

  }
  else if (h[3] == now.hour() && m[3] == now.minute())
  {
    //rotate stepper motor
    speed = analogRead(A0);
    rpm = map(speed, 0, 1023, 0, 100);
    digitalWrite(mot, LOW);
    motor.step(1);
    motor.setSpeed(rpm);
    delay(100);
    bro();
    re(nopill[3]);

  }
  else if (h[4] == now.hour() && m[4] == now.minute())
  {
    //rotate stepper motor
    speed = analogRead(A0);
    rpm = map(speed, 0, 1023, 0, 100);
    digitalWrite(mot, LOW);
    motor.step(1);
    motor.setSpeed(rpm);
    delay(100);
    bro();
    re(nopill[4]);

  }
  else if (h[5] == now.hour() && m[5] == now.minute())
  {
    //rotate stepper motor
    speed = analogRead(A0);
    rpm = map(speed, 0, 1023, 0, 100);
    digitalWrite(mot, LOW);
    motor.step(1);
    motor.setSpeed(rpm);
    delay(100);
    bro();
    re(nopill[5]);
  }
  else
  {
    Serial.println("its not yet the time for your medicine. Please do come back later");
  }

}
