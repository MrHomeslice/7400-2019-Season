#ifndef CAN_SIMULATOR_HEADER_INCLUDED
#define CAN_SIMULATOR_HEADER_INCLUDED

#include "DataTable\TableController.h"
#include <ctre/Phoenix.h>

extern TableController g_tc;

using namespace ctre::phoenix;

class WPI_TalonSRX_
{
  public    : WPI_TalonSRX_(int canID, const char *pName, bool bSim = false);

	ErrorCode Config_kF(int index, double value, int timeOut)
  {
    if (m_bSim) return ErrorCode::OK;

    return m_talon.Config_kF(index, value, timeOut);
  }

	ErrorCode Config_kP(int index, double value, int timeOut)
  {
    if (m_bSim) return ErrorCode::OK;

    return m_talon.Config_kP(index, value, timeOut);
  }

	ErrorCode Config_kI(int index, double value, int timeOut)
  {
    if (m_bSim) return ErrorCode::OK;

    return m_talon.Config_kI(index, value, timeOut);
  }

	ErrorCode Config_kD(int index, double value, int timeOut)
  {
    if (m_bSim) return ErrorCode::OK;

    return m_talon.Config_kD(index, value, timeOut);
  }

  ErrorCode ConfigOpenloopRamp(double value, int index)
  {
    if (m_bSim) return ErrorCode::OK;

    return m_talon.ConfigOpenloopRamp(value, index);
  }

  ErrorCode ConfigSelectedFeedbackSensor(motorcontrol::FeedbackDevice feedbackDevice, int index, int timeOut)
  {
    if (m_bSim) return ErrorCode::OK;

    return m_talon.ConfigSelectedFeedbackSensor(feedbackDevice, index, timeOut);
  }

  void SetSensorPhase(bool bPhaseSensor)
  {
    if (!m_bSim)
      m_talon.SetSensorPhase(bPhaseSensor);
  }

  ErrorCode ConfigNominalOutputForward(double percentOut, int timeoutMs = 0)
  {
    if (m_bSim) return ErrorCode::OK;

    return m_talon.ConfigNominalOutputForward(percentOut, 0);
  }

  ErrorCode ConfigNominalOutputReverse(double percentOut, int timeoutMs = 0)
  {
    if (m_bSim) return ErrorCode::OK;

    return m_talon.ConfigNominalOutputReverse(percentOut, timeoutMs);
  }

  ErrorCode ConfigPeakOutputForward(double percentOut, int timeoutMs = 0)
  {
    if (m_bSim) return ErrorCode::OK;

    return m_talon.ConfigPeakOutputForward(percentOut, 0);
  }

  ErrorCode ConfigPeakOutputReverse(double percentOut, int timeoutMs = 0)
  {
    if (m_bSim) return ErrorCode::OK;

    return m_talon.ConfigPeakOutputReverse(percentOut, timeoutMs);
  }

  void SetSafetyEnabled(bool enabled)
  {
    if (!m_bSim)
      m_talon.SetSafetyEnabled(enabled);
  }

  int GetSelectedSensorPosition(int pidIdx = 0)
  {
    if (m_bSim)
      return 0;

    return m_talon.GetSelectedSensorPosition(pidIdx);
  }

  void Set(double speed)
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

  void Set(motorcontrol::ControlMode mode, double value)
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

  int GetClosedLoopError(int pidIndex = 0)
  {
    if (m_bSim)
      return 0;

    return m_talon.GetClosedLoopError(pidIndex);
  }

  motorcontrol::SensorCollection &GetSensorCollection()
  {
    return m_talon.GetSensorCollection();
  }

  double GetOutputCurrent()
  {
    if (m_bSim)
      return g_tc.GetDouble("CANSim/GrabberCurrent", 1.337);
      //return g_tc.GetDouble("CANSim/GrabberCurrent", 0);
      
    return m_talon.GetOutputCurrent();
  }
	
  //g_tc.PutInt(m_reportSteerTarget.c_str(),    m_steer.GetSelectedSensorPosition(kPIDLoopIdx));
	//g_tc.PutInt(m_reportSteerAnalogRaw.c_str(), m_steer.GetSensorCollection().GetAnalogInRaw());

              std::string  m_name;
  protected :
              WPI_TalonSRX m_talon;
              bool         m_bSim;
              
};


#endif