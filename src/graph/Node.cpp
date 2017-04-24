#include "Node.hpp"

#include "Edge.hpp"
#include "../gl.hpp"
#include <GL/glut.h>

namespace GraphStructure {
Node::Node(wxPoint pos, const wxString &label):
    pos(pos), label(label), radius(10) {}

void Node::render(bool selected) const {
    //Fill
    if(selected)
        glColor3ub(221, 204, 187);
    else
        glColor3ub(221, 221, 221);
    glCircle(pos.x, pos.y, radius, radius, true);

    //Outline
    glLineWidth(2);
    if(selected)
        glColor3ub(221, 136, 136);
    else
        glColor3ub(15, 15, 15);
    glCircle(pos.x, pos.y, radius, radius, false);

    glColor3ub(15, 15, 15);
    glLineWidth(0.7);
    glPushMatrix();
    glTranslatef(pos.x + radius, pos.y + radius + 8, 0);
    glScalef(0.15f, -0.12f, 0);
    std::for_each(label.begin(), label.end(), [](const char &ch) {
        glutStrokeCharacter(GLUT_STROKE_ROMAN, ch);;
    });
    glPopMatrix();
}
}
