#ifndef SRC_JOYSTICKBUTTON_H_
#define SRC_JOYSTICKBUTTON_H_

class JSButton
{
public :
	JSButton(int index);

	virtual ~JSButton() {}
	virtual bool Update(bool bPressed);
	virtual void Initialize();

	void Changed(bool bChanged) { m_bChanged = bChanged; }

	/*@return true if the value of the button has changed*/
	bool Changed() { return m_bChanged; }

	/*@return true while the button is pressed and held*/
	bool Pressed() { return m_bPressed; }
	int Index() { return m_index; }

protected :
	bool m_bPressed, m_bChanged;
	int  m_index;
};

#endif /* SRC_JOYSTICKBUTTON_H_ */
