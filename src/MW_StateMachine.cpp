#include "MW_StateMachine.h"
#include "MW_InputHandler.h"
#include "MW_Strip.h"

MWSM_TypeState currentState = STATE_STANDBY;

uint8_t currentColorIndex = 0;

bool sensorPressed[] = {false, false, false};

void StandByState();
void ColorSettingState();

void MWSM_InitalizeStateMachine()
{

  currentState = STATE_STANDBY;
}

void MWSM_RunStateMachine()
{
  MWIH_RunInputHandler(); // Run input handler routine
  StandByState();
  

}

void CheckPressedActionStandBy()
{
  for (uint8_t i = 0; i < 2; i++)
  {
    if (sensorPressed[i] == true)
    {

      MWST_IncreaseStripIlumination(i, 1);
     

    }

  }
}

void StandByState()
{


  switch (MWIH_GetEvent(MWIH_LEFT_SENSOR))
  {
    case MWIH_NO_EVENT:
      break;

    case MWIH_PRESSED:
      sensorPressed[MWIH_LEFT_SENSOR] = true;
      break;
    case MWIH_RELEASED:
      sensorPressed[MWIH_LEFT_SENSOR] = false;
      break;
    case MWIH_SINGLE_TAP:
      // Turn ON LED
      Serial.println("Single");
      MWST_ToggleStripState(STRIP_LEFT, EFFECT_PROGRESIVE_ON);
      break;
    case MWIH_DOUBLE_TAP:
      //MWST_ToggleStripState(STRIP_LEFT, EFFECT_PROGRESIVE_ON);

      currentColorIndex++;
      if (currentColorIndex >= 8)
      {
        currentColorIndex = 0;
      }
      //MWST_SetStripColor(STRIP_BOTH, MWST_COLOR_PALETTE[currentColorIndex]);

      break;

    case MWIH_TRIPLE_TAP:
      break;

    case MWIH_SINGLE_LONG_TAP:
    
      MWST_IncreaseStripIlumination(STRIP_LEFT, 1);
      break;

    default:
      break;
  }

  switch (MWIH_GetEvent(MWIH_RIGHT_SENSOR))
  {
    case MWIH_NO_EVENT:
      break;

    case MWIH_PRESSED:
      sensorPressed[MWIH_RIGHT_SENSOR] = true;
      break;
    case MWIH_RELEASED:
      sensorPressed[MWIH_RIGHT_SENSOR] = false;
      break;
    case MWIH_SINGLE_TAP:
      // Turn ON LED
      Serial.println("Single");
      MWST_ToggleStripState(STRIP_LEFT, EFFECT_PROGRESIVE_ON);
      break;
    case MWIH_DOUBLE_TAP:
      //MWST_ToggleStripState(STRIP_LEFT, EFFECT_PROGRESIVE_ON);

      currentColorIndex++;
      if (currentColorIndex >= 8)
      {
        currentColorIndex = 0;
      }

      break;

    case MWIH_TRIPLE_TAP:
      break;

    case MWIH_SINGLE_LONG_TAP:
    
      MWST_IncreaseStripIlumination(STRIP_LEFT, 1);
      break;

    default:
      break;
  }

 

  //CheckPressedActionStandBy();
}

void ColorSettingState()
{

  switch (MWIH_GetEvent(MWIH_LEFT_SENSOR))
  {
    case MWIH_NO_EVENT:
      break;
    case MWIH_SINGLE_TAP:
      // Turn ON LED
      break;
    case MWIH_DOUBLE_TAP:
      break;

    case MWIH_TRIPLE_TAP:
      break;

    case MWIH_SINGLE_LONG_TAP:
      break;

    default:
      break;
  }

  switch (MWIH_GetEvent(MWIH_RIGHT_SENSOR))
  {
    case MWIH_NO_EVENT:
      break;
    case MWIH_SINGLE_TAP:
      // Turn ON LED

      break;
    case MWIH_DOUBLE_TAP:
      break;

    case MWIH_TRIPLE_TAP:
      break;

    case MWIH_SINGLE_LONG_TAP:
      break;

    default:
      break;
  }
}
