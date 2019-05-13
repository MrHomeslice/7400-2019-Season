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
	bool Changed() { return m_bChanged; }
	bool Pressed() { return m_bPressed; }
	int Index() { return m_index; }

protected :
	bool m_bPressed, m_bChanged;
	int  m_index;
};

#endif /* SRC_JOYSTICKBUTTON_H_ */
