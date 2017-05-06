#include "Edge.hpp"

#include "Node.hpp"
#include "../gl.hpp"
#include <cmath>
#include <GL/glut.h>

namespace GraphStructure {
Edge::Edge(Node &from, Node &to, int weight, int id):
    from(from), to(to), weight(weight), id(id) {}

void Edge::render(bool selected) const {
    glLineWidth(4);
    if(selected)
        glColor3ub(0, 128, 0);
    else
        glColor3ub(170, 170, 170);

    glBegin(GL_LINES);
    glVertex2f(from.pos.x, from.pos.y);
    glVertex2f(to.pos.x, to.pos.y);
    glEnd();
    constexpr unsigned short arrowSize = 15;
    const float angle = atan2(to.pos.y - from.pos.y, to.pos.x - from.pos.x);

    /*Arrow
    const wxPoint head(
        to.pos.x - arrowSize*cosf(angle)/2.,
        to.pos.y - arrowSize*sinf(angle)/2.
    );

    if(selected)
        glColor3ub(0, 0, 0);
    else
        glColor3ub(170, 170, 170);

    glBegin(GL_TRIANGLES);
    glVertex2f(head.x, head.y);
    glVertex2f(
        head.x - arrowSize*cosf(angle - M_PI/6),
        head.y - arrowSize*sinf(angle - M_PI/6)
    );
    glVertex2f(
        head.x - arrowSize*cosf(angle + M_PI/6),
        head.y - arrowSize*sinf(angle + M_PI/6)
    );
    glVertex2f(head.x, head.y);
    glEnd();
    */

    if(weight != 0) {
        glColor3ub(0, 0, 0);
        glLineWidth(2);
        glPushMatrix();
        glTranslatef((to.pos.x-from.pos.x)/2 + arrowSize*cosf(angle)/10 + from.pos.x -5,
                     (to.pos.y-from.pos.y)/2 + arrowSize*cosf(angle)/10 + from.pos.y -5,
                     0);
        glScalef(0.15f, -0.12f, 0);


        wxString weightStr = std::to_string(weight);
        std::for_each(weightStr.begin(), weightStr.end(), [](const char &ch) {
            glutStrokeCharacter(GLUT_STROKE_ROMAN, ch);;
        });
    }
    glPopMatrix();
}

}
