#ifndef _J_TEXTINPUT_
#define _J_TEXTINPUT_

#include "JGE.h"
#include <string>

class JTextInput
{
private:
    static JGE* mEngine;

public:
	std::string mString;
	bool mIsActive;

    JTextInput();
    ~JTextInput();

    void Enable();
	void Disable();

    void Update(float dt);
	void Render(float x, float y);
};

#endif