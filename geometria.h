#ifndef GEOMETRIA_H
#define GEOMETRIA_H

#include <stdio.h>
#include <stdbool.h>
#include <math.h>

#define EPSILON 1e-10

typedef struct {
    double x, y;
} Point;

typedef struct {
    Point a, b;
} Segment;

double distancia(Point a, Point b);
bool   dentro_dominio(Point a, double r);
double orient(Point a, Point b, Point c);
bool   intersecta(Segment s1, Segment s2);
double dist_ponto_segmento(Point p, Segment s);
double dist_segmento_segmento(Segment s1, Segment s2);

#endif