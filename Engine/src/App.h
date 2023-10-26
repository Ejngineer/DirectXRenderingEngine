#pragma once

#include "Window.h"

class App
{
public:
	App();
	~App();
	int Go();
private:
	void Frame();
	Window wnd;
};