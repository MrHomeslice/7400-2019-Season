#ifndef DELIVERY_SYSTEM_H_INCLUDED
#define DELIVERY_SYSTEM_H_INCLUDED

#include <ctre/Phoenix.h>
#include <frc/wpilib.h>
#include "..\Pneumatics\Pneumatics.h"
#include "..\CanSimulator.h"

#define DELIVERY_SYSTEM_LEFT_MOTOR    0
#define DELIVERY_SYSTEM_RIGHT_MOTOR   1
#define DELIVERY_SYSTEM_INTAKE_MOTOR  2
#define DELIVERY_SYSTEM_CAPTURE_MOTOR 3

#define EJECT_CYCLE_COUNT 50
#define FLIP_CYLCE_COUNT  50
#define CAPTURE_MOVE_TIME 50

typedef enum 
{
	eCargoStateNull = 0,
	eCargoStateAquiring,
	eCargoStateAquired,
	eCargoStateForwardFlip,
	eCargoStateFlipped,
	eCargoStateEjecting,
	eCargoStateEjected,
	eCargoStateBackFlip
} CargoState;

typedef enum
{
	eCargoCaptureStateUp = 0,
	eCargoCaptureStateMovingDown,
	eCargoCaptureStateDown,
	eCargoCaptureStateMovingUp
} CargoCaptureState;

class CargoControl
{
	public    : CargoControl(int leftID, int rightID, int intakeID, int captureID);

				void Periodic();
				void SetCapturePIDValues();
				void ProcessCargoState();
				void ProcessCaptureState();
				void EjectMotorsOff();
				void IntakeMotorOff();
				void CaptureMotorOff();
				void LowerCapture();
				void RaiseCapture();
                void IntakeMotorOn();
				void GrabCargo();
				void EjectCargo();
				void NewCargoStateCheck();
				void NewCaptureStateCheck();
				void SetNewCargoState(CargoState state);
				void SetNewCaptureState(CargoCaptureState state);

				static constexpr float kDefaultCaptureF = 0.0;
				static constexpr float kDefaultCaptureP = 10.0;
				static constexpr float kDefaultCaptureI = 0.002;
				static constexpr float kDefaultCaptureD = 100;

	protected :
				const char* CargoStateToString();
				const char* CaptureStateToString();

				WPI_TalonSRX_     m_leftGrabberMotor, m_rightGrabberMotor, m_intakeMotor, m_cargoCaptureMotor;
				frc::DigitalInput m_acquiredSwitch;
				CargoState        m_cargoState, m_lastCargoState;
				CargoCaptureState m_captureState, m_lastCaptureState;
				Pneumatics        m_pneumatics;

				int m_ejectCounter, m_flippingCounter, m_captureCounter;
};

#endif