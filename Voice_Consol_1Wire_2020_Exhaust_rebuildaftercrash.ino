/* Darn fool thing doesn't want to update the thermal readings unless
 * one of the switches is changing state. Not sure how to fix that.
 * Based on Neds code and the one i worked on for ages with a little help from
 * Roman-from oldschool and maybe, probably someone else too.
 * And one guy said to replace ....
 * "int thermoSensorNewVal, thermoSensorOldVal;" with  "volatile thermoSensorNewVal, thermoSensorOldVal;" 
 * Goodluck.
 */
#include <max6675.h>
#include <SoftwareSerial.h>
#include <OneWire.h> 
#include <DallasTemperature.h>

// Data wire is plugged into pin 2 on the Arduino 
#define ONE_WIRE_BUS A2 
// Setup a oneWire instance to communicate with any OneWire devices  
// (not just Maxim/Dallas temperature ICs) 
OneWire oneWire(ONE_WIRE_BUS); 
// Pass our oneWire reference to Dallas Temperature. 
DallasTemperature sensors(&oneWire);

SoftwareSerial mySerial(10, 11);

int soPin = 12;// SO=Serial Out
int csPin = 13;// CS = chip select CS pin
int sckPin = A1;// SCK = Serial Clock pin

MAX6675 thermoSensor(sckPin, csPin, soPin);

/* This is where i build my own thermal state gizmo & set the voltile thingys */
int volatile thermoSensorNewVal, thermoSensorOldVal;  // an attempt to add zoomcats potentometer test to thermosensor
volatile int thermalTriggerPlayState = true;

volatile int heatSensorFailure = false;
volatile int heatNominal = true;
volatile int heatMinor = false;
volatile int heatGeneral = false;
volatile int heatMajor = false;
volatile int heatMeltdown = false;

/* This is where i build my own thermal state gizmo, changed them to exhaust, & set the voltile thingys */
int volatile exTempSensorNewVal, exTempSensorOldVal;  // an attempt to add zoomcats potentometer test to thermosensor
volatile int exTempTriggerPlayState = true;

volatile int heatExSensorFailure = false;
volatile int heatExNominal = true;
volatile int heatExMinor = false;
volatile int heatExGeneral = false;
volatile int heatExMajor = false;
volatile int heatExMeltdown = false;

#define TEMP_BAND     2
#define TEMP_MINOR    75 //Legacy code from neds code, which i never was able to learn how to modify. Lifes harder when you're dumb.
#define TEMP_GENERAL  85 //Legacy code from neds code, which i never was able to learn how to modify. Lifes harder when you're dumb.
#define TEMP_DIRE     97 //Legacy code from neds code, which i never was able to learn how to modify. Lifes harder when you're dumb.


int buttonPause = 3;
int buttonVolPlus = 2;
int buttonVolMinus = 4;

# define ACTIVATED LOW

boolean isPlaying = false;

const int  buttonPin = 6;    // the pin that the pushbutton is attached to
const int  buttonPin2 = 7;    // the pin that the pushbutton is attached to
const int  buttonPin3 = 8;    // the pin that the pushbutton is attached to
const int  buttonPin4 = 9;    // the pin that the pushbutton is attached to
const int  buttonPin5 = 5;
// the pin that the pushbutton is attached to
// Variables will change:
int buttonPushCounter = 0;   // counter for the number of button presses
int buttonState = 0;         // current state of the button
int lastButtonState = 0;     // previous state of the button
int buttonPushCounter2 = 0;   // counter for the number of button presses
int buttonState2 = 0;         // current state of the button
int lastButtonState2 = 0;     // previous state of the button
int buttonPushCounter3 = 0;   // counter for the number of button presses
int buttonState3 = 0;         // current state of the button
int lastButtonState3 = 0;     // previous state of the button
int buttonPushCounter4 = 0;   // counter for the number of button presses
int buttonState4 = 0;         // current state of the button
int lastButtonState4 = 0;     // previous state of the button
int buttonPushCounter5 = 0;   // counter for the number of button presses
int buttonState5 = 0;         // current state of the button
int lastButtonState5 = 0;     // previous state of the button


