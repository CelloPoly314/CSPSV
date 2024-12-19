#include "../include/JGE.h"
#include "../include/JApp.h"
#include "../include/JRenderer.h"
#include "../include/JSoundSystem.h"
#include "../include/Vector2D.h"
#include "../include/JFileSystem.h"

using namespace std;


JGE::JGE()
{
	mApp = NULL;
	
	strcpy(mDebuggingMsg, "");
	mCurrentMusic = NULL;
	Init();
}


JGE::~JGE()
{
	JRenderer::Destroy();
	JFileSystem::Destroy();
	JSoundSystem::Destroy();
}


	
void JGE::Init()
{
	mDone = false;
	mPaused = false;
	mCriticalAssert = false;
	
	JRenderer::GetInstance();
	JFileSystem::GetInstance();
	JSoundSystem::GetInstance();
}

void JGE::Run()
{
	
}

void JGE::SetDelta(float delta)
{
	mDeltaTime = delta;
}


float JGE::GetDelta()
{
	return mDeltaTime;
}


float JGE::GetFPS()
{
	return 1.0f/mDeltaTime;
}


bool JGE::GetButtonState(u32 button)
{
	return JGEGetButtonState(button);
}


bool JGE::GetButtonClick(u32 button)
{
	return JGEGetButtonClick(button);
}


u8 JGE::GetAnalogX()
{
	return JGEGetAnalogX();
}


u8 JGE::GetAnalogY()
{
	return JGEGetAnalogY();
}

s8 JGE::GetAnalogRX()
{
	return JGEGetAnalogRX();
}


s8 JGE::GetAnalogRY()
{
	return JGEGetAnalogRY();
}

void JGE::GetMouseMovement(int *x, int *y)
{
	JGEGetMouseMovement(x, y);
}

string JGE::GetTextInput()
{
	return JGEGetTextInput();
}

JGE* JGE::mInstance = NULL;


JGE* JGE::GetInstance()
{
	if (mInstance == NULL)
	{
		mInstance = new JGE();
	}
	
	return mInstance;
}


void JGE::Destroy()
{
	if (mInstance)
	{
		delete mInstance;
		mInstance = NULL;
	}
}


void JGE::SetApp(JApp *app)
{
	mApp = app;
}


void JGE::Update()
{
	if (mApp != NULL)
		mApp->Update();
}

void JGE::Render()
{
	JRenderer* renderer = JRenderer::GetInstance();

	renderer->BeginScene();

	if (mApp != NULL)
		mApp->Render();

	renderer->EndScene();
}


void JGE::End()
{
	mDone = true;
}


void JGE::Pause()
{
	if (mPaused) return;

	mPaused = true;
	if (mApp != NULL)
		mApp->Pause();
}


void JGE::Resume()
{
	if (mPaused)
	{
		mPaused = false;
		if (mApp != NULL)
			mApp->Resume();
	}
}