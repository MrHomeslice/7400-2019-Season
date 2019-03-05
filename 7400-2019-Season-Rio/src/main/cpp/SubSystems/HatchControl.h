#ifndef HATCH_CONTROL_HEADER_INCLUDED
#define HATCH_CONTROL_HEADER_INCLUDED

#include <ctre/Phoenix.h>
#include "..\CanSImulator.h"

typedef enum
{
    eHatchSliderStateInitialize = 0,
    eHatchSliderStateMovingIn,
    eHatchSliderStateIn,
    eHatchSliderStateMovingOut,
    eHatchSliderStateOut
} HatchSliderState;

typedef enum
{
    eHatchGrabStateInitialize = 0,
    eHatchGrabStateAcquring,
    eHatchGrabStateAcquried,
    eHatchGrabStateNotHolding,
    eHatchGrabStateEjecting,
    eHatchGrabStateWaiting,
} HatchGrabState;

#define HATCH_CURRENT_ITERATIONS 10
#define HATCH_SLIDER_INITIALIZE_CURRENT_THRESHOLD 1.3
#define HATCH_SLIDER_IN_CURRENT_THRESHOLD 0.3
#define HATCH_SLIDER_OUT_CURRENT_THRESHOLD 1.8

#define HATCH_GRAB_CURRENT_THRESHOLD 1.2
#define HATCH_GRAB_ITERATIONS 25
#define HATCH_GRAB_HOLDING_POSITION 273
#define HATCH_GRAB_NOT_HOLDING_POSITION 350

class HatchControl
{
	public    : HatchControl(int movementID, int grabberID, int hatchID);

                void Periodic();

                HatchSliderState GetHatchMoveState();
                HatchGrabState   GetHatchGrabState();

	protected :
                const char *HatchGrabStateToString(HatchGrabState hatchGrabState);
                const char *HatchSliderStateToString(HatchSliderState hatchSliderState);

				WPI_TalonSRX  m_hatchGrab, m_hatchSlide;   

                HatchSliderState  m_hatchSliderState;
                HatchGrabState    m_hatchGrabState;

                int m_hatchGrabCounter, m_hatchGrabInitialPosition, m_currentCounter;
};

#endif