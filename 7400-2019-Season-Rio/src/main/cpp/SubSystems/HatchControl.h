#include <ctre/Phoenix.h>

typedef enum 
{
	eHatchStateMoveIn = 0,
    eHatchStateMoveOut,
    eHatchStateMoveMovingIn,
    eHatchStateMoveMovingOut
} HatchStateMove;

typedef enum
{
    eGrabberStateNull = 0,
    eGrabberStateAquiring,
    eGrabberStateAquired,
    eGrabberStateEjecting,
    eGrabberStateEjected
} GrabberState;

#define GRABBER_THRESHOLD_CURRENT 1
#define HATCH_EJECT_TIME          1
#define HATCH_MOVE_TIME           1

class HatchControl
{
	public    : HatchControl(int movementID, int grabberID);

                void Periodic();
                void Aquire();
                void Eject();
                void GrabbersOff();

	protected :
                double HatchCurrent();
                double GrabberCurrent();

				WPI_TalonSRX m_movementMotor, m_grabberMotor;

                HatchStateMove m_hatchStateMove;
                GrabberState   m_grabberState;

                int m_ejectCounter;
};
