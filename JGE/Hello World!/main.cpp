#include <psp2/kernel/modulemgr.h>
//#include <psp2/display.h>
#include <psp2/ctrl.h>
#include <psp2/io/fcntl.h>
#include <psp2/rtc.h>
//#include <psp2/paf/graphics.h>
//#include <psp2/gxm.h>
//#include <psp2/heap.h>
#include <psp2/kernel/processmgr.h> 
#include <psp2/types.h>

#include <SDL2/SDL.h>
//#include <GLES3/gl3.h>
#include <vitaGL.h>
#include <JGE.h>
#include <JApp.h>
#include <JRenderer.h>
#include <JSoundSystem.h>

#include "GameApp.h"
//#include "Wlan.h"

//#include "debugScreen.h"
#include <stdio.h>
#include <map>

//#define printf psvDebugScreenPrintf

using namespace std;

SDL_Window    * gWindow   = NULL;
SDL_Renderer  * gRenderer = NULL;


SDL_GLContext glContext;
SDL_Event event;
//SDL_GameController* gameController = nullptr;
SDL_Joystick* joystick = nullptr;

bool done = false;

JGE* g_engine = NULL;
JApp* g_app = NULL;

unsigned int lastTickCount;

u32 gButtons = 0;
u32 gOldButtons = 0;
double analogX = 0; 
double analogY = 0; 
string textInput = "";

map<int, int> gGamepadMap = {
    {0, CTRL_TRIANGLE},
    {1, CTRL_CIRCLE},
    {2, CTRL_CROSS},
    {3, CTRL_SQUARE},
    {4, CTRL_LTRIGGER},
    {5, CTRL_RTRIGGER},
    {6, CTRL_DOWN},
    {7, CTRL_LEFT},
    {8, CTRL_UP},
    {9, CTRL_RIGHT},
    {10, CTRL_SELECT},
    {11, CTRL_START}
};

bool JGEGetButtonState(u32 button)
{
    return (gButtons & button) == button;
}

bool JGEGetButtonClick(u32 button)
{
    return (gButtons & button) == button && (gOldButtons & button) != button;
}

u8 JGEGetAnalogX()
{
    return (analogX + 1) * 127.0;  // Convert [-1,1] range to [0,255]
}

u8 JGEGetAnalogY()
{
    return (analogY + 1) * 127.0;  // Convert [-1,1] range to [0,255]
}

void JGEGetMouseMovement(int *x, int *y)
{
    // PS Vita does not have a mouse, consider using touchscreen or analog sticks
}

string JGEGetTextInput()
{
    return textInput;
}

void process_input()
{
    gOldButtons = gButtons;
    gButtons = 0;
    textInput = "";

    while (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
		case SDL_TEXTINPUT:
			textInput = event.text.text;
			break;
		case SDL_TEXTEDITING:
			break;
		case SDL_QUIT:
			done = true;
			break;
        case SDL_JOYBUTTONDOWN:
            printf("Button %d pressed\n", event.jbutton.button);  // 输出按钮的索引
            break;
		default:
			break;
		}
    }

    if (joystick != nullptr) {
        for (const auto& entry : gGamepadMap) {
            int buttonIndex = entry.first;
            int buttonState = SDL_JoystickGetButton(joystick, buttonIndex);  // 使用 SDL_JoystickGetButton

            if (buttonState == 1) {  // 按钮被按下
                gButtons |= entry.second;
            }
        }
        analogX = SDL_JoystickGetAxis(joystick, SDL_CONTROLLER_AXIS_LEFTX) / 32767.0f;  // 使用 SDL_JoystickGetAxis
        analogY = SDL_JoystickGetAxis(joystick, SDL_CONTROLLER_AXIS_LEFTY) / 32767.0f;
    }
}

void main_loop() 
{ 
    process_input();

    unsigned int tickCount = SDL_GetTicks();   // Get The Tick Count
    unsigned int delta = tickCount - lastTickCount;
	g_engine->SetDelta(delta / 1000.0f);
	g_engine->Update();
	g_engine->mClicked = false;
    lastTickCount = tickCount;
    
	g_engine->Render();
    SDL_GL_SwapWindow(gWindow);

    
}

int InitGame(GLvoid)
{
    
	g_app = new GameApp();
	g_app->Create();
    g_engine = JGE::GetInstance();
	g_engine->SetApp(g_app);
    
	
	JRenderer::GetInstance()->Enable2D();
	
	lastTickCount = SDL_GetTicks();

    srand (time(NULL));

	return true;
}

int main(int argc, char *argv[])
{

    if( SDL_Init( SDL_INIT_VIDEO | SDL_INIT_JOYSTICK) < 0 )
        return -1;

    if ((gWindow = SDL_CreateWindow( "RedRectangle", 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_OPENGL)) == NULL)
        return -1;


    glContext = SDL_GL_CreateContext(gWindow);
    if (glContext == NULL)
    {
        printf("OpenGL context creation failed: %s\n", SDL_GetError());
        return -1;
    }

    joystick = SDL_JoystickOpen(0);
    if (joystick == NULL) {
        printf("Joystick open failed: %s\n", SDL_GetError());
    }

    glViewport(0, 0, (GLsizei)SCREEN_WIDTH, (GLsizei)SCREEN_HEIGHT);


  InitGame();

  while (true)
    {
        main_loop();
    }

  SDL_GL_DeleteContext(glContext);
  SDL_DestroyWindow( gWindow );
  gWindow = NULL;
  gRenderer = NULL;

  SDL_Quit();
  return 0;
}

