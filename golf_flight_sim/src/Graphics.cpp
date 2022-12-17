#include "Graphics.h"
#include "./math/trig.h"
#include <cmath>

void Graphics::draw_line(SDL_Renderer *renderer, float x0, float y0, float x1,
                         float y1, Uint32 color) {
  lineColor(renderer, static_cast<Sint16>(x0), static_cast<Sint16>(y0),
            static_cast<Sint16>(x1), static_cast<Sint16>(y1), color);
}

void Graphics::draw_crosshair(SDL_Renderer *renderer, float x, float y,
                              Uint32 color) {
  lineColor(renderer, static_cast<Sint16>(x - 2.0f), static_cast<Sint16>(y),
            static_cast<Sint16>(x + 2.0f), static_cast<Sint16>(y), color);
  lineColor(renderer, static_cast<Sint16>(x), static_cast<Sint16>(y - 2.0f),
            static_cast<Sint16>(x), static_cast<Sint16>(y + 2.0f), color);
}

void Graphics::draw_arrow(SDL_Renderer *renderer, vec2 v1, vec2 v2, Uint32 color) {

  // Translate the window coordinates into local space centered around the tip of the arrow
  vec2 v1_local = (v1 - v2) * -1.0f;
  vec2 v2_local = vec2(0.0, 0.0);

  // Get the angle of the arrow relative to the origin
  float angle = fast_atan2(v1_local.y, v1_local.x);

  // The length of the arrowheads will be 5 pixels, giving us an offset of
  // 5/sqrt(2) on both x and y axes for each point.
  float offset = 3.0f;

  // Perform the 2d rotation about the origin
  vec2 arrowhead_point_a =
      vec2(offset * cosf(angle) - offset * sinf(angle),
           offset * sinf(angle) + offset * cosf(angle));

  // The second point on the arrowhead will always be perpendicular to the first one.
  vec2 arrowhead_point_b = arrowhead_point_a.perp();

  // Now we will translate the coordinate from local space back into window space.
  arrowhead_point_a *= -1.0f;
  arrowhead_point_b *= -1.0f;
  arrowhead_point_a += v2;
  arrowhead_point_b += v2;

  // And finally, draw all the lines making up the arrow.

  lineColor(renderer, static_cast<Sint16>(v1.x), static_cast<Sint16>(v1.y),
            static_cast<Sint16>(v2.x), static_cast<Sint16>(v2.y), color);
  lineColor(renderer, static_cast<Sint16>(v2.x), static_cast<Sint16>(v2.y),
            static_cast<Sint16>(arrowhead_point_a.x),
            static_cast<Sint16>(arrowhead_point_a.y), color);
  lineColor(renderer, static_cast<Sint16>(v2.x), static_cast<Sint16>(v2.y),
            static_cast<Sint16>(arrowhead_point_b.x),
            static_cast<Sint16>(arrowhead_point_b.y),
            color);

}
