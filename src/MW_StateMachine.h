
#ifndef _H_MW_StateMachine
#define _H_MW_StateMachine

#include <stdint.h>

typedef enum
{
    STATE_STANDBY,
    STATE_COLOR_SETTING

} MWSM_TypeState;

void MWSM_InitalizeStateMachine();
void MWSM_RunStateMachine();

#endif
