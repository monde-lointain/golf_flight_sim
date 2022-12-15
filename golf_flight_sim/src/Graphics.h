#pragma once

#include "../lib/SDL2_gfx/SDL2_gfxPrimitives.h"
#include "./Physics/vec2.h"
#include <SDL.h>

namespace Graphics {
void draw_line(SDL_Renderer *renderer, float x0, float y0, float x1, float y1,
               Uint32 color);
void draw_arrow(SDL_Renderer *renderer, vec2 v1, vec2 v2, Uint32 color);
} // namespace Graphics
