#include "CanSimulator.h"

WPI_TalonSRX_::WPI_TalonSRX_(int canID, const char *pName, bool bSim)
             : m_talon(canID), m_bSim(bSim), m_name(pName)
{
}

DigitalInput_::DigitalInput_(int id, const char *pName, bool bSim)
             : m_digitalInput(id), m_bSim(bSim), m_name(pName)
{
}