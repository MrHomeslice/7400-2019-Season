#ifndef SRC_JOYSTICKUPDOWNBUTTON_H_
#define SRC_JOYSTICKUPDOWNBUTTON_H_

#include "JoystickValueButton.h"

class JoystickUpDownButton
{
public :
	JoystickUpDownButton(JoystickValueButton *pUpButton, JoystickValueButton *pDownButton);

	bool Update();

	bool Changed() {return m_bChanged; }
	int Value() { return m_value; }
	int LastValue() { return m_lastValue; }

private :
	JoystickValueButton *m_pUpButton, *m_pDownButton;
	int  m_value, m_lastValue;
	bool m_bChanged;
};

#endif /* SRC_JOYSTICKUPDOWNBUTTON_H_ */
