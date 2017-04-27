#include "gl.hpp"
#include <cmath>

void glInit2D(int x, int y, int w, int h, uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha) {
    glClearColor(red/255.0f, green/255.0f, blue/255.0f, alpha/255.0f);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_DEPTH_TEST);

    glViewport(x, y, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    glOrtho(x, w, h, y, 1, -1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void glLineRect(float x1, float y1, float x2, float y2) {
    glBegin(GL_LINE_LOOP);
    glVertex2f(x1, y1);
    glVertex2f(x1, y2);
    glVertex2f(x2, y2);
    glVertex2f(x2, y1);
    glEnd();
}

///Source: http://slabode.exofire.net/circle_draw.shtml
void glCircle(float x, float y, float radius, int num_segments, bool filled){
    float theta = 2.0f * 3.1415926f / float(num_segments);
    float c = cosf(theta);
    float s = sinf(theta);
    float t;

    float px = radius;
    float py = 0;

    glBegin(filled ? GL_POLYGON : GL_LINE_LOOP);
    for(int ii = 0; ii < num_segments; ++ii){
        glVertex2f(px + x, py + y);
        t = px;
        px = c*px - s*py;
        py = s*t + c*py;
    }
    glEnd();
}
