#ifndef CAN_SIMULATOR_HEADER_INCLUDED
#define CAN_SIMULATOR_HEADER_INCLUDED

#include "DataTable/TableController.h"
#include <ctre/Phoenix.h>
#include <frc/WPILib.h>

extern TableController g_tc;
extern bool g_bSim;

using namespace ctre::phoenix;

class DigitalInput_
{
  public    : DigitalInput_(int port, const char *pName, bool bSim = g_bSim);

              bool Get() {
                if (m_bSim) {
                  std::string name = "CANSim/" + m_name + "-Input";

                  return g_tc.GetInt(name.c_str(), 0);       
                }
                else

                return m_digitalInput.Get();
              }

  protected :
              frc::DigitalInput m_digitalInput;
              bool              m_bSim;
              std::string       m_name;
};

class WPI_TalonSRX_
{
  public    : WPI_TalonSRX_(int canID, const char *pName, bool bSim = g_bSim);

              motorcontrol::SensorCollection &GetSensorCollection();

              double GetOutputCurrent();

              void Follow(WPI_TalonSRX_ &master);
              void SetSensorPhase(bool bPhaseSensor);
              void SetSafetyEnabled(bool enabled);
              void Set(double speed);
              void Set(motorcontrol::ControlMode mode, double value);

              int GetSelectedSensorPosition(int pidIdx = 0);
              int GetClosedLoopError(int pidIndex = 0);

              ErrorCode Config_kF(int index, double value, int timeOut);
 	            ErrorCode Config_kP(int index, double value, int timeOut);
 	            ErrorCode Config_kI(int index, double value, int timeOut);
 	            ErrorCode Config_kD(int index, double value, int timeOut);
              ErrorCode ConfigOpenloopRamp(double value, int index);
              ErrorCode ConfigSelectedFeedbackSensor(motorcontrol::FeedbackDevice feedbackDevice, int index, int timeOut);
              ErrorCode ConfigNominalOutputForward(double percentOut, int timeoutMs = 0);
              ErrorCode ConfigNominalOutputReverse(double percentOut, int timeoutMs = 0);
              ErrorCode ConfigPeakOutputForward(double percentOut, int timeoutMs = 0);
              ErrorCode ConfigPeakOutputReverse(double percentOut, int timeoutMs = 0);
              ErrorCode SetSelectedSensorPosition(int sensorPos, int pidIdx = 0, int timeoutMs = 50);
 
              std::string  m_name;
  protected :
              WPI_TalonSRX m_talon;
              bool         m_bSim;              
};


#endif