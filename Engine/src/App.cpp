#include "App.h"

App::App()
	:
	wnd(1920, 1080, "EJngineer DirectX Rendering Engine")
{

}

App::~App()
{
}

int App::Go()
{
	//message loop
	while (true)
	{
		if (const auto ecode = Window::ProcessMessage())
		{
			return *ecode;
		}
		Frame();
	}
	return 0;
}

void App::Frame()
{
}
