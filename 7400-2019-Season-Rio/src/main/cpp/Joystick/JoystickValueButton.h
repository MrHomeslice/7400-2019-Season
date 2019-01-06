#ifndef JOYSTICKSTATEBUTTON_H_
#define JOYSTICKSTATEBUTTON_H_

#include "JoystickButton.h"

class JoystickValueButton : public JSButton
{
public :
	JoystickValueButton(int index, int valueCount, int initialValue = 0, bool bLoop = false, int delta = 1);
	~JoystickValueButton() {}

	virtual bool Update(bool bPressed);
	void Initialize();

	void Value(int value) { m_value = value; m_lastValue = value;}
	int Value() { return m_value; }

	int LastValue() { return m_lastValue; }

protected :
	int m_value, m_valueCount, m_initialValue, m_lastValue, m_delta;
	bool m_bLoop;
};

#endif
