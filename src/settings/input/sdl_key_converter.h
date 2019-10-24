/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *   Mupen64plus - sdl_key_converter.h                                     *
 *   Mupen64Plus homepage: http://code.google.com/p/mupen64plus/           *
 *   Copyright (C) 2013 Mupen64plus development team                       *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.          *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include <cstdint>
#include <SDL/SDL.h>

std::uint16_t sdl_keysym2scancode(std::uint16_t keysym);
std::uint16_t sdl_scancode2keysym(std::uint16_t scancode);

#if SDL_VERSION_ATLEAST(1,3,0)

static inline std::uint16_t sdl_keysym2native(std::uint16_t keysym)
{
    return sdl_keysym2scancode(keysym);
}

static inline std::uint16_t sdl_native2keysym(std::uint16_t native)
{
    return sdl_scancode2keysym(native);
}

#else

static std::uint16_t sdl_keysym2native(std::uint16_t keysym)
{
    return keysym;
}

static std::uint16_t sdl_native2keysym(std::uint16_t native)
{
    return native;
}

#endif
