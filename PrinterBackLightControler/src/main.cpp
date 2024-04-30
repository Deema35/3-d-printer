#include <Arduino.h>

#define BUTTON_PIN 3
#define ENCODER_INT_PIN 2
#define ENCODER_PIN 0
#define MOSFET_PIN 4

#define PRESSDELAY 50
#define ENCODERDELAY 50

PROGMEM const uint8_t CRTgammaPGM[]  = {2, 4, 7, 10, 13, 17, 22, 27, 33, 39, 47, 55, 64, 74, 85, 97, 110, 124, 140, 158, 176, 196, 218, 255};

enum class RegulatorState
{
	LightOff,
	LightOn
};

RegulatorState CurrentState = RegulatorState::LightOff;
unsigned long LastPress = 0;
bool LasButtonState = false;
volatile byte CurrentLightValue = 10;
volatile bool S1EncoderTick = false;
volatile bool S2EncoderTick = false;
bool S2EncoderState = false;

bool ButtonHasPressed();
void ChangeState(RegulatorState NewState);
void SetState(RegulatorState State);
void EncoderTurnS1();
void EncoderTurnS2();
byte GetCRTGamma(byte val);




void setup()
{
	pinMode(BUTTON_PIN,INPUT);
	pinMode(ENCODER_PIN,INPUT);
	pinMode(ENCODER_INT_PIN,INPUT);
	attachInterrupt(digitalPinToInterrupt(ENCODER_INT_PIN),EncoderTurnS1,FALLING);
 	pinMode(MOSFET_PIN,OUTPUT);
	SetState(CurrentState);
}

void loop()
{

	switch (CurrentState)
	{
		case RegulatorState::LightOn:

			if (ButtonHasPressed()) ChangeState(RegulatorState::LightOff);
			if (digitalRead(ENCODER_PIN)) S2EncoderState = true;
			else 
			{
				if (S2EncoderState)
				{
					EncoderTurnS2();
					S2EncoderState = false;
				}
			}
			

			break;

		case RegulatorState::LightOff:

			if (ButtonHasPressed()) ChangeState(RegulatorState::LightOn);

			break;

	}
}

bool ButtonHasPressed()
{
	unsigned long millisval = millis();
	bool butt = !digitalRead(BUTTON_PIN);

	if (millisval - LastPress > PRESSDELAY && LasButtonState != butt)
	{
		
		LastPress = millisval;
		LasButtonState = butt;
		if (butt) return true;
		else return false;
	}
	else return false;
}

void EncoderTurnS1()
{
	if (S2EncoderTick && !digitalRead(ENCODER_PIN))
	{
		
		if (CurrentLightValue != 23)
		{
			CurrentLightValue++;
			analogWrite(MOSFET_PIN, GetCRTGamma(CurrentLightValue));
		}

			
		S2EncoderTick = false;
	}
	else
	{
		S1EncoderTick = true;
	}
  
}

void EncoderTurnS2()
{
	if (S1EncoderTick && !digitalRead(ENCODER_INT_PIN))
	{
		
		if (CurrentLightValue != 0)
		{
			CurrentLightValue--;
			analogWrite(MOSFET_PIN, GetCRTGamma(CurrentLightValue));
		}
		
		S1EncoderTick = false;
	}
	else
	{
		S2EncoderTick = true;
	}
  
}

void SetState(RegulatorState State)
{
	switch (State)
	{
	case RegulatorState::LightOff:

		analogWrite(MOSFET_PIN, 0);
		break;

	case RegulatorState::LightOn:

		analogWrite(MOSFET_PIN, GetCRTGamma(CurrentLightValue));
		break;
	
	default:
		break;
	}
}

void ChangeState(RegulatorState NewState)
{
	SetState(NewState);

	CurrentState = NewState;
}

byte GetCRTGamma(byte val)
{
  return pgm_read_byte(&(CRTgammaPGM[val]));
}