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

class HatchControl
{
	public    : HatchControl(int movementID, int grabberID);

                void Periodic();

	protected :
                double HatchCurrent();
                double GrabberCurrent();

				WPI_TalonSRX m_movementMotor, m_grabberMotor;

                HatchStateMove m_hatchStateMove;
                GrabberState   m_grabberState;
};
