#pragma once

#ifdef __WXMAC__
#include "OpenGL/gl.h"
#else
#include <GL/gl.h>
#endif

#include <cstdint>

/**
 * Initiate 2d rendering
 */
void glInit2D(int x, int y, int w, int h, uint8_t red = 0, uint8_t green = 0, uint8_t blue = 0, uint8_t alpha = 0);

void glLineRect(float x1, float y1, float x2, float y2);

void glCircle(float x, float y, float radius, int num_segments, bool filled);
