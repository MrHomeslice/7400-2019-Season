#include "CanSimulator.h"

WPI_TalonSRX_::WPI_TalonSRX_(int canID, bool bSim)
             : m_talon(canID), m_bSim(bSim)
{
}