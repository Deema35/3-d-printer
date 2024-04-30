#include <Arduino.h>

#define UPS_PIN 0
#define LED_BUTTON_PIN 1
#define BACK_LIGHT_PIN 2
#define BUTTON_PIN 3
#define MOSFET_PIN 4
#define PRESSDELAY 50

enum class RegulatorState
{
  FistInicial,
  MosfetOn,
  MosfetOff,
  PowerLoss
};

RegulatorState CurrentState = RegulatorState::FistInicial;
RegulatorState StateBeforePowerLoss = RegulatorState::MosfetOn;
unsigned long LastPress = 0;
bool LasButtonState = false;

void ChangeState(RegulatorState NewState);
bool ButtonHasPressed();

void setup()
 {
  pinMode(LED_BUTTON_PIN, OUTPUT);
  pinMode(MOSFET_PIN, OUTPUT);
  pinMode(BACK_LIGHT_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT);
  pinMode(UPS_PIN, INPUT);

	digitalWrite(BACK_LIGHT_PIN, LOW);

}

void loop()
 {
	
	switch (CurrentState)
	{
		case RegulatorState::FistInicial:

			if (!digitalRead(UPS_PIN))
			{
				ChangeState(RegulatorState::MosfetOn);
			}
			break;

		case RegulatorState::MosfetOn:

			if (digitalRead(UPS_PIN))
			{
				ChangeState(RegulatorState::PowerLoss);
			}

			else if (ButtonHasPressed())
			{
				ChangeState(RegulatorState::MosfetOff);
			}
			break;

		case RegulatorState::MosfetOff:

			if (digitalRead(UPS_PIN))
			{
				ChangeState(RegulatorState::PowerLoss);
			}

			else if (ButtonHasPressed())
			{
				ChangeState(RegulatorState::MosfetOn);
			}
			break;

		case RegulatorState::PowerLoss:

			if (!digitalRead(UPS_PIN))
			{
				ChangeState(StateBeforePowerLoss);
				break;
			}

			delay(500);
  			digitalWrite(LED_BUTTON_PIN, HIGH);
  			delay(500);
  			digitalWrite(LED_BUTTON_PIN, LOW);
			break;
	}
}


void ChangeState(RegulatorState NewState)
{
  switch (NewState)
  {
    case RegulatorState::MosfetOff:

		digitalWrite(MOSFET_PIN, LOW);
		digitalWrite(LED_BUTTON_PIN, LOW);
		digitalWrite(BACK_LIGHT_PIN, HIGH);

	break;

	case RegulatorState::MosfetOn:

		digitalWrite(MOSFET_PIN, HIGH);
		digitalWrite(LED_BUTTON_PIN, HIGH);
		digitalWrite(BACK_LIGHT_PIN, HIGH);

	break;

	case RegulatorState::PowerLoss:

		StateBeforePowerLoss = CurrentState;

		digitalWrite(LED_BUTTON_PIN, LOW);
		digitalWrite(BACK_LIGHT_PIN, LOW);

	break;

	default: return;
  }
	CurrentState = NewState;
	return;
}

bool ButtonHasPressed()
{
	boolean butt = !digitalRead(BUTTON_PIN);

	if (millis() - LastPress > PRESSDELAY && LasButtonState != butt)
	{
		
		LastPress = millis();
		LasButtonState = butt;
		if (butt) return true;
		else return false;
	}
	else return false;
}