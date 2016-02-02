
/* Derived from scene.c in the The OpenGL Programming Guide */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

/* Linux Headers */
/*
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
*/

/* OSX Headers */
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <GLUT/glut.h>

#define SHAPE_SPHERE    0
#define SHAPE_CUBE      1
#define SHAPE_TORUS     2
#define SHAPE_CONE      3

#define COLOUR_BLUE     0
#define COLOUR_RED      1
#define COLOUR_GREEN    2
#define COLOUR_WHITE    3

int shapesDrawn = 0;


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
    float inyr;
    float yroff;
    float inzr;
    float zroff;
    float inxs;
    float xsoff;
    float inys;
    float ysoff;
    float inzs;
    float zsoff;
} ShapeInfo;

	/* number of lines in the input file */
int numberLevels;

ShapeInfo** shapes = NULL;
int* rh = NULL;

	/* flags used to control the appearance of the image */
int lineDrawing = 1;	// draw polygons as solid or lines
int lighting = 0;	// use diffuse and specular lighting
int smoothShading = 0;  // smooth or flat shading


/*  Initialize material property and light source.
 */
void init (void)
{
   GLfloat light_ambient[] = { 0.0, 0.0, 0.0, 1.0 };
   GLfloat light_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
   GLfloat light_specular[] = { 1.0, 1.0, 1.0, 1.0 };
   GLfloat light_full_off[] = {0.0, 0.0, 0.0, 1.0};
   GLfloat light_full_on[] = {1.0, 1.0, 1.0, 1.0};

   GLfloat light_position[] = { 1.0, 1.0, 1.0, 0.0 };

	/* if lighting is turned on then use ambient, diffuse and specular
	   lights, otherwise use ambient lighting only */
   if (lighting == 1) {
      glLightfv (GL_LIGHT0, GL_AMBIENT, light_ambient);
      glLightfv (GL_LIGHT0, GL_DIFFUSE, light_diffuse);
      glLightfv (GL_LIGHT0, GL_SPECULAR, light_specular);
   } else {
      glLightfv (GL_LIGHT0, GL_AMBIENT, light_full_on);
      glLightfv (GL_LIGHT0, GL_DIFFUSE, light_full_off);
      glLightfv (GL_LIGHT0, GL_SPECULAR, light_full_off);
   }
   glLightfv (GL_LIGHT0, GL_POSITION, light_position);

   glEnable (GL_LIGHTING);
   glEnable (GL_LIGHT0);
   glEnable(GL_DEPTH_TEST);
	/* needed to correct lightin when glScalef() isn't == 1 */
   glEnable(GL_NORMALIZE);
}

/* Transformation matrix should be set before calling this. So this draws it at the current location*/
void drawShape(ShapeInfo* si)
{
  GLfloat blue[]  = {0.0, 0.0, 1.0, 1.0};
  GLfloat red[]   = {1.0, 0.0, 0.0, 1.0};
  GLfloat green[] = {0.0, 1.0, 0.0, 1.0};
  GLfloat white[] = {1.0, 1.0, 1.0, 1.0};

  switch(si->colour)
  {
    case(COLOUR_BLUE):
      glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, blue);
      break;
    case(COLOUR_RED):
      glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, red);
      break;
    case(COLOUR_GREEN):
      glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, green);
      break;
    case(COLOUR_WHITE):
      glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, white);
      break;
  }

  switch(si->shape)
  {
    case(SHAPE_SPHERE):
      glutSolidSphere (si->size, 30, 30);
      break;
    case(SHAPE_CUBE):
      glutSolidCube(si->size);
      break;
    case(SHAPE_TORUS):
      glutSolidTorus(si->size/2.0f, si->size, 10, 10);
      break;
    case(SHAPE_CONE):
      glutSolidCone(si->size, si->size, 30, 30);
      break;
    default:
      printf("Shape not supported");
  }

  return;
}

void drawObjects2(int level, int numits, int curlevel, int curit)
{
   
    //if we leave the bounds of the "tree" start recursing back
    if(curlevel >= level || curit >= shapes[curlevel]->iterations)
        return;
    
    
    if(curit > 0)
    {
        glTranslatef(shapes[curlevel]->xoff,shapes[curlevel]->yoff, shapes[curlevel]->zoff);
        
        glRotatef(shapes[curlevel]->xroff, 1 , 0, 0);
        glRotatef(shapes[curlevel]->yroff, 0 , 1, 0);
        glRotatef(shapes[curlevel]->zroff, 0 , 0, 1);
        
        glScalef(shapes[curlevel]->xsoff ,shapes[curlevel]->ysoff, shapes[curlevel]->zsoff);
    }
    else
    {
        
        glTranslatef(shapes[curlevel]->inx, shapes[curlevel]->iny, shapes[curlevel]->inz);
        
        glRotatef(shapes[curlevel]->inxr, 1 , 0, 0);
        glRotatef(shapes[curlevel]->inyr, 0 , 1, 0);
        glRotatef(shapes[curlevel]->inzr, 0 , 0, 1);
        
        glScalef(shapes[curlevel]->inxs ,shapes[curlevel]->inys, shapes[curlevel]->inzs);
        
    }
    
    drawShape(shapes[curlevel]);
    shapesDrawn++;
    
    
    //go down a level
    drawObjects2(level, numits, curlevel+1, 0);
   
    //go right an iteration
    drawObjects2(level, numits, curlevel, curit+1);
    
    
}

