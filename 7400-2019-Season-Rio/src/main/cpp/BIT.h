#include "defines.h"

#include <frc/wpilib.h>
#include "Gyro/NavXGyro.h"
#include "Control/RobotControl.h"
#include "SwerveDrive/SwerveDrive.h"
#include "Pneumatics/Pneumatics.h"
#include "SubSystems/CargoControl.h"


typedef enum
{
    eSwerveTest = 0,
    eIntakeMotorTest,
    eIntakeMechanismRotation,
    eLadderTest,
    eFlipTest,
    eCargoGrabberTest,
    eHatchGrabTest,
    eHatchLinearTest,
}BIT_State;



class BIT
{
    public : BIT();
            void Run();

            BIT_State m_BIT_State;

            bool b_testInProgress,b_testComplete;
    protected :
            NavXGyro     m_gyro;
			SwerveDrive  m_swerve;
			CargoControl m_cargoControl;

};
