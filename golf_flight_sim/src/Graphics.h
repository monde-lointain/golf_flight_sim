#pragma once

#include "../lib/SDL2_gfx/SDL2_gfxPrimitives.h"
#include "./Physics/vec2.h"
#include "./Physics/vec3.h"
#include <SDL.h>

namespace Graphics {
void draw_line(SDL_Renderer *renderer, vec2 v1, vec2 v2, Uint32 color);
void draw_crosshair(SDL_Renderer *renderer, float x, float y, Uint32 color);
void draw_arrow(SDL_Renderer *renderer, vec2 v1, vec2 v2, Uint32 color);
void draw_force_vector(SDL_Renderer *renderer, vec3 force, vec2 windowL_coordinates, vec2 windowR_coordinates, float windowL_pixels_per_meter, float windowR_pixels_per_meter, Sint16 ball_radius, Sint16 windowborderL, Sint16 windowborderR, Uint32 color);
} // namespace Graphics
