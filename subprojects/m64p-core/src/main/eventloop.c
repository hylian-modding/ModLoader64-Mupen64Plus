/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *   Mupen64plus - eventloop.c                                             *
 *   Mupen64Plus homepage: https://mupen64plus.org/                        *
 *   Copyright (C) 2008-2009 Richard Goedeken                              *
 *   Copyright (C) 2008 Ebenblues Nmn Okaygo Tillin9                       *
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

#include <SDL.h>
#include <SDL_syswm.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#if !SDL_VERSION_ATLEAST(1, 3, 0)

#define SDL_SCANCODE_ESCAPE SDLK_ESCAPE
#define SDL_NUM_SCANCODES SDLK_LAST
#define SDL_SCANCODE_F5 SDLK_F5
#define SDL_SCANCODE_F7 SDLK_F7
#define SDL_SCANCODE_F9 SDLK_F9
#define SDL_SCANCODE_F10 SDLK_F10
#define SDL_SCANCODE_F11 SDLK_F11
#define SDL_SCANCODE_F12 SDLK_F12
#define SDL_SCANCODE_P SDLK_p
#define SDL_SCANCODE_M SDLK_m
#define SDL_SCANCODE_RIGHTBRACKET SDLK_RIGHTBRACKET
#define SDL_SCANCODE_LEFTBRACKET SDLK_LEFTBRACKET
#define SDL_SCANCODE_F SDLK_f
#define SDL_SCANCODE_SLASH SDLK_SLASH
#define SDL_SCANCODE_G SDLK_g
#define SDL_SCANCODE_RETURN SDLK_RETURN
#define SDL_SCANCODE_0 SDLK_0
#define SDL_SCANCODE_1 SDLK_1
#define SDL_SCANCODE_2 SDLK_2
#define SDL_SCANCODE_3 SDLK_3
#define SDL_SCANCODE_4 SDLK_4
#define SDL_SCANCODE_5 SDLK_5
#define SDL_SCANCODE_6 SDLK_6
#define SDL_SCANCODE_7 SDLK_7
#define SDL_SCANCODE_8 SDLK_8
#define SDL_SCANCODE_9 SDLK_9
#define SDL_SCANCODE_UNKNOWN SDLK_UNKNOWN

#define SDL_SetEventFilter(func, data) SDL_SetEventFilter(func)
#define event_sdl_filter(userdata, event) event_sdl_filter(const event)

#else
SDL_JoystickID l_iJoyInstanceID[10];
#endif

#define M64P_CORE_PROTOTYPES 1
#include "api/callbacks.h"
#include "api/config.h"
#include "api/m64p_config.h"
#include "api/m64p_types.h"
#include "eventloop.h"
#include "main.h"
#include "plugin/plugin.h"
#include "sdl_key_converter.h"
#include "util.h"

/* version number for CoreEvents config section */
#define CONFIG_PARAM_VERSION 1.00

static m64p_handle l_CoreEventsConfig = NULL;

/*********************************************************************************************************
* static variables and definitions for eventloop.c
*/

static int JoyCmdActive[16][2]; /* if extra joystick commands are added above, make sure there is enough room in this array */
                                /* [i][0] is Command Active, [i][1] is Hotkey Active */

static int GamesharkActive = 0;

/*********************************************************************************************************
* sdl event filter
*/
static int SDLCALL event_sdl_filter(void *userdata, SDL_Event *event)
{
    int cmd, action;

    switch (event->type)
    {
    // user clicked on window close button
    case SDL_QUIT:
        main_stop();
        break;

    case SDL_KEYDOWN:
#if SDL_VERSION_ATLEAST(1, 3, 0)
        if (event->key.repeat)
            return 0;

        event_sdl_keydown(event->key.keysym.scancode, event->key.keysym.mod);
#else
        event_sdl_keydown(event->key.keysym.sym, event->key.keysym.mod);
#endif
        return 0;
    case SDL_KEYUP:
#if SDL_VERSION_ATLEAST(1, 3, 0)
        event_sdl_keyup(event->key.keysym.scancode, event->key.keysym.mod);
#else
        event_sdl_keyup(event->key.keysym.sym, event->key.keysym.mod);
#endif
        return 0;

#if SDL_VERSION_ATLEAST(1, 3, 0)
    case SDL_WINDOWEVENT:
        switch (event->window.event)
        {
        case SDL_WINDOWEVENT_RESIZED:
            // call the video plugin.  if the video plugin supports resizing, it will resize its viewport and call
            // VidExt_ResizeWindow to update the window manager handling our opengl output window
            gfx.resizeVideoOutput(event->window.data1, event->window.data2);
            return 0; // consumed the event
            break;

        case SDL_WINDOWEVENT_MOVED:
            gfx.moveScreen(event->window.data1, event->window.data2);
            return 0; // consumed the event
            break;
        }
        break;
#else
    case SDL_VIDEORESIZE:
        // call the video plugin.  if the video plugin supports resizing, it will resize its viewport and call
        // VidExt_ResizeWindow to update the window manager handling our opengl output window
        gfx.resizeVideoOutput(event->resize.w, event->resize.h);
        return 0; // consumed the event
        break;

#ifdef WIN32
    case SDL_SYSWMEVENT:
        if (event->syswm.msg->msg == WM_MOVE)
            gfx.moveScreen(0, 0); // The video plugin is responsible for getting the new window position
        return 0;                 // consumed the event
        break;
#endif
#endif
    }

    return 1; // add this event to SDL queue
}

/*********************************************************************************************************
* global functions
*/

void event_initialize(void)
{
    /* set up SDL event filter and disable key repeat */
#if !SDL_VERSION_ATLEAST(2, 0, 0)
    SDL_EnableKeyRepeat(0, 0);
#endif
    SDL_SetEventFilter(event_sdl_filter, NULL);

#if defined(WIN32) && !SDL_VERSION_ATLEAST(1, 3, 0)
    SDL_EventState(SDL_SYSWMEVENT, SDL_ENABLE);

    if (SDL_EventState(SDL_SYSWMEVENT, SDL_QUERY) != SDL_ENABLE)
        DebugMessage(M64MSG_WARNING, "Failed to change event state: %s", SDL_GetError());
#endif
}

int event_set_core_defaults(void)
{
    return 1;
}

/*********************************************************************************************************
* sdl keyup/keydown handlers
*/

void event_sdl_keydown(int keysym, int keymod)
{
    /* pass all other keypresses to the input plugin */
    input.keyDown(keymod, keysym);
}

void event_sdl_keyup(int keysym, int keymod)
{
    input.keyUp(keymod, keysym);
}