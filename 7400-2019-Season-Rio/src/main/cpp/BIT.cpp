#include "MeeseeksProperties.h"
#include "DataTable\TableController.h"
#include "Control\RobotControl.h"
#include "BIT.h"

BIT::BIT()
{
}

void BIT::Initialize(SwerveDrive *pSwerve, CargoControl *pCargoControl)
{
    m_pSwerve       = pSwerve;
    m_pCargoControl = pCargoControl;

    m_BIT_State = eSwerveTest;

    b_testInProgress = true;
    b_testComplete   = false;

    m_angle    = 0.0;
    m_modNum   = 0;
    m_testTime = 0;
}

void BIT::Periodic()
{
    //SEND HELP
    if(b_testInProgress)
    {
        switch(m_BIT_State)
        {
            case eSwerveTest:
            {
                b_testComplete = m_pSwerve->BITTest(m_angle, m_modNum, m_testTime);
                
                if(b_testComplete)
                {
                    b_testComplete = false;
                    m_BIT_State = eIntakeMotorTest;
                    printf("Motor Test Complete\n");
                }
                
                break;
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
                break;
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
                break;
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
                break;
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
                break;
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
                break;
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
                break;
            }

            case eHatchLinearTest:
            {
                //Insert Test Code
                b_testComplete = true;
                if(b_testComplete)
                {
                    b_testInProgress = false;
                }
                break;
            }
        }
    }
}