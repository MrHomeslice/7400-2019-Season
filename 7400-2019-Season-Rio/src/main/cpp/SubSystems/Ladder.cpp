#include "Ladder.h"
#include "..\DataTable\TableController.h"
#include "..\Control\RobotControl.h"

extern TableController g_tc;
extern RobotControl	   g_rc;

Ladder::Ladder()
{
    m_pDrives[0] = new WPI_TalonSRX_(0, "Ladder Drive 1", true);
    m_pDrives[1] = new WPI_TalonSRX_(1, "Ladder Drive 2", true);

    m_ladderState = eLadderStateDisabled;
}

void Ladder::Initialize()
{
    double f = g_tc.GetDouble("Swerve/Steer/F", kDefaultDrivesF);
	double p = g_tc.GetDouble("Swerve/Steer/P", kDefaultDrivesP);
	double i = g_tc.GetDouble("Swerve/Steer/I", kDefaultDrivesI);
	double d = g_tc.GetDouble("Swerve/Steer/D", kDefaultDrivesD);

    for(int n = 0; n < 2; n++)
    {
        ctre::phoenix::ErrorCode err = m_pDrives[n]->Config_kF(kPIDLoopIdx, f, kTimeoutMs);
        if (err != 0) printf("%s Steer, error %d Config_kF\n", m_name.c_str(), err);

        err = m_pDrives[n]->Config_kP(kPIDLoopIdx, p, kTimeoutMs);
        if (err != 0) printf("%s Steer, error %d Config_kP\n", m_name.c_str(), err);

        err = m_pDrives[n]->Config_kI(kPIDLoopIdx, i, kTimeoutMs);
        if (err != 0) printf("%s Steer, error %d Config_kI\n", m_name.c_str(), err);

        err = m_pDrives[n]->Config_kD(kPIDLoopIdx, d, kTimeoutMs);
        if (err != 0) printf("%s Steer, error %d Config_kD\n", m_name.c_str(), err);
    }
}

void Ladder::Periodic()
{
    ProcessLadderStates();
}

void Ladder::ProcessLadderStates()
{
    switch(m_ladderState)
    {
        case eLadderStateDisabled :
        {
            g_rc.m_bChangedHeight = false;
        }

        case eLadderStateEnabled  :
        {
            m_pDrives[0]->Set(SetLadderPosition());
            m_pDrives[1]->Set(SetLadderPosition());
        }
    }
}

int Ladder::SetLadderPosition()
{
    switch(g_rc.m_ladderTargetHeight)
    {
        case eLadderHeightGround      : return LADDER_GROUND_HEIGHT;

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