void setup() {
  Serial.begin(9600);  
  Serial.println("Robojax MAX6675");
 Serial.println("Dallas Temperature IC Control Library Demo"); 
 // Start up the library 
 sensors.begin(); 
 
# define Start_Byte 0x7E
# define Version_Byte 0xFF
# define Command_Length 0x06
# define End_Byte 0xEF
# define Acknowledge 0x00 //Returns info with command 0x41 [0x01: info, 0x00: no info]

# define ACTIVATED LOW


pinMode(buttonPause, INPUT);
digitalWrite(buttonPause,HIGH);
// initialize the button pin as a input:
pinMode(buttonVolPlus, INPUT);
digitalWrite(buttonVolPlus,HIGH);
// initialize the button pin as a input:
pinMode(buttonVolMinus, INPUT);
digitalWrite(buttonVolMinus,HIGH);
// initialize the button pin as a input:
  
pinMode(buttonPin, INPUT);
// initialize the button pin as a input:
pinMode(buttonPin2, INPUT);
// initialize the button pin as a input:
pinMode(buttonPin3, INPUT);
// initialize the button pin as a input:
pinMode(buttonPin4, INPUT);
// initialize the button pin as a input:
pinMode(buttonPin5, INPUT);
// initialize the button pin as a input:

  mySerial.begin (9600);
  delay(1000);
  execute_CMD(0x06, 0, 0x12);   // Set the volume between 0-30 (0x00~0x30)//
  execute_CMD(0x03, 0, 0x0002);    //file to play at system start-up//
  isPlaying = true;



}


void loop()
{
  /*the 3 wire (one wire) temp sensor code, that then looks at all the stupid code i made for the state gizmo,*/
  sensors.requestTemperatures(); // Send the command to get temperature readings 
  thermoSensorNewVal = (sensors.getTempCByIndex(0));

  if (thermoSensorNewVal != thermoSensorOldVal)
  {
    tempConsultThermalTable();
    if (thermalTriggerPlayState == false)
      {
        chooseVoiceWarning();
      }
    thermoSensorOldVal = thermoSensorNewVal;
  }
  
/*now to do the same for the exhaust sensor.  i got tired of the max6675 thing glitching out over the spark plugs so used the one wire, now max6675's an exhaust probe*/
  exTempSensorNewVal = thermoSensor.readCelsius();
  if (exTempSensorNewVal != exTempSensorOldVal)
  {
    tempConsultExhaustTable();
    if (exTempTriggerPlayState == false)
      {
        chooseExhaustVoiceWarning();
      }
    exTempSensorOldVal = exTempSensorNewVal;
  }

  delay(500);
    Serial.print(" Engine-C = ");
    Serial.print(thermoSensorNewVal);
    Serial.print(" Exhaust-C = ");
    Serial.print(exTempSensorNewVal);
  // read the pushbutton input pin:
  buttonState = digitalRead(buttonPin);

  // compare the buttonState to its previous state
  if (buttonState != lastButtonState) {
    // if the state has changed, increment the counter
    if (buttonState == HIGH) {
      // if the current state is HIGH then the button went from off to on:
      buttonPushCounter++;
      playEngineEnabled();
    } else {
      // if the current state is LOW then the button went from on to off:
      playShuttingDown();
    }
    // Delay a little bit to avoid bouncing
    delay(20);
  }
  // save the current state as the last state, for next time through the loop
  lastButtonState = buttonState;

  // read the pushbutton input pin:
  buttonState2 = digitalRead(buttonPin2);

  // compare the buttonState to its previous state
  if (buttonState2 != lastButtonState2) {
    // if the state has changed, increment the counter
    if (buttonState2 == HIGH) {
      // if the current state is HIGH then the button went from off to on:
      buttonPushCounter2++;
      playLightAmpEngaged();
    } else {
      // if the current state is LOW then the button went from on to off:
      playButtonOff();
    }
    // Delay a little bit to avoid bouncing
    delay(20);
  }
  // save the current state as the last state, for next time through the loop
  lastButtonState2 = buttonState2;

  buttonState3 = digitalRead(buttonPin3);
  // compare the buttonState to its previous state
  if (buttonState3 != lastButtonState3) {
    // if the state has changed, increment the counter
    if (buttonState3 == HIGH) {
      // if the current state is HIGH then the button went from off to on:
      buttonPushCounter++;
      playLightAmp2Engaged();
    } else {
      // if the current state is LOW then the button went from on to off:
      playButtonOff();
    }
    // Delay a little bit to avoid bouncing
    delay(20);
  }
  // save the current state as the last state, for next time through the loop
  lastButtonState3 = buttonState3;

  // read the pushbutton input pin:
  buttonState4 = digitalRead(buttonPin4);

  // compare the buttonState to its previous state
  if (buttonState4 != lastButtonState4) {
    // if the state has changed, increment the counter
    if (buttonState4 == HIGH) {
      // if the current state is HIGH then the button went from off to on:
      buttonPushCounter4++;
      playEXcamEngaged();
    } else {
      // if the current state is LOW then the button went from on to off:
      playButtonOff();
    }
    // Delay a little bit to avoid bouncing
    delay(20);
  }
  // save the current state as the last state, for next time through the loop
  lastButtonState4 = buttonState4;

    // read the pushbutton input pin:
  buttonState5 = digitalRead(buttonPin5);

  // compare the buttonState to its previous state
  if (buttonState5 != lastButtonState5) {
    // if the state has changed, increment the counter
    if (buttonState5 == HIGH) {
      // if the current state is HIGH then the button went from off to on:
      buttonPushCounter5++;
      playPhoneChargePort();
    } else {
      // if the current state is LOW then the button went from on to off:
      playButtonOff();
    }
    // Delay a little bit to avoid bouncing
    delay(20);
  }
  // save the current state as the last state, for next time through the loop
  lastButtonState5 = buttonState5;

 if (digitalRead(buttonPause) == ACTIVATED)
  {
    if(isPlaying)
    {
      pause();
      isPlaying = false;
    }else
    {
      isPlaying = true;
      play();
    }
  }

 if (digitalRead(buttonVolPlus) == ACTIVATED)
  {
    if(isPlaying)
    {
      setVolumeUp();
    }
  }

  if (digitalRead(buttonVolMinus) == ACTIVATED)
  {
    if(isPlaying)
    {
      setVolumeDown();
    }
  }
} // end of loop?

