#include <iostream>

#include "../../imgui/ImGuiContextWrapper.h"
#include "../../imgui/SDLWrapper.h"
#include "InputWindow.h"
#include "Mupen.h"

void run()
{
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_COMPATIBILITY);

	SDL::Window window;
	window.create("Controllers configuration", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 490, 695, SDL_WINDOW_OPENGL);
	SDL::GLContext glContext = window.createContext();

	try {
		SDL::Surface icon;
		icon.load("data/ico_input.png");
		window.setIcon(icon);
	}
	catch (...) {}

	ImGuiContextWrapper imguiContextWrapper{ window, glContext };
	imguiContextWrapper.addFont("data/font.ttf", 14.0f);
	
	Mupen::CoreWrapper coreWrapper{ "emulator/mupen64plus" };
	InputWindow inputWindow{ coreWrapper.core };

	bool quit{};
	Uint32 lastTicks{ SDL_GetTicks() };

	GLuint backgroundTexture{};
	try {
		backgroundTexture = SDL::Surface::loadTexture("data/controller.png");
	}
	catch (...) {}

	while (!quit) {
		SDL_Event e;

		while (SDL_PollEvent(&e)) {
			if (e.type == SDL_QUIT) {
				quit = true;
			}
			else {
				imguiContextWrapper.processEvent(e);
				inputWindow.handleEvent(e);
			}
		}

		glClearColor(0.29f, 0.30f, 0.29f, 1);
		glClear(GL_COLOR_BUFFER_BIT);

		imguiContextWrapper.beginFrame(window);
		inputWindow.show();

		if (backgroundTexture != 0) {
			ImGui::SetNextWindowPosCenter();
			ImGui::Begin("##background", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoBackground
				| ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_AlwaysAutoResize);
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 68);
			ImGui::Image(reinterpret_cast<void*>(backgroundTexture), { 587, 587 });
			ImGui::End();
		}

		imguiContextWrapper.endFrame();
		window.swap();

		if (SDL_GetTicks() - lastTicks < 20) {
			SDL_Delay(20 - (SDL_GetTicks() - lastTicks));
		}
		lastTicks = SDL_GetTicks();
	}
}

int main(int argc, char** argv)
{
	SDL_SetHint(SDL_HINT_MOUSE_FOCUS_CLICKTHROUGH, "1");

	try {
		SDL::Initialization init{ SDL_INIT_VIDEO | SDL_INIT_JOYSTICK };
		run();
	}
	catch (const std::exception& e) {
		std::cerr << "exception: " << e.what() << std::endl;
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Exception", e.what(), nullptr);

		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
