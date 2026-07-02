#ifndef GEOMETRIA_H
#define GEOMETRIA_H

#include <stdio.h>
#include <stdbool.h>
#include <math.h>

#define EPSILON 1e-10

//Estruturas Geométricas

typedef struct {
    double x, y;
} Point;

typedef struct {
    Point a, b;
} Segment;

//Funções
double distancia (Point a, Point b);
bool dentro_dominio (Point a, double r);
double orient(Point a, Point b, Point c);
bool intersecta(Segment s1, Segment s2);
double dist_ponto_segmento(Point p, Segment s);
static bool sobre_segmento(Point p, Point a, Point b);

#endif