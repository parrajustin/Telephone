/*
  DigitalReadSerial

  Reads a digital input on pin 2, prints the result to the Serial Monitor

  This example code is in the public domain.

  http://www.arduino.cc/en/Tutorial/DigitalReadSerial
*/
#include <SoftwareSerial.h>
#include <stdlib.h>
SoftwareSerial GPRS(7, 8);

// digital pin 2 has a pushbutton attached to it. Give it a name:
int inputPin = 2;
int activePin = 3;
int resetPin = 4;

int reset = 1;
int counter = 0;
int isHigh = 0;

char *numberBuffer;
int numberIndex = 0;
int inCall = 0;

unsigned char buffer[64]; // buffer array for data recieve over serial port
int count = 0;            // counter for buffer array

// the setup routine runs once when you press reset:
void setup()
{
  numberBuffer = malloc(sizeof(char) * (10));
  GPRS.begin(19200);   // the GPRS baud rate
  Serial.begin(19200); // the Serial port of Arduino baud rate.
  // make the pushbutton's pin an input:
  pinMode(inputPin, INPUT);
  Serial.println("I'M ALIVE");
}

// the loop routine runs over and over again forever:
void loop()
{
  //Read input pins to get state
  int isActive = digitalRead(activePin);
  int isReset = digitalRead(resetPin);

  // resets/hangs up
  if (isReset == 1)
  {
    Serial.println("RESET");
    numberIndex = 0;
    inCall = 0;
    GPRS.write("ATH;\r\n");
  }

  // 10 digits stored, make call
  if (numberIndex == 10 && inCall == 0)
  {
    Serial.println("CALLING");
    inCall = 1;
    GPRS.write("ATD+1"); // write it to the GPRS shield

    for (int i = 0; i < 10; i++)
    {
      GPRS.write(numberBuffer[i]);
    }

    GPRS.write(";\r\n");
  }

  // Serial.print(isActive);
  // Serial.print(" ");
  // Serial.print(inCall);

  // store dial digits
  int pulse = digitalRead(inputPin);
  if (isActive == 1 && inCall == 0)
  {
    //    Serial.println("Is Active");
    if (reset == 1)
    {
      Serial.println("RESET");
      counter = 0;
      reset = 0;
      isHigh = 0;
    }

    if (pulse == 1 && isHigh == 0)
    {
      counter++;
      isHigh = 1;
      Serial.println("COUNTER++");
    }
    else if (pulse == 0 && isHigh == 1)
    {
      isHigh = 0;
      Serial.println("RESET HIGH");
    }
  }
  else if (isActive == 0 && reset == 0)
  {
    reset = 1;
    isHigh = 0;
    counter %= 10;
    Serial.println(counter);

    if (numberIndex < 10)
    {
      numberBuffer[numberIndex] = (char)(48 + counter);
      numberIndex++;
    }
  }

  //Debug printout statements
  Serial.print(isActive);
  Serial.print(" ");
  Serial.print(pulse);
  Serial.print(" ");
  Serial.print(counter);
  Serial.print(" ");
  Serial.println(inCall);

  if (GPRS.available())
  {
    while (GPRS.available()) // reading data into char array
    {
      buffer[count++] = GPRS.read(); // writing data into array
      if (count == 64)
      {
        break;
      }
    }
    Serial.write(buffer, count); // if no data transmission ends, write buffer to hardware serial port
    clearBufferArray();          // call clearBufferArray function to clear the storaged data from the array
    count = 0;                   // set counter of while loop to zero
  }
  if (Serial.available())
  {                            // if data is available on hardwareserial port ==> data is comming from PC or notebook
    GPRS.write(Serial.read()); // write it to the GPRS shield
  }
  delay(10);
}

void clearBufferArray()
{
  for (int i = 0; i < count; i++)
  {
    buffer[i] = NULL;
  } // clear all index of array with command NULL
}
