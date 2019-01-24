#include "Ladder.h"
#include "..\DataTable\TableController.h"

extern TableController g_tc;

Ladder::Ladder()
{
    m_pDrives[0] = new WPI_TalonSRX(0);
    m_pDrives[1] = new WPI_TalonSRX(1);
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

}