void playThermalExceed()
{
  Serial.println(" Thermal Exceeded Triggered Once ");
  execute_CMD(0x03, 0, 0x000c);    //file between 0-2999 in main folder. Doesn't love folders.//
  delay(3000);
}

void playHeatLevelCrit()
{
  Serial.println(" Heat Critical Triggered Once ");
  execute_CMD(0x03, 0, 0x000a);    //file between 0-2999 in main folder. Doesn't love folders.//
  delay(2200);
}

void playMeltdownImminent()
{
  Serial.println(" Meltdown Triggered Once ");
  execute_CMD(0x03, 0, 0x000b);    //file between 0-2999 in main folder. Doesn't love folders.//
  delay(1600);
}

void playHeatModerate()
{
  Serial.println(" HeatModerate Once ");
  execute_CMD(0x03, 0, 0x000d);    //file between 0-2999 in main folder. Doesn't love folders.//
  delay(1500);
}

void playHeatNominal()
{
  Serial.println(" HeatNominal Triggered Once ");
  execute_CMD(0x03, 0, 0x000e);    //file between 0-2999 in main folder. Doesn't love folders.//
  delay(1500);
}

void pause()
{
  execute_CMD(0x0E,0,0);
  delay(500);
}

void play()
{
  execute_CMD(0x0D,0,1); 
  delay(500);
}

void playNext()
{
  execute_CMD(0x01,0,1);
  delay(50);
}

void playPrevious()
{
  execute_CMD(0x02,0,1);
  delay(50);
}

void setVolumeUp()
{
  execute_CMD(0x04, 0, 1); // Set the volume (0x00~0x30)
  delay(50);
}

void setVolumeDown()
{
  execute_CMD(0x05, 0, 1); // Set the volume (0x00~0x30)
  delay(50);
}

