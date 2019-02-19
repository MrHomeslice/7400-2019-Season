#include "CanSimulator.h"
#include "DataTable\TableController.h"
#include <ctre/Phoenix.h>
#include <frc/WPILib.h>

WPI_TalonSRX_::WPI_TalonSRX_(int canID, const char *pName, bool bSim)
             : m_talon(canID), m_bSim(bSim), m_name(pName)
{
}

DigitalInput_::DigitalInput_(int id, const char *pName, bool bSim)
             : m_digitalInput(id), m_bSim(bSim), m_name(pName)
{
}

extern TableController g_tc;
extern bool g_bSim;

void WPI_TalonSRX_::Follow(WPI_TalonSRX_ &master)
{
  if (!m_bSim)
    m_talon.Follow(master.m_talon);
}

ErrorCode WPI_TalonSRX_::Config_kF(int index, double value, int timeOut)
{
  if (m_bSim) return ErrorCode::OK;

  return m_talon.Config_kF(index, value, timeOut);
}

ErrorCode WPI_TalonSRX_::Config_kP(int index, double value, int timeOut)
{
  if (m_bSim) return ErrorCode::OK;

  return m_talon.Config_kP(index, value, timeOut);
}

ErrorCode WPI_TalonSRX_::Config_kI(int index, double value, int timeOut)
{
  if (m_bSim) return ErrorCode::OK;

  return m_talon.Config_kI(index, value, timeOut);
}

ErrorCode WPI_TalonSRX_::Config_kD(int index, double value, int timeOut)
{
  if (m_bSim) return ErrorCode::OK;

  return m_talon.Config_kD(index, value, timeOut);
}

ErrorCode WPI_TalonSRX_::ConfigOpenloopRamp(double value, int index)
{
  if (m_bSim) return ErrorCode::OK;

  return m_talon.ConfigOpenloopRamp(value, index);
}

ErrorCode WPI_TalonSRX_::ConfigSelectedFeedbackSensor(motorcontrol::FeedbackDevice feedbackDevice, int index, int timeOut)
{
  if (m_bSim) return ErrorCode::OK;

  return m_talon.ConfigSelectedFeedbackSensor(feedbackDevice, index, timeOut);
}

void WPI_TalonSRX_::SetSensorPhase(bool bPhaseSensor)
{
  if (!m_bSim)
    m_talon.SetSensorPhase(bPhaseSensor);
}

ErrorCode WPI_TalonSRX_::ConfigNominalOutputForward(double percentOut, int timeoutMs)
{
  if (m_bSim) return ErrorCode::OK;

  return m_talon.ConfigNominalOutputForward(percentOut, timeoutMs);
}

ErrorCode WPI_TalonSRX_::ConfigNominalOutputReverse(double percentOut, int timeoutMs)
{
  if (m_bSim) return ErrorCode::OK;

  return m_talon.ConfigNominalOutputReverse(percentOut, timeoutMs);
}

ErrorCode WPI_TalonSRX_::ConfigPeakOutputForward(double percentOut, int timeoutMs)
{
  if (m_bSim) return ErrorCode::OK;

  return m_talon.ConfigPeakOutputForward(percentOut, timeoutMs);
}

ErrorCode WPI_TalonSRX_::ConfigPeakOutputReverse(double percentOut, int timeoutMs)
{
  if (m_bSim) return ErrorCode::OK;

  return m_talon.ConfigPeakOutputReverse(percentOut, timeoutMs);
}

void WPI_TalonSRX_::SetSafetyEnabled(bool enabled)
{
  if (!m_bSim)
    m_talon.SetSafetyEnabled(enabled);
}

int WPI_TalonSRX_::GetSelectedSensorPosition(int pidIdx)
{
  if (m_bSim) {
    std::string name = "CANSim/" + m_name + "-Position";

    return g_tc.GetDouble(name.c_str(), 0.0);       
  }

  return m_talon.GetSelectedSensorPosition(pidIdx);
}

ErrorCode WPI_TalonSRX_::SetSelectedSensorPosition(int sensorPos, int pidIdx, int timeoutMs)
{
  if (m_bSim)
    return ctre::phoenix::ErrorCode::OK;  

  return m_talon.SetSelectedSensorPosition(sensorPos, pidIdx, timeoutMs);
}

void WPI_TalonSRX_::Set(double speed)
{
  if (!m_bSim)
    m_talon.Set(speed);
  else
  {
    std::string name = "CANSim/";

    name += m_name;

    g_tc.PutDouble(name.c_str(), speed);
  }
}

void WPI_TalonSRX_::Set(motorcontrol::ControlMode mode, double value)
{
  if (!m_bSim)
    m_talon.Set(mode, value);
  else
  {
    std::string name = "CANSim/";

    name += m_name;

    g_tc.PutDouble(name.c_str(), value);
  }

}

int WPI_TalonSRX_::GetClosedLoopError(int pidIndex)
{
  if (m_bSim)
    return 0;

  return m_talon.GetClosedLoopError(pidIndex);
}

motorcontrol::SensorCollection &WPI_TalonSRX_::GetSensorCollection()
{
  return m_talon.GetSensorCollection();
}

double WPI_TalonSRX_::GetOutputCurrent()
{
  if (m_bSim) {

    std::string name = "CANSim/" + m_name + "-Current";

    return g_tc.GetDouble(name.c_str(), 0.0);
  }
    //return g_tc.GetDouble("CANSim/GrabberCurrent", 0);
    
  return m_talon.GetOutputCurrent();
}