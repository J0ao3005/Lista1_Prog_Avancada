#include "geometria.h"

double distancia (Point a, Point b) {
    double Dist_x = b.x - a.x;
    double Dist_y = b.y - a.y;
    return sqrt((Dist_x * Dist_x) + (Dist_y * Dist_y));
}

bool dentro_dominio (Point a, double r){
    return (a.x * a.x + a.y * a.y) <= (r * r);
}

double orient(Point a, Point b, Point c) {
    double seg_a = (b.x - a.x) * (c.y - a.y);
    double seg_b = (b.y - a.y) * (c.x - a.x);
    return seg_a - seg_b;
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
    if (dot_uu == 0.0)
        return distancia(p, s.a);

    double t = (v.x * u.x + v.y * u.y) / dot_uu; 

    // Clamp: garante que t fique em [0, 1]
    if (t < 0.0) t = 0.0;
    if (t > 1.0) t = 1.0;

    // Ponto mais próximo sobre o segmento
    Point mais_proximo = {s.a.x + t * u.x, s.a.y + t * u.y};

    return distancia(p, mais_proximo);
}