void setVolume(int volume) //Specify volume using code. Not applicable to the normal button interface.
{
  execute_CMD(0x06, 0, volume); // Set the volume (0x00~0x30)
  delay(2000);
}

void playEngineEnabled()
{
  execute_CMD(0x03, 0, 0x0006);    //file between 0-2999 in main folder. Doesn't love folders.//
  delay(2000);
}

void playLightAmpEngaged()
{
  execute_CMD(0x03, 0, 0x0004);    //file between 0-2999 in main folder. Doesn't love folders.//
  delay(2600);
}

void playLightAmp2Engaged()
{
  execute_CMD(0x03, 0, 0x0005);    //file between 0-2999 in main folder. Doesn't love folders.//
  delay(2800);
}

void playEXcamEngaged()
{
  execute_CMD(0x03, 0, 0x0007);    //file between 0-2999 in main folder. Doesn't love folders.//
  delay(2500);
}

void playShuttingDown()
{
  execute_CMD(0x03, 0, 0x0003);    //file between 0-2999 in main folder. Doesn't love folders.//
  delay(1000);
}

void playPhoneChargePort()
{
  execute_CMD(0x03, 0, 0x0009);    //file between 0-2999 in main folder. Doesn't love folders.//
  delay(1800);
}

void playButtonOff()
{
  execute_CMD(0x03, 0, 0x0008);    //file between 0-2999 in main folder. Doesn't love folders.//
  delay(250);
}

void playTempSensorError()
{
  Serial.println(" temp sensor appears to be disconnected ");
  execute_CMD(0x03, 0, 0x0010);    //file between 0-2999 in main folder. Doesn't love folders.//
  delay(2000);
}

void playThermalShutdownWarning()
{
  Serial.println(" Heat critical shutdown triggered once ");
  execute_CMD(0x03, 0, 0x000F);    //file between 0-2999 in main folder. Doesn't love folders.//
  delay(2000);
}

void playExhaustWarning1()
{
  execute_CMD(0x03, 0, 0x0011);
}

void playExhaustWarning2()
{
  execute_CMD(0x03, 0, 0x0012);
}

void playExhaustWarning3()
{
  execute_CMD(0x03, 0, 0x0013);
}

void playExhaustWarning4()
{
  execute_CMD(0x03, 0, 0x0014);
}

void playExhaustWarning5()
{
  execute_CMD(0x03, 0, 0x0015);
}

void playExhaustWarning6()
{
  execute_CMD(0x03, 0, 0x0016);
}

void execute_CMD(byte CMD, byte Par1, byte Par2)
// Excecute the command and parameters
{
  // Calculate the checksum (2 bytes)
  word checksum = -(Version_Byte + Command_Length + CMD + Acknowledge + Par1 + Par2);
  // Build the command line
  byte Command_line[10] = { Start_Byte, Version_Byte, Command_Length, CMD, Acknowledge,
                            Par1, Par2, highByte(checksum), lowByte(checksum), End_Byte
                          };
  //Send the command line to the module
  for (byte k = 0; k < 10; k++)
  {
    mySerial.write( Command_line[k]);
  }
}

void tempConsultThermalTable()
{
  if (thermoSensorNewVal <= 0) //either freezing or sensor wire fell off
    {
      if (heatSensorFailure != true)
      {
        heatSensorFailure = true;
        Serial.println(" temp sensor appears to be disconnected ");
        thermalTriggerPlayState = false;
        heatNominal = false;
        heatMinor = false;
        heatGeneral = false;
        heatMajor = false;
        heatMeltdown = false;
      }
    }
  else if (thermoSensorNewVal - TEMP_BAND <= 75) //below 75 degree centigrade
    {
      if (heatNominal != true)
      {
        heatSensorFailure = false;
        heatNominal = true;
        Serial.println(" temp sensor thinks we're below 75 degrees");
        thermalTriggerPlayState = false;
        heatMinor = false;
        heatGeneral = false;
        heatMajor = false;
        heatMeltdown = false;
      }
      else
      {
        thermalTriggerPlayState = true;
      }
    }
  else if ((thermoSensorNewVal + TEMP_BAND >= 75) && (thermoSensorNewVal + TEMP_BAND <= 85)) //getting warmer
    {
      if (heatMinor != true)
      {
        heatSensorFailure = false;
        heatMinor = true;
        thermalTriggerPlayState = false;
        Serial.println(" temp sensor thinks we're above 75 degrees" );
        heatNominal = false;
        heatGeneral = false;
        heatMajor = false;
        heatMeltdown = false;
      }
      else
      {
        thermalTriggerPlayState = true;
      }
    }
  else if ((thermoSensorNewVal + TEMP_BAND >= 85) && (thermoSensorNewVal + TEMP_BAND <= 97)) //nearing boiling
    {
      if (heatGeneral != true)
      {
        heatSensorFailure = false;
        heatGeneral = true;
        thermalTriggerPlayState = false;
        Serial.println(" temp sensor thinks we're above 85 degrees");
        heatNominal = false;
        heatMinor = false;
        heatMajor = false;
        heatMeltdown = false;
      }
      else
      {
        thermalTriggerPlayState = true;
      }
    }
  else if ((thermoSensorNewVal + TEMP_BAND >= 96) && (thermoSensorNewVal + TEMP_BAND <= 100)) //actually boiling
    {
      if (heatMajor != true)
      {
        heatSensorFailure = false;
        heatMajor = true;
        thermalTriggerPlayState = false;
        Serial.println(" temp sensor thinks we're above 96 degrees");
        heatMinor = false;
        heatNominal = false;
        heatGeneral = false;
        heatMeltdown = false;
      }
      else
      {
        thermalTriggerPlayState = true;
      }
    }
  else if (thermoSensorNewVal + TEMP_BAND >= 99) //engine is likely to explode very soon.
    {
      if (heatMeltdown != true)
      {
        heatSensorFailure = false;
        heatMeltdown = true;
        thermalTriggerPlayState = false;
        Serial.println(" temp sensor thinks we're above 99 degrees");
        heatNominal = false;
        heatMinor = false;
        heatGeneral = false;
        heatMajor = false;
      }
    }
}
void chooseVoiceWarning()
{
  if ((thermoSensorNewVal <= 0) && (heatSensorFailure == true))
  {
    playTempSensorError();
    thermalTriggerPlayState = true;
  }
  else if ((thermoSensorNewVal >= thermoSensorOldVal) && (heatMinor == true))
  {
     playThermalExceed();
     thermalTriggerPlayState = true;
  }
  else if ((thermoSensorNewVal >= thermoSensorOldVal) && (heatGeneral == true))
  {
     playHeatLevelCrit();
     thermalTriggerPlayState = true;
  }
  else if ((thermoSensorNewVal >= thermoSensorOldVal) && (heatMajor == true))
  {
     playMeltdownImminent();
     thermalTriggerPlayState = true;
  }
  else if ((thermoSensorNewVal >= thermoSensorOldVal) && (heatMeltdown == true))
  {
     playThermalShutdownWarning();
     thermalTriggerPlayState = true;
  }
  else if ((thermoSensorNewVal <= thermoSensorOldVal) && (heatGeneral == true))
  {
     thermalTriggerPlayState = true;
  }
  else if ((thermoSensorNewVal <= thermoSensorOldVal) && (heatMinor == true))
  {
     playHeatModerate();
     thermalTriggerPlayState = true;
  }
    else if ((thermoSensorNewVal <= thermoSensorOldVal) && (heatNominal == true))
  {
     playHeatNominal();
     thermalTriggerPlayState = true;
  }
}

