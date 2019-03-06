#ifndef DELIVERY_SYSTEM_H_INCLUDED
#define DELIVERY_SYSTEM_H_INCLUDED

#include <ctre/Phoenix.h>
#include <frc/wpilib.h>
#include "..\Pneumatics\Pneumatics.h"
#include "..\CanSimulator.h"
#include "..\include\Defines.h"

#define EJECT_TIME 25

#define CARGO_CURRENT_THRESHOLD 5.0
#define CARGO_CURRENT_ITERATIONS 10

#define FLIP_TIME 50

#define CAPTURE_TILT_CURRENT_THRESHOLD  3.0
#define CAPTURE_TILT_CURRENT_ITERATIONS 5
#define CAPTURE_TILT_DOWN_POSITION 		-2000

typedef enum
{
	eCargoCaptureStateInitialize = 0,
    eCargoStateStationIntake,
    eCargoStateHardPullIn,
    eCargoStateSoftPullIn,
    eCargoStateFlipping,
    eCargoStateFlipped,
    eCargoStateEjecting,
    eCargoStateEjected,
    eCargoStateEmpty
} CargoState;

typedef enum
{
    eCargoCaptureStateInitialize = 0,
    eCargoCaptureStateToReady,
    eCargoCaptureStateUp,
    eCargoCaptureStateMovingUp,
    eCargoCaptureStateDown,
    eCargoCaptureStateMovingDown
} CargoCaptureState;

class CargoControl
{
	public    : CargoControl(int leftGrabberID, int rightGrabberID, int intakeID, int captureID);

				void Periodic();
				
				CargoState 		  GetCargoState();
				CargoCaptureState GetCargoCaptureState();
				
	protected :
				const char* CargoStateToString(CargoState cargoState);

				bool MonitorCaptureMotor(int targetPosition, int maxError, double maxCurrent);

				WPI_TalonSRX 	  m_leftGrabberMotor, m_rightGrabberMotor;
       		 	WPI_TalonSRX 	  m_cargoCaptureTilt, m_cargoCaptureIntake;
				CargoState   	  m_cargoState;
				CargoCaptureState m_cargoCaptureState;
				Pneumatics   	  m_pneumatics;

				int m_ejectCounter, m_flippingCounter;

				int m_leftEncoderPosition;
        		int m_rightEncoderPosition;
        		int m_currentCounter;
      	 		int m_cargoStateCounter;
      	 		int m_hatchGrabCounter;
      	  		int m_captureCurrentCounter;
     	   		int m_DICounter;
        		int m_hatchGrabInitialPosition;

        		bool m_bCargoIntakeTestWaiting;
        		bool m_bGlobeTrotterMode;
        		bool m_bFlipped;
				bool m_bChangeHeight;
};

#endif