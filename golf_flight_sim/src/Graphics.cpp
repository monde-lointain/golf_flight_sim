#include "Graphics.h"
#include "./math/linear.h"
#include "./math/trig.h"
#include "./tracy/tracy/Tracy.hpp"
#include <cmath>

void Graphics::draw_line(SDL_Renderer *renderer, vec2 v1, vec2 v2,
                         Uint32 color) {
  lineColor(renderer, static_cast<Sint16>(v1.x), static_cast<Sint16>(v1.y),
            static_cast<Sint16>(v2.x), static_cast<Sint16>(v2.y), color);
}

void Graphics::draw_crosshair(SDL_Renderer *renderer, float x, float y,
                              Uint32 color) {

  lineColor(renderer, static_cast<Sint16>(x - 2.0f), static_cast<Sint16>(y),
            static_cast<Sint16>(x + 2.0f), static_cast<Sint16>(y), color);

  lineColor(renderer, static_cast<Sint16>(x), static_cast<Sint16>(y - 2.0f),
            static_cast<Sint16>(x), static_cast<Sint16>(y + 2.0f), color);

}

void Graphics::draw_arrow(SDL_Renderer *renderer, vec2 v1, vec2 v2,
                          Uint32 color) {

  // Translate the window coordinates into local space centered around the tip
  // of the arrow
  vec2 v1_local = (v1 - v2) * -1.0f;
  vec2 v2_local = vec2(0.0, 0.0);

  // Get the angle of the arrow relative to the origin
  float angle = fast_atan2(v1_local.y, v1_local.x);

  // Assuming a perfectly horizontal arrow, the end points of the arrowhead will
  // be 3 pixels to the right/left and 3 pixels above/below the main arrowhead.
  float offset = 3.0f;

  // Perform the 2d rotation about the origin
  vec2 arrowhead_point_a = vec2(offset * cosf(angle) - offset * sinf(angle),
                                offset * sinf(angle) + offset * cosf(angle));

  // The second point on the arrowhead will always be perpendicular to the first
  // one.
  vec2 arrowhead_point_b = arrowhead_point_a.perp();

  // Now we will translate the coordinate from local space back into window
  // space.
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
            static_cast<Sint16>(arrowhead_point_b.y), color);

}

void Graphics::draw_force_vector(SDL_Renderer *renderer, vec3 force,
                                 vec2 windowL_coordinates,
                                 vec2 windowR_coordinates,
                                 float windowL_pixels_per_meter,
                                 float windowR_pixels_per_meter,
                                 Sint16 ball_radius, Sint16 windowborderL,
                                 Sint16 windowborderR, Uint32 color) {

  ZoneScoped; // for tracy

  float window_border = static_cast<float>(windowborderL + ball_radius);

  vec2 vector_start_point_windowL =
      vec2(windowL_coordinates.x, windowL_coordinates.y);
  vec2 vector_end_point_windowL =
      vec2(windowL_coordinates.x + force.x * windowL_pixels_per_meter,
           windowL_coordinates.y - force.z * windowL_pixels_per_meter);

  if ((vector_end_point_windowL.x < window_border)
      && vector_start_point_windowL.x < window_border) {

    // Draw the full vector with arrow tip if it's completely in the left
    // window
    Graphics::draw_arrow(renderer, vector_start_point_windowL,
                         vector_end_point_windowL, color);

  } else if (!(vector_end_point_windowL.x < window_border)
             && vector_start_point_windowL.x < window_border) {

    // If the end point is not in the window and the start point is (arrow
    // pointing forwards)
    vector_end_point_windowL = solve_y_linear(
        vector_start_point_windowL, vector_end_point_windowL, window_border);

    Graphics::draw_line(renderer, vector_start_point_windowL,
                        vector_end_point_windowL, color);

  } else if ((vector_end_point_windowL.x < window_border)
             && !(vector_start_point_windowL.x < window_border)) {

    // If the end point is in the window and the starting point isn't (arrow
    // pointing backwards). In this case we'll use windowborderL for the margin
    // to hide the backwards facing arrow head.
    vector_start_point_windowL = solve_y_linear(
        vector_end_point_windowL, vector_start_point_windowL, window_border);

    Graphics::draw_arrow(renderer, vector_start_point_windowL,
                         vector_end_point_windowL, color);
  }

  vec2 vector_start_point_windowR =
      vec2(windowR_coordinates.x, windowR_coordinates.y);
  vec2 vector_end_point_windowR =
      vec2(windowR_coordinates.x - force.y * windowR_pixels_per_meter,
           windowR_coordinates.y - force.x * windowR_pixels_per_meter);

  if ((vector_end_point_windowR.x > window_border)
      && (vector_start_point_windowR.x > window_border)) {

    // Draw the full vector with arrow tip if it's completely in the right
    // window
    Graphics::draw_arrow(renderer, vector_start_point_windowR,
                         vector_end_point_windowR, color);

  } else if (!(vector_end_point_windowR.x > window_border)
             && (vector_start_point_windowR.x > window_border)) {

    // If the end point is not in the window and the start point is (arrow
    // pointing forwards)
    vector_end_point_windowR = solve_y_linear(
        vector_start_point_windowR, vector_end_point_windowR, window_border);

    Graphics::draw_line(renderer, vector_start_point_windowR,
                        vector_end_point_windowR, color);

  } else if ((vector_end_point_windowR.x > windowborderR)
             && !(vector_start_point_windowR.x > window_border)) {

    // If the end point is in the window and the starting point isn't (arrow
    // pointing backwards). In this case we'll use windowborderR for the margin
    // to hide the backwards facing arrow head.
    vector_start_point_windowR = solve_y_linear(
        vector_end_point_windowR, vector_start_point_windowR, window_border);

    Graphics::draw_arrow(renderer, vector_start_point_windowR,
                         vector_end_point_windowR, color);

  }

}
