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
} BIT_State;

class BIT
{
    public : BIT();
            void Initialize(SwerveDrive *pSwerve, CargoControl *pCargoControl);
            void Periodic();

            BIT_State m_BIT_State;

            bool b_testInProgress,b_testComplete;
    protected :
            SwerveDrive  *m_pSwerve;
			CargoControl *m_pCargoControl;

            int m_modNum, m_testTime;
            double m_angle;

};
