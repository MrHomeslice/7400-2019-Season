#include "MeeseeksProperties.h"
#include "DataTable\TableController.h"
#include "Control\RobotControl.h"
#include "BIT.h"

BIT::BIT()
    :m_cargoControl(DELIVERY_SYSTEM_LEFT_MOTOR, DELIVERY_SYSTEM_RIGHT_MOTOR, DELIVERY_SYSTEM_INTAKE_MOTOR, DELIVERY_SYSTEM_CAPTURE_MOTOR),
     b_testInProgress(true),
     b_testComplete(false)
{
    m_BIT_State = eSwerveTest;
}

void BIT::Run()
{
    //SEND HELP
    if(b_testInProgress)
    {
        switch(m_BIT_State)
        {
            case eSwerveTest:
            {
                b_testComplete = m_swerve.BITTest();
                
                if(b_testComplete)
                {
                    b_testComplete = false;
                    m_BIT_State = eIntakeMotorTest;
                }
                else
                    printf("SWERVE TEST FAILED");
            }

            case eIntakeMotorTest:
            {
                //Insert Test Code
                b_testComplete = true;
                
                if(b_testComplete)
                {
                    b_testComplete = false;
                    m_BIT_State = eIntakeMechanismRotation;
                }
            }

            case eIntakeMechanismRotation:
            {
                //Insert Test Code
                b_testComplete = true;
                
                if(b_testComplete)
                {
                    b_testComplete = false;
                    m_BIT_State = eLadderTest;
                }
            }

            case eLadderTest:
            {
                //Insert Test Code
                b_testComplete = true;
                
                if(b_testComplete)
                {
                    b_testComplete = false;
                    m_BIT_State = eFlipTest;
                }
            }

            case eFlipTest:
            {
                //Insert Test Code
                b_testComplete = true;
                
                if(b_testComplete)
                {
                    b_testComplete = false;
                    m_BIT_State = eCargoGrabberTest;
                }
            }

            case eCargoGrabberTest:
            {
                //Insert Test Code
                b_testComplete = true;
                
                if(b_testComplete)
                {
                    b_testComplete = false;
                    m_BIT_State = eHatchGrabTest;
                }
            }

            case eHatchGrabTest:
            {
                //Insert Test Code
                b_testComplete = true;
                
                if(b_testComplete)
                {
                    b_testComplete = false;
                    m_BIT_State = eHatchLinearTest;
                }
            }

            case eHatchLinearTest:
            {
                //Insert Test Code
                b_testComplete = true;
                if(b_testComplete)
                {
                    b_testInProgress = false;
                }
            }
        }
    }
}