void tempConsultExhaustTable()
{
  if (exTempSensorNewVal <= 1) //either freezing or sensor wire fell off
    {
      if (heatExSensorFailure == false)
      {
        heatExSensorFailure = true;
        Serial.println(" temp sensor appears to be disconnected ");
        exTempTriggerPlayState = false;
        heatExNominal = false;
        heatExMinor = false;
        heatExGeneral = false;
        heatExMajor = false;
        heatExMeltdown = false;
      }
    }
  else if (exTempSensorNewVal <= 150)
    {
      if (heatExNominal == false)
      {
        heatExSensorFailure = false;
        heatExNominal = true;
        Serial.println(" Exhaust below 150 degrees ");
        exTempTriggerPlayState = false;
        heatExMinor = false;
        heatExGeneral = false;
        heatExMajor = false;
        heatExMeltdown = false;
      }
      else
      {
        exTempTriggerPlayState = true;
      }
    }
  else if ((exTempSensorNewVal >= 150) && (exTempSensorNewVal <= 400)) //open road running?
    {
      if (heatExMinor == false)
      {
        heatExSensorFailure = false;
        heatExMinor = true;
        exTempTriggerPlayState = false;
        Serial.println(" Exhaust above 150 degrees" );
        heatExNominal = false;
        heatExGeneral = false;
        heatExMajor = false;
        heatExMeltdown = false;
      }
      else
      {
        exTempTriggerPlayState = true;
      }
    }
  else if ((exTempSensorNewVal >= 400) && (exTempSensorNewVal <= 600)) //steep hillclimbing
    {
      if (heatExGeneral == false)
      {
        heatExSensorFailure = false;
        heatExGeneral = true;
        exTempTriggerPlayState = false;
        Serial.println(" Exhaust above 400-450 degrees ");
        heatExNominal = false;
        heatExMinor = false;
        heatExMajor = false;
        heatExMeltdown = false;
      }
      else
      {
        exTempTriggerPlayState = true;
      }
    }
  else if ((exTempSensorNewVal >= 600) && (exTempSensorNewVal <= 700)) //smell the exhaust through those wooden floor boards? the sensor is glowing dull red
    {
      if (heatExMajor == false)
      {
        heatExSensorFailure = false;
        heatExMajor = true;
        exTempTriggerPlayState = false;
        Serial.println( " Exhaust above 700-750 degrees. Danger to manifold, no joke! " );
        heatExMinor = false;
        heatExNominal = false;
        heatExGeneral = false;
        heatExMeltdown = false;
      }
      else
      {
        exTempTriggerPlayState = true;
      }
    }
  else if (exTempSensorNewVal >= 700) //Danger to manifold, no joke!
    {
      if (heatExMeltdown == false)
      {
        heatExSensorFailure = false;
        heatExMeltdown = true;
        exTempTriggerPlayState = false;
        Serial.println(" Unless Max6675 sensor module has glitched, exhaust over 700. Danger to manifold, no joke! ");
        heatExNominal = false;
        heatExMinor = false;
        heatExGeneral = false;
        heatExMajor = false;
      }
    }
}
void chooseExhaustVoiceWarning()
{
  if ((exTempSensorNewVal <= 0) && (heatExSensorFailure == true))
  {
    playTempSensorError();
    exTempTriggerPlayState = true;
  }
  else if ((exTempSensorNewVal >= exTempSensorOldVal) && (heatExMinor == true))
  {
     playExhaustWarning2();
     exTempTriggerPlayState = true;
  }
  else if ((exTempSensorNewVal >= exTempSensorOldVal) && (heatExGeneral == true))
  {
     playExhaustWarning4();
     exTempTriggerPlayState = true;
  }
  else if ((exTempSensorNewVal >= exTempSensorOldVal) && (heatExMajor == true))
  {
     playExhaustWarning6();
     exTempTriggerPlayState = true;
  }
  else if ((exTempSensorNewVal >= exTempSensorOldVal) && (heatExMeltdown == true))
  {
     playMeltdownImminent();
     exTempTriggerPlayState = true;
  }
  else if ((exTempSensorNewVal <= exTempSensorOldVal) && (heatExGeneral == true))
  {
     playExhaustWarning5();
     exTempTriggerPlayState = true;
  }
  else if ((exTempSensorNewVal <= exTempSensorOldVal) && (heatExMinor == true))
  {
     playExhaustWarning3();
     exTempTriggerPlayState = true;
  }
    else if ((exTempSensorNewVal <= exTempSensorOldVal) && (heatExNominal == true))
  {
     playExhaustWarning1();
     exTempTriggerPlayState = true;
  }
}
