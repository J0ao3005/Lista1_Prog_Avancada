#include "geometria.h"

double distancia(Point a, Point b) {
    double dx = b.x - a.x;
    double dy = b.y - a.y;
    return sqrt(dx * dx + dy * dy);
}

bool dentro_dominio(Point a, double r) {
    return (a.x * a.x + a.y * a.y) <= (r * r);
}

double orient(Point a, Point b, Point c) {
    return (b.x - a.x) * (c.y - a.y) - (b.y - a.y) * (c.x - a.x);
}

static bool sobre_segmento(Point p, Point a, Point b) {
    return (p.x <= fmax(a.x, b.x) && p.x >= fmin(a.x, b.x) &&
            p.y <= fmax(a.y, b.y) && p.y >= fmin(a.y, b.y));
}

bool intersecta(Segment s1, Segment s2) {
    double o1 = orient(s1.a, s1.b, s2.a);
    double o2 = orient(s1.a, s1.b, s2.b);
    double o3 = orient(s2.a, s2.b, s1.a);
    double o4 = orient(s2.a, s2.b, s1.b);

    if ((o1 * o2 < 0) && (o3 * o4 < 0)) return true;

    if (fabs(o1) < EPSILON && sobre_segmento(s2.a, s1.a, s1.b)) return true;
    if (fabs(o2) < EPSILON && sobre_segmento(s2.b, s1.a, s1.b)) return true;
    if (fabs(o3) < EPSILON && sobre_segmento(s1.a, s2.a, s2.b)) return true;
    if (fabs(o4) < EPSILON && sobre_segmento(s1.b, s2.a, s2.b)) return true;

    return false;
}

double dist_ponto_segmento(Point p, Segment s) {
    Point u = {s.b.x - s.a.x, s.b.y - s.a.y};
    Point v = {p.x - s.a.x,   p.y - s.a.y};

    double dot_uu = u.x * u.x + u.y * u.y;
    if (dot_uu == 0.0) return distancia(p, s.a);

    double t = (v.x * u.x + v.y * u.y) / dot_uu;
    if (t < 0.0) t = 0.0;
    if (t > 1.0) t = 1.0;

    Point proj = {s.a.x + t * u.x, s.a.y + t * u.y};
    return distancia(p, proj);
}

// Distância mínima entre dois segmentos.
// Se eles se intersectam a distância é zero.
// Caso contrário é o mínimo das quatro distâncias ponto-segmento
// entre cada extremidade de um segmento e o outro segmento.
double dist_segmento_segmento(Segment s1, Segment s2) {
    if (intersecta(s1, s2)) return 0.0;

    double d1 = dist_ponto_segmento(s1.a, s2);
    double d2 = dist_ponto_segmento(s1.b, s2);
    double d3 = dist_ponto_segmento(s2.a, s1);
    double d4 = dist_ponto_segmento(s2.b, s1);

    double menor = d1;
    if (d2 < menor) menor = d2;
    if (d3 < menor) menor = d3;
    if (d4 < menor) menor = d4;
    return menor;
}