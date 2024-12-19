#include "../include/JTextInput.h"

using namespace std;

JGE* JTextInput::mEngine = NULL;

JTextInput::JTextInput()
{
    mEngine = JGE::GetInstance();
    mIsActive = false;
    mString = "";
}

JTextInput::~JTextInput()
{
}

void JTextInput::Enable()
{
    mIsActive = true;
    mString = "";
}

void JTextInput::Disable()
{
    mIsActive = false;
}

void JTextInput::Update(float dt)
{
    string input = mEngine->GetTextInput();
    mString += input;

    if(mEngine->GetButtonClick(CTRL_CIRCLE))
        mString = mString.substr(0, mString.size()-1);
}

void JTextInput::Render(float x, float y)
{

}