void display (void)
{

   glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	/* draw surfaces as either smooth or flat shaded */
   if (smoothShading == 1)
      glShadeModel(GL_SMOOTH);
   else
      glShadeModel(GL_FLAT);

	/* draw polygons as either solid or outlines */
   if (lineDrawing == 1)
      glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
   else
      glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	/* give all objects the same shininess value */
   glMaterialf(GL_FRONT, GL_SHININESS, 30.0);

	/* set starting location of objects */
   glPushMatrix ();
   glRotatef(180.0, 1.0, 0.0, 0.0);
   glRotatef(180.0, 0.0, 1.0, 0.0);
   glRotatef(180.0, 0.0, 0.0, 1.0);
   glTranslatef(0.0, 0.0, -15.0);
	/* function which calls transformations and drawing of objects */
    //glPopMatrix();
   drawObjects2(numberLevels, 0, 0, 0);
    
    int num;
    glGetIntegerv(GL_MODELVIEW_STACK_DEPTH, &num);
    printf("Stack size: %d\n", num);
    printf("Done");

    printf("Shapes drawn %d\n", shapesDrawn);
    shapesDrawn=0;
   glPopMatrix ();

   glFlush ();
}

void reshape(int w, int h)
{
   glViewport (0, 0, (GLsizei) w, (GLsizei) h);
   glMatrixMode (GL_PROJECTION);
   glLoadIdentity ();
   gluPerspective(45.0, (GLfloat)w/(GLfloat)h, 1.0, 50.0);
   glMatrixMode (GL_MODELVIEW);
   glLoadIdentity ();
}

void keyboard(unsigned char key, int x, int y)
{
   switch (key) {
      case 27:
      case 'q':
         exit(0);
         break;
      case '1':		// draw polygons as outlines
         lineDrawing = 1;
         lighting = 0;
         smoothShading = 0;
         init();
         display();
         break;
      case '2':		// draw polygons as filled
         lineDrawing = 0;
         lighting = 0;
         smoothShading = 0;
         init();
         display();
         break;
      case '3':		// diffuse and specular lighting, flat shading
         lineDrawing = 0;
         lighting = 1;
         smoothShading = 0;
         init();
         display();
         break;
      case '4':		// diffuse and specular lighting, smooth shading
         lineDrawing = 0;
         lighting = 1;
         smoothShading = 1;
         init();
         display();
         break;
   }
}


	/* read data file and store in arrays */
ShapeInfo** readFile(char **argv)
{
    FILE *fp;
    char instr[1024];
    numberLevels = 0;
    int curLine = 0;
    ShapeInfo** shapeArray = NULL;


	/* open file and print error message if name incorrect */
   if ((fp = fopen(argv[1], "r")) == NULL) {
      printf("ERROR, could not open file.\n");
      printf("Usage: %s <filename>\n", argv[0]);
      exit(1);
   }

    while(fgets(instr,1024, fp) != NULL){
        if(instr[0] == '#') /* ignore comment in line count */
            continue;
        numberLevels++;
    }

    shapeArray = (ShapeInfo**) malloc(sizeof(ShapeInfo*) * numberLevels); //init array

    rewind(fp);
    while(fgets(instr, 1024, fp) != NULL){
        if(instr[0] == '#') /* ignore comment */
            continue;

        shapeArray[curLine] = (ShapeInfo*) malloc(sizeof(ShapeInfo));
        sscanf(instr, "%d %d %d %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f",
               &shapeArray[curLine]->shape,
               &shapeArray[curLine]->colour,
               &shapeArray[curLine]->iterations,
               &shapeArray[curLine]->size,
               &shapeArray[curLine]->inx,
               &shapeArray[curLine]->xoff,
               &shapeArray[curLine]->iny,
               &shapeArray[curLine]->yoff,
               &shapeArray[curLine]->inz,
               &shapeArray[curLine]->zoff,
               &shapeArray[curLine]->inxr,
               &shapeArray[curLine]->xroff,
               &shapeArray[curLine]->inyr,
               &shapeArray[curLine]->yroff,
               &shapeArray[curLine]->inzr,
               &shapeArray[curLine]->zroff,
               &shapeArray[curLine]->inxs,
               &shapeArray[curLine]->xsoff,
               &shapeArray[curLine]->inys,
               &shapeArray[curLine]->ysoff,
               &shapeArray[curLine]->inzs,
               &shapeArray[curLine]->zsoff
               );
        curLine++;

    };


	/* the code to read the input file goes here */
	/* numlevels is set to the number of lines in the file not including the first comment line */

    fclose(fp);

    return shapeArray;
}


/*  Main Loop
 *  Open window with initial window size, title bar,
 *  RGBA display mode, and handle input events.
 */
int main(int argc, char** argv)
{

   shapes = readFile(argv);
    
    int i = 0;
    for(i = 0; i < numberLevels; i++)
    {
        printf("%d\n", shapes[i]->iterations);
    }

   glutInit(&argc, argv);
   glutInitDisplayMode (GLUT_SINGLE | GLUT_RGBA | GLUT_DEPTH);
   glutInitWindowSize (1024, 768);
   glutCreateWindow (argv[0]);

   init();

   glutReshapeFunc (reshape);
   glutDisplayFunc(display);
   glutKeyboardFunc (keyboard);

   glutMainLoop();
   return 0;
}
