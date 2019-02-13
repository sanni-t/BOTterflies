// Arduino Code to control 7 butterflies
// Communicates with an image processing MAX patch through Serial

#define SETA_CW  3
#define SETA_CC  5
#define SETB_CW  9
#define SETB_CC  6
#define SETC_CW  10
#define SETC_CC  13

#define FLY_NUM  7

#define LOW_INTENSITY  60
#define MED_INTENSITY  70
#define HI_INTENSITY   90
#define TEST_CASE     100

byte fly_en[] = {8, 4, 7, 12, 11, 2};
byte motionIntensityLevel = 0;
byte activeFlyNumber = 0;
byte fliesStatus = 0x00;
byte serialInput;
int loopCounter;

void setup() {
  // put your setup code here, to run once:
  for(int i = 0; i < FLY_NUM-1 ; i++)  //7 flies but two have common enable pin
  {
    pinMode(fly_en [i], OUTPUT);
    digitalWrite(fly_en[i], LOW);
  }
  analogWrite(SETA_CW, 150);
  analogWrite(SETA_CW, 0);
  analogWrite(SETA_CC, 0);
  analogWrite(SETB_CW, 0);
  analogWrite(SETB_CC, 0);
  analogWrite(SETC_CW, 0);
  analogWrite(SETC_CC, 0);

  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  if(Serial.available())
  {
    serialInput = Serial.read();

    switch(serialInput)
    {
      case LOW_INTENSITY:
        motionIntensityLevel = 1;
        break;
      case MED_INTENSITY:
        motionIntensityLevel = 2;
        break;
      case HI_INTENSITY:
        motionIntensityLevel = 3;
        break;
      case TEST_CASE:
        digitalWrite(fly_en[5], HIGH);
        runSetA();
        runSetB();
        runSetC();
        digitalWrite(fly_en[5], LOW);
        break;
      default:
        motionIntensityLevel = 0;
    }


	  if(motionIntensityLevel != 0)
	    flutterTheFlies();
	  //Flush out the serial buffer
	  while(Serial.available() > 0)
	  {
	      Serial.read();
	  }
  }
}

void flutterTheFlies()
{
  switch(motionIntensityLevel)
  {
    case 1: //Turn ON only 2 flies
      for(int i = 0; i < 2; i++)
      {
        byte temp;
        do
        {
          activeFlyNumber = (byte)random(1,6);
          temp = fliesStatus | (1 << activeFlyNumber);
        }
        while(temp == fliesStatus );  //Try again if same number is picked

        fliesStatus |= (1 << activeFlyNumber);
        //Temporary workaround for having two flies connected to same enable pin (micro pin 12)
        if(activeFlyNumber == 3)
        {
           Serial.write(activeFlyNumber);  //Send fly number to processing via MAX
           Serial.write(5);
        }
        else if(activeFlyNumber == 5)
        {
          Serial.write(6);
        }
        else
        {
          Serial.write(activeFlyNumber);
        }
        digitalWrite(fly_en[activeFlyNumber], HIGH);
      }
      executePWM();
      for(int i = 0; i < FLY_NUM-1; i++)
      {
        digitalWrite(fly_en[i], LOW);
      }
      fliesStatus = 0x00;
      Serial.write(fliesStatus);
      break;

    case 2:
      for(int i = 0; i < 4; i++)
      {
        //If same number as previous one is picked
        byte temp;
        do
        {
          activeFlyNumber = (byte)random(1,6);
          temp = fliesStatus | (1 << activeFlyNumber);
        }
        while(temp == fliesStatus );  //Try again if same number is picked

        fliesStatus |= (1 << activeFlyNumber);
        //Temporary workaround for having two flies connected to same enable pin (micro pin 12)
        if(activeFlyNumber == 3)
        {
           Serial.write(activeFlyNumber);  //Send fly number to processing via MAX
           Serial.write(5);
        }
        else if(activeFlyNumber == 5)
        {
          Serial.write(6);
        }
        else
        {
          Serial.write(activeFlyNumber);
        }
        digitalWrite(fly_en[activeFlyNumber], HIGH);
      }
      executePWM();
      for(int i = 0; i < FLY_NUM-1; i++)
      {
        digitalWrite(fly_en[i], LOW);
      }
      fliesStatus = 0x00;
      Serial.write(fliesStatus);
      break;

    case 3: //Turn on all flies
      for(int i = 0; i < FLY_NUM-1; i++)
      {
        activeFlyNumber = (byte)i;
        fliesStatus |= (1 << activeFlyNumber);
        //Temporary workaround for having two flies connected to same enable pin (micro pin 12)
        if(activeFlyNumber == 3)
        {
           Serial.write(activeFlyNumber);  //Send fly number to processing via MAX
           Serial.write(5);
        }
        else if(activeFlyNumber == 5)
        {
          Serial.write(6);
        }
        else
        {
          Serial.write(activeFlyNumber);
        }
        digitalWrite(fly_en[activeFlyNumber], HIGH);
      }
      executePWM();
      for(int i = 0; i < FLY_NUM-1; i++)
      {
        digitalWrite(fly_en[i], LOW);
      }
      fliesStatus = 0x00;
      Serial.write(fliesStatus);
      break;
  }
}

