#ifndef SDL_KEYS_H
#define SDL_KEYS_H

    #include <stdint.h>
    #include <SDL.h>

    uint16_t sdl_keysym2scancode(uint16_t keysym);
    uint16_t sdl_scancode2keysym(uint16_t scancode);
    uint16_t sdl_keysym2native(uint16_t keysym);
    uint16_t sdl_native2keysym(uint16_t native);

#endif
