//
//  a1.h
//  
//
//  Created by Alex Hockley on 2016-01-26.
//
//

#ifndef _a1_h
#define _a1_h

#define SHAPE_SPHERE    0
#define SHAPE_CUBE      1
#define SHAPE_TORUS     2
#define SHAPE_CONE      3

#define COLOUR_BLUE     0
#define COLOUR_RED      1
#define COLOUR_GREEN    2
#define COLOUR_WHITE    3


typedef struct ShapeInfo {
    int shape;
    int colour;
    int iterations;
    float size;
    float inx;
    float xoff;
    float iny;
    float yoff;
    float inz;
    float zoff;
    float inxr;
    float xroff;
    float xroff;
    float inyr;
    float yroff;
    float inzr;
    float zroff;
    float inxs;
    float xsoff;
    float inys;
    float yoff;
    float inzs;
    float zsoff;
} ShapeInfo;

typedef struct ShapeList {
    ShapeInfo* next;
    ShapeInfo* prev;
    ShapeInfo* current;
} ShapeList;





#endif
