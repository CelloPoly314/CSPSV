//-------------------------------------------------------------------------------------
//
// JGE++ is a hardware accelerated 2D game SDK for PSP/Windows.
//
// Licensed under the BSD license, see LICENSE in JGE root for details.
// 
// Copyright (c) 2007 James Hui (a.k.a. Dr.Watson) <jhkhui@gmail.com>
// 
//-------------------------------------------------------------------------------------

#include <stdio.h>

#include <JGE.h>
#include <JRenderer.h>
#include <JLBFont.h>

#include "GameApp.h"

//#include "debugScreen.h"

//#define printf psvDebugScreenPrintf


//-------------------------------------------------------------------------------------
// Constructor. Variables can be initialized here.
//
//-------------------------------------------------------------------------------------
GameApp::GameApp()
{
	//mFont = NULL;
}


//-------------------------------------------------------------------------------------
// Destructor.
//
//-------------------------------------------------------------------------------------
GameApp::~GameApp()
{
}


//-------------------------------------------------------------------------------------
// This is the init callback function. You should load and create your in-game 
// resources here.
// 
//-------------------------------------------------------------------------------------
void GameApp::Create()
{

	// Load a bitmap font for displaying text
	printf("yoyo");
	mFont = new JLBFont("data/f3", 16);
	//mFont->SetBase(0);

}


//-------------------------------------------------------------------------------------
// This is the clean up callback function. You should delete all your in-game 
// resources, for example texture and quads, here.
// 
//-------------------------------------------------------------------------------------
void GameApp::Destroy()
{
	// Cleaning up
	if (mFont)
		delete mFont;

}


//-------------------------------------------------------------------------------------
// This is the update callback function and is called at each update frame
// before rendering. You should update the game logic here.
//
//-------------------------------------------------------------------------------------
void GameApp::Update()
{

	JGE* engine = JGE::GetInstance();

	// do a screen shot when the TRIANGLE button is pressed
	if (engine->GetButtonClick(CTRL_TRIANGLE))		
	{
		printf("CTRL_TRIANGLE\n");
	}
	if (engine->GetButtonClick(CTRL_SQUARE))		
	{
		printf("CTRL_SQUARE\n");
	}
	if (engine->GetButtonClick(CTRL_START))		
	{
		printf("CTRL_START\n");
	}
	if (engine->GetButtonClick(CTRL_CROSS))		
	{
		printf("CTRL_CROSS\n");
	}
	if (engine->GetButtonClick(CTRL_CIRCLE))		
	{
		printf("CTRL_CIRCLE\n");
	}
	if (engine->GetButtonClick(CTRL_LTRIGGER))		
	{
		printf("CTRL_LTRIGGER\n");
	}
	if (engine->GetButtonClick(CTRL_RTRIGGER))		
	{
		printf("CTRL_RTRIGGER\n");
	}
	if (engine->GetButtonClick(CTRL_SELECT))		
	{
		printf("CTRL_SELECT\n");
	}
	if (engine->GetButtonClick(CTRL_UP))		
	{
		printf("CTRL_UP\n");
	}
	if (engine->GetButtonClick(CTRL_DOWN))		
	{
		printf("CTRL_DOWN\n");
	}
	if (engine->GetButtonClick(CTRL_LEFT))		
	{
		printf("CTRL_LEFT\n");
	}
	if (engine->GetButtonClick(CTRL_RIGHT))		
	{
		printf("CTRL_RIGHT\n");
	}

	// exit when the CROSS button is pressed
	if (engine->GetButtonClick(CTRL_CROSS))	
	{
		engine->End();
		return;
	}

}


//-------------------------------------------------------------------------------------
// All rendering operations should be done in Render() only.
// 
//-------------------------------------------------------------------------------------
void GameApp::Render()
{

	// get JRenderer instance
	JRenderer* renderer = JRenderer::GetInstance();		

	// clear screen to black
	renderer->ClearScreen(ARGB(0,0,0,0));
	
	// set font colour
 	mFont->SetColor(ARGB(255,255,255,255));				
	
	// render text
 	mFont->DrawString("Hello World!", 240.0f, 130.0f, JGETEXT_CENTER); 
	//printf("hey");

}


//-------------------------------------------------------------------------------------
// This function is called when the system wants to pause the game. You can set a flag
// here to stop the update loop and audio playback.
//
//-------------------------------------------------------------------------------------
void GameApp::Pause()
{
	// we haven't had anything to do here yet
}


//-------------------------------------------------------------------------------------
// This function is called when the game returns from the pause state.
//
//-------------------------------------------------------------------------------------
void GameApp::Resume()
{
	// we haven't had anything to do here yet
}
