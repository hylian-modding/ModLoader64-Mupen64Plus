#include <sstream>
#include <stdio.h>
#include <iostream>
#include <fmt/format.h>
#include <SDL/SDL_timer.h>
#include <mystd_clamp.h>

#include "HexWindow.h"
#include "../../imgui/ImGuiContextWrapper.h"
#include "../../imgui/ImGuiUtil.h"
#include "../../imgui/SDLWrapper.h"
#include "../../mupen_core/core_interface.h"


namespace 
{
	GLuint backgroundTexture;
	SDL::Window window = SDL::Window();
	SDL::GLContext* context;
	ImGuiContextWrapper* imguiContextWrapper;
}

HexWindow::HexWindow() {
	try
	{
		//SDL_SetHint(SDL_HINT_MOUSE_FOCUS_CLICKTHROUGH, "1");
		//SDL::Initialization init{ SDL_INIT_VIDEO | SDL_INIT_JOYSTICK };
		//SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_COMPATIBILITY);

		window.create("Memory Tool", 32, 32, 600, 800, SDL_WINDOW_OPENGL | SDL_WINDOW_BORDERLESS | SDL_WINDOW_RESIZABLE);
		context = &window.createContext();
		SDL::Surface icon;
		window.setIcon(icon);

		imguiContextWrapper = new ImGuiContextWrapper(window, *context);
		imguiContextWrapper->addFont("data/font.ttf", 14.0f);

		backgroundTexture = SDL::Surface::loadTexture("data/controller.png");

		glClearColor(0, 0, 0, 1);
		glClear(GL_COLOR_BUFFER_BIT);
		window.swap();
	}
	catch (...)
	{
		printf("Failed to init HexWindow!\n");
	}
}

inline void HexWindow::readData()
{
	const uint8_t* hexMem = (*RdRamReadBuffer)(currentAddr.hex, getSize());
	const uint8_t* funcMem = (*RdRamReadBuffer)(currentAddr.func, size.func);

	data.hexData = (uint8_t*)hexMem;
	data.funcData = (uint8_t*)funcMem;
}


void HexWindow::runOnce() {
	window.makeCurrent(*context);
	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT);

	ImGui::SetNextWindowSize(ImVec2(600, 800), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowPos(ImVec2(32, 32));

	imguiContextWrapper->beginFrame(window); //ImGui::NewFrame
	ImGui::Begin("Memory Viewer", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
	ImGui::Text("foo");
	ImGui::End();

	imguiContextWrapper->endFrame(); //ImGui::Render
	window.swap();
}