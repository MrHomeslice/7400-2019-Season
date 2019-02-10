#ifndef SRC_JOYSTICKREPEATBUTTON_H_
#define SRC_JOYSTICKREPEATBUTTON_H_

#include "JoystickButton.h"
#include <frc/Timer.h>

class JoystickRepeatButton : public JSButton
{
public :
	JoystickRepeatButton(int index, int valueCount, int initialValue = 0,
			bool bLoop = false, int delta = 1, double dwellTime = 1.0, double updateRate = 1.0);
private :
	bool Update(bool bPressed);

	frc::Timer m_timer;
	double m_dwellTime, m_waitTime, m_lastUpdateTime;
	bool m_bWaitingForDwell, m_bTimerRunning;
};

#endif /* SRC_JOYSTICKREPEATBUTTON_H_ */
