#include "DCEL.h"

Vector2 Face::getCircumcentre() const {

    auto e = edge;
    Vector2 dir1 = e->to->pos - e->from->pos;
    dir1 = Vector2(-dir1.y, dir1.x);
    Vector2 pos1 = (e->to->pos + e->from->pos) / 2.0;

    e = e->next;

    Vector2 dir2 = e->to->pos - e->from->pos;
    dir2 = Vector2(-dir2.y, dir2.x);
    Vector2 pos2 = (e->to->pos + e->from->pos) / 2.0;

    double t = Vector2::cross(dir2, pos1 - pos2) / Vector2::cross(dir1, dir2);
    return pos1 + dir1 * t;

}

int dcmp(double x) {
    if (fabs(x) < EPS) return 0;
    return x > 0 ? 1 : -1;
}
