#ifndef DELIVERY_SYSTEM_H_INCLUDED
#define DELIVERY_SYSTEM_H_INCLUDED

#include <ctre/Phoenix.h>
#include <frc/wpilib.h>
#include "..\Pneumatics\Pneumatics.h"
#include "..\CanSimulator.h"
#include "..\include\Defines.h"

#define DELIVERY_SYSTEM_LEFT_MOTOR    0
#define DELIVERY_SYSTEM_RIGHT_MOTOR   1
#define DELIVERY_SYSTEM_INTAKE_MOTOR  2
#define DELIVERY_SYSTEM_CAPTURE_MOTOR 3

#define EJECT_CYCLE_COUNT 50
#define FLIP_CYLCE_COUNT  50
#define CAPTURE_MOVE_TIME 50

#define CAPTURE_LOWER_POSITION  100
#define CAPTURE_READY_POSITION  500
#define CAPTURE_RAISE_POSITION 1200
#define MAXIMUM_CAPTURE_ERROR    20
#define MAXIMUM_CAPTURE_CURRENT  10

typedef enum 
{
	eCargoStateNull = 0,
	eCargoStateIntaking,
	eCargoStateWaitingForReady,
	eCargoStateWaitingForAcquired,
	eCargoStateForwardFlip,
	eCargoStateFlipped,
	eCargoStateEjecting,
	eCargoStateEjected,
	eCargoStateBackFlip
} CargoState;

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
				void ReadyCapture();
       			void IntakeMotorOn();
				void GrabCargo();
				void EjectCargo();
				void NewCargoStateCheck();
				void SetNewCargoState(CargoState state);
				
				CargoState GetCargoState();

				static constexpr float kDefaultCaptureF = 0.0;
				static constexpr float kDefaultCaptureP = 10.0;
				static constexpr float kDefaultCaptureI = 0.002;
				static constexpr float kDefaultCaptureD = 100;

	protected :
				const char* CargoStateToString();

				bool MonitorCaptureMotor(int targetPosition, int maxError, double maxCurrent);

				WPI_TalonSRX_     m_leftGrabberMotor, m_rightGrabberMotor, m_intakeMotor, m_cargoCaptureMotor;
				DigitalInput_     m_acquiredSwitch, m_intakingSwitch;
				CargoState        m_cargoState, m_lastCargoState;
				Pneumatics        m_pneumatics;

				int m_ejectCounter, m_flippingCounter;
};

#endif