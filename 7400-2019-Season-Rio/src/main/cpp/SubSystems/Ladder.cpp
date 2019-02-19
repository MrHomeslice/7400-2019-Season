#include "Ladder.h"
#include "..\DataTable\TableController.h"
#include "..\Control\RobotControl.h"

extern TableController g_tc;
extern RobotControl	   g_rc;

#define ELEVATOR_MASTER  10
#define ELEVATOR_SLAVE   11

const double kDefaultLadderF = 0.0;
const double kDefaultLadderP = 0.3;
const double kDefaultLadderI = 0.0;
const double kDefaultLadderD = 0.0;

Ladder::Ladder()
{
    m_pDrives[0] = new WPI_TalonSRX_(ELEVATOR_MASTER, "Ladder Drive 1");
    m_pDrives[1] = new WPI_TalonSRX_(ELEVATOR_SLAVE, "Ladder Drive 2");

    m_ladderState = eLadderStateDisabled;
}

void Ladder::Initialize()
{
    m_pDrives[1]->Follow(*m_pDrives[0]);

    m_pDrives[0]->ConfigPeakOutputReverse(-0.4);
    m_pDrives[0]->ConfigPeakOutputForward(1.0);    
    
    double f = g_tc.GetDouble("Ladder/F", kDefaultLadderF);
	double p = g_tc.GetDouble("Ladder/P", kDefaultLadderP);
	double i = g_tc.GetDouble("Ladder/I", kDefaultLadderI);
	double d = g_tc.GetDouble("Ladder/D", kDefaultLadderD);

    ctre::phoenix::ErrorCode err = m_pDrives[0]->Config_kF(kPIDLoopIdx, f, kTimeoutMs);
    if (err != 0) printf("%s Ladder, error %d Config_kF\n", m_name.c_str(), err);

    err = m_pDrives[0]->Config_kP(kPIDLoopIdx, p, kTimeoutMs);
    if (err != 0) printf("%s Ladder, error %d Config_kP\n", m_name.c_str(), err);

    err = m_pDrives[0]->Config_kI(kPIDLoopIdx, i, kTimeoutMs);
    if (err != 0) printf("%s Ladder, error %d Config_kI\n", m_name.c_str(), err);

    err = m_pDrives[0]->Config_kD(kPIDLoopIdx, d, kTimeoutMs);
    if (err != 0) printf("%s Ladder, error %d Config_kD\n", m_name.c_str(), err);
}

void Ladder::Periodic()
{
    ProcessLadderStates();
    // Uncoment to following line to force ladder buttons to be enabled
    //m_ladderState = eLadderStateEnabled; 
}

void Ladder::ProcessLadderStates()
{
    int ladderPosition;

    switch(m_ladderState)
    {
        case eLadderStateDisabled :
        {
            g_rc.m_bChangedHeight = false;
            break;
        }

        case eLadderStateEnabled  :
        {
            ladderPosition = SetLadderPosition();
            m_pDrives[0]->Set(motorcontrol::ControlMode::Position, ladderPosition);
            break;
        }
    }
}

int Ladder::SetLadderPosition()
{
    switch(g_rc.m_ladderTargetHeight)
    {
        case eLadderHeightGround      : return LADDER_GROUND_HEIGHT;

        case eLadderHeightCargoShip   : return LADDER_CARGO_SHIP_HEIGHT;

	    case eLadderHeightCargoBottom : return LADDER_CARGO_BOTTOM_HEIGHT;

	    case eLadderHeightCargoMid    : return LADDER_CARGO_MID_HEIGHT;
        
        case eLadderHeightCargoTop    : return LADDER_CARGO_TOP_HEIGHT;
        
        case eLadderHeightHatchBottom : return LADDER_HATCH_BOTTOM_HEIGHT;
        
        case eLadderHeightHatchMid    : return LADDER_HATCH_MID_HEIGHT;
        
        case eLadderHeightHatchTop    : return LADDER_HATCH_TOP_HEIGHT;
        
    }

    return 0;
}

double Ladder::GetLadderPosition()
{
    return g_tc.GetDouble("CANSim/LadderHeight", 1.337);
}