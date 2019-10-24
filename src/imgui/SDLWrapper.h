#pragma once

#include <memory>
#include <stdexcept>
#include <cassert>
#include <fmt/format.h>
#include <SDL/SDL.h>
#include <SDL/SDL_opengl.h>

#include "stb_image.h"

namespace SDL
{

struct Error : std::runtime_error
{
	Error() :
		std::runtime_error{ SDL_GetError() }
	{}
};

struct Initialization final
{
	Initialization(Uint32 flags)
	{
		if (SDL_Init(flags) < 0) throw Error{};
	}

	~Initialization()
	{
		SDL_Quit();
	}
};


struct Surface final
{
	std::unique_ptr<SDL_Surface, void(*)(SDL_Surface*)> ptr{ nullptr, SDL_FreeSurface };

	Surface() = default;

	explicit Surface(SDL_Surface* ptr_) :
		ptr{ ptr_, SDL_FreeSurface }
	{
		assert(ptr_);
	}

	~Surface()
	{
		if (m_stbiData) {
			stbi_image_free(m_stbiData);
		}
	}

	Surface(const Surface&) = delete;
	Surface& operator=(const Surface&) = delete;

	Surface(Surface&& other) noexcept :
		ptr{ std::move(other.ptr) },
		m_stbiData{ other.m_stbiData }
	{
		other.m_stbiData = nullptr;
	}
	
	Surface& operator=(Surface&& other) noexcept
	{
		if (m_stbiData) {
			stbi_image_free(m_stbiData);
		}

		ptr = std::move(other.ptr);
		m_stbiData = other.m_stbiData;
		other.m_stbiData = nullptr;

		return *this;
	}

	void load(const char* path)
	{
		if (m_stbiData) {
			stbi_image_free(m_stbiData);
		}

		int width, height;
		m_stbiData = stbi_load(path, &width, &height, nullptr, STBI_rgb_alpha);
		if (!m_stbiData) throw std::runtime_error{ fmt::format("failed loading `{}`", path) };

		ptr.reset(SDL_CreateRGBSurfaceWithFormatFrom(m_stbiData, width, height, 32, 4 * width, SDL_PIXELFORMAT_RGBA32));
		if (!ptr) throw Error{};
	}

	static Surface fromConvertedSurface(Surface& s, Uint32 pixelFormat)
	{
		SDL_Surface* conv = SDL_ConvertSurfaceFormat(s.ptr.get(), pixelFormat, 0);
		if (!conv) throw Error{};

		return Surface{ conv };
	}

	static GLuint loadTexture(const char* path)
	{
		SDL::Surface s;
		s.load(path);
		s = std::move(SDL::Surface::fromConvertedSurface(s, SDL_PIXELFORMAT_RGBA32));

		GLuint id;

		glGenTextures(1, &id);
		glBindTexture(GL_TEXTURE_2D, id);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, s.ptr.get()->w, s.ptr.get()->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, s.ptr.get()->pixels);
		glBindTexture(GL_TEXTURE_2D, 0);

		return id;
	}

private:
	unsigned char* m_stbiData{};
};

struct SharedObject final
{
	std::unique_ptr<void, void(*)(void*)> ptr{ nullptr, SDL_UnloadObject };

	void load(const char* path)
	{
#ifdef __linux__
		static const char* s_sharedLibraryExtension = ".so";
#else
		static const char* s_sharedLibraryExtension = ".dll";
#endif

		ptr.reset(SDL_LoadObject(fmt::format("{}{}", path, s_sharedLibraryExtension).c_str()));
		if (!ptr) throw Error{};
	}

	template <class T>
	T loadFunction(const char* name)
	{
		void* func = SDL_LoadFunction(ptr.get(), name);
		if (!func) throw Error{};

		return reinterpret_cast<T>(func);
	}
};

struct GLContext final
{
	std::unique_ptr<void, void(*)(SDL_GLContext)> ptr{ nullptr, SDL_GL_DeleteContext };

	explicit GLContext(SDL_GLContext ptr_) :
		ptr{ ptr_, SDL_GL_DeleteContext }
	{
		assert(ptr_);
	}
};

struct Window final
{
	std::unique_ptr<SDL_Window, void(*)(SDL_Window*)> ptr{ nullptr, SDL_DestroyWindow };

	void create(const char* title, int x, int y, int w, int h, Uint32 flags)
	{
		ptr.reset(SDL_CreateWindow(title, x, y, w, h, flags));
		if (!ptr) Error{};
	}

	GLContext createContext()
	{
		SDL_GLContext glContext = SDL_GL_CreateContext(ptr.get());
		if (!glContext) throw Error{};

		return GLContext{ glContext };
	}

	void makeCurrent(GLContext& context)
	{
		SDL_GL_MakeCurrent(ptr.get(), context.ptr.get());
	}

	void swap()
	{
		SDL_GL_SwapWindow(ptr.get());
	}

	void setIcon(Surface& icon)
	{
		SDL_SetWindowIcon(ptr.get(), icon.ptr.get());
	}
};

}