/**** Pulse widths for PWM are determined by trial & error ****/
void runSetA()
{
  uint16_t pulseWidth = 20;
  // Serial.println("Executing PWM");
  //Set A
  analogWrite(SETA_CW, 0);
  //Ramp up
  for(int i = 1; i <= 8; i++)
  {
    pulseWidth += 21/i;
    analogWrite(SETA_CC, pulseWidth);
    delay(10);
  }
  //Ramp down
  for(int i = 19; i > 8; i--)
  {
    pulseWidth -=21/i;
    analogWrite(SETA_CC, pulseWidth);
    delay(10);
  }

  pulseWidth = 20;
  analogWrite(SETA_CC, 0);
  //Ramp up
  for(int i = 1; i <= 8; i++)
  {
    pulseWidth += 22/i;
    analogWrite(SETA_CW, pulseWidth);
    delay(10);
  }
  //Ramp down
  for(int i = 14; i > 7; i--)
  {
    pulseWidth -= 21/i;
    analogWrite(SETA_CW, pulseWidth);
    delay(10);
  }
  analogWrite(SETA_CW, 0);
}

void runSetB()
{
  uint16_t pulseWidth = 20;
  analogWrite(SETB_CW, 0);
  //Ramp up
  for(int i = 1; i <= 8; i++)
  {
    pulseWidth += 21/i;
    analogWrite(SETB_CC, pulseWidth);
    delay(10);
  }
  //Ramp down
  for(int i = 19; i > 8; i--)
  {
    pulseWidth -=21/i;
    analogWrite(SETB_CC, pulseWidth);
    delay(10);
  }

  pulseWidth = 20;
  analogWrite(SETB_CC, 0);
  //Ramp up
  for(int i = 1; i <= 8; i++)
  {
    pulseWidth += 22/i;
    analogWrite(SETB_CW, pulseWidth);
    delay(10);
  }
  //Ramp down
  for(int i = 19; i > 8; i--)
  {
    pulseWidth -=21/i;
    analogWrite(SETB_CW, pulseWidth);
    delay(10);
  }
  analogWrite(SETB_CW, 0);
}

void runSetC()
{
  uint16_t pulseWidth = 20;
  analogWrite(SETC_CW, 0);

  //Ramp up
  for(int i = 1; i <= 8; i++)
  {
    pulseWidth += 21/i;
    analogWrite(SETC_CC, pulseWidth);
    delay(10);
  }
  //Ramp down
  for(int i = 19; i > 8; i--)
  {
    pulseWidth -=21/i;
    analogWrite(SETC_CC, pulseWidth);
    delay(10);
  }

  pulseWidth = 20;
  analogWrite(SETC_CC, 0);
  //Ramp up
  for(int i = 1; i <= 8; i++)
  {
    pulseWidth += 22/i;
    analogWrite(SETC_CW, pulseWidth);
    delay(10);
  }
  //Ramp down
  for(int i = 19; i > 8; i--)
  {
    pulseWidth -=21/i;
    analogWrite(SETC_CW, pulseWidth);
    delay(10);
  }
  analogWrite(SETC_CW, 0);
}

void executePWM()
{   //Execute different PWM patterns acc. to motion intensity
    switch(motionIntensityLevel)
    {
      case 1:
        runSetA();
        runSetA();
        delay(500);
        runSetB();
        runSetC();
        runSetC();
        break;
      case 2:
        runSetC();
        runSetC();
        delay(680);
        runSetA();
        runSetA();
        delay(500);
        runSetB();
        runSetB();
        runSetB();
        break;
      case 3:
        runSetB();
        runSetB();
        delay(680);
        runSetC();
        runSetC();
        runSetA();
        runSetA();
        runSetA();
        delay(500);
        runSetB();
        runSetB();
        runSetB();
        runSetB();
    }
}
