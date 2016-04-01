
/* Derived from scene.c in the The OpenGL Programming Guide */

/*
 Alex Hockley
 0758114
 March 10 2016
 CIS 4800 Assignment 4
 Ray Tracing
 */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
/*
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
*/
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <GLUT/glut.h>
#include <math.h>


//location data for the camera
float camX = 1;
float camY = 2;
float camZ = 1;


float viewX = 0;
float viewY = 0;
float viewZ = 0;
float viewWidth = 500;
float viewHeight = 500;

int rtX = 1024;
int rtY = 768;

typedef struct Intersection{
  Point* in; //where the intersection goes into the object
  Point* out; //where the intersection leaves the object
  Point* normalIn; //normal direction vector from origin to [x y z]
  Point* normalOut; //normal direction
  int sphereNum;
}Intersection;

typedef struct IntersectionInfo{
  int numInterstections = rtX*rtY;
  Intersection** intersections;
}IntersectionInfo;

typedef struct RayInfo{
  int totalRays = rtX*rtY;
  int raysX = rtX;
  int raysY = rtY;
  Ray** rays;
}RayInfo;

typedef struct Ray{
  Point* origin;
  Point* direction;
}Ray;

typedef struct Point{
  float x;
  float y;
  float z;
}Point;

typedef struct SceneInfo{
  int numSpheres;
  LightInfo* light;
  SphereInfo** spheres;
  RayInfo* rays;
}SceneInfo;

typedef struct LightInfo{
  Point* location;
  float r;
  float g;
  float b;
}LightInfo;

typedef struct SphereInfo{
  Point* location;
  float radius;
  float r;
  float g;
  float b;
}SphereInfo;

SceneInfo* sceneInfo = NULL;


/*  Initialize material property and light source.
 */
void init (void)
{
   GLfloat light_ambient[] = { 0.0, 0.0, 0.0, 1.0 };
   GLfloat light_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
   GLfloat light_specular[] = { 1.0, 1.0, 1.0, 1.0 };
   GLfloat light_full_off[] = {0.0, 0.0, 0.0, 1.0};
   GLfloat light_full_on[] = {1.0, 1.0, 1.0, 1.0};

   GLfloat light_position[] = { lightX, lightY, lightZ, 1.0 };

	/* if lighting is turned on then use ambient, diffuse and specular
	   lights, otherwise use ambient lighting only */
   glLightfv (GL_LIGHT0, GL_AMBIENT, light_ambient);
   glLightfv (GL_LIGHT0, GL_DIFFUSE, light_diffuse);
   glLightfv (GL_LIGHT0, GL_SPECULAR, light_specular);
   glLightfv (GL_LIGHT0, GL_POSITION, light_position);


   glEnable (GL_LIGHTING);
   glEnable (GL_LIGHT0);
   glEnable(GL_DEPTH_TEST);
}

void freeIntersections(IntersectionInfo* ii)
{

}

void normalize(float sX, float sY, float sZ, float dX, float dY, float dZ, float* fX, float* fY, float fZ)
{
  float length = sqrt(pow(dX-sX,2) + pow(dY-sY,2) + pow(dZ-sZ,2));
  fX = (dX-sX) / length;
  fY = (dY-sY) / length;
  fZ = (dZ-sZ) / length;
}

void calcDistance(Point* origin, Point* destination)
{
  return sqrt(pow(destination->x - origin->x,2) + pow(destination->y - origin->y,2) + pow(destination->z - origin->z,2));
}

void calculateIntersections(SceneInfo* si, RayInfo* ri, IntersectionInfo* ii)
{
  if(ii != NULL){
    freeIntersections(ii);
  }
  ii = (IntersectionInfo*)malloc(sizeof(IntersectionInfo));
  ii->intersections = (Intersection**)malloc(sizeof(Intersection*)*ii->numIntersections);



}

Intersection* calculateIntersection(SphereInfo** spheres, int n, Ray* ray)
{

  Intersection* inter = NULL;

  float A = 0,B = 0,C = 0,To = 0,Td = 0;

  float At = 0,Bt = 0,Ct = 0,Tot = 0, Tdt = 0;
  Point* tempPoint = (Point*)malloc(sizeof(Point));
  int flag = 0, firstFlag = 1;
  int sphereNum = -1;

  float lastDistance = 0, tempDistance = 0;


  for(int i = 0; i < n; i++){
    Bt = 2*(ray->direction->point->x * (ray->origin->point->x * spheres[i]->location->x) +
           ray->direction->point->y * (ray->origin->point->y * spheres[i]->location->y) +
           ray->direction->point->z * (ray->origin->point->z * spheres[i]->location->z));
    Ct = pow((ray->origin->point->x - spheres[i]->location->x),2) +
        pow((ray->origin->point->y - spheres[i]->location->y),2) +
        pow((ray->origin->point->z - spheres[i]->location->z),2) - pow(spheres[i]->radius,2);

    if((pow(Bt,2)-4*Ct) == 0) //no intersections
      continue;
    flag = 1;
    //components of quadratic formula
    Tot = -Bt - (sqrt(pow(Bt,2) - (4*Ct) )) / 2;
    Tft = -Bt + (sqrt(pow(Bt,2) - (4*Ct) )) / 2;

    //see if the entry is closer to the camera than the last one
    tempPoint->x = ray->origin->x + ray->direction->x*Tot;
    tempPoint->y = ray->origin->y + ray->direction->y*Tot;
    tempPoint->z = ray->origin->z + ray->direction->z*Tot;

    //if it is, replace the A B C To Td values with the last calculated ones and continue
    tempDistance = calcDistance(tempPoint, ray->origin);
    if(firstFlag == 1 || abs(tempDistance) < abs(lastDistance)){
      firstFlag = 0;
      B = Bt;
      C = Ct;
      To = Tot;
      Tf = Tft;
      lastDistance = tempDistance;
      sphereNum = i;
    }

  }
  if(flag == 1){ //we had an intersection
    inter = (Intersection*) malloc(sizeof(Intersection))
    inter->in = (Point*)malloc(sizeof(Point));
    inter->in->x = ray->origin->x + ray->direction->x*To;
    inter->in->y = ray->origin->y + ray->direction->y*To;
    inter->in->z = ray->origin->z + ray->direction->z*To;

    inter->out = (Point*)malloc(sizeof(Point));
    inter->out->x = ray->origin->x + ray->direction->x*Tf;
    inter->out->y = ray->origin->y + ray->direction->y*Tf;
    inter->out->z = ray->origin->z + ray->direction->z*Tf;

    inter->normalIn = (Point*)malloc(sizeof(Point));
    inter->normalIn->x = (inter->in->x - spheres[sphereNum]->location->x) / spheres[sphereNum]->radius;
    inter->normalIn->y = (inter->in->y - spheres[sphereNum]->location->y) / spheres[sphereNum]->radius;
    inter->normalIn->z = (inter->in->z - spheres[sphereNum]->location->z) / spheres[sphereNum]->radius;

    inter->normalOut = (Point*)malloc(sizeof(Point));
    inter->normalOut->x = (inter->out->x - spheres[sphereNum]->location->x) / spheres[sphereNum]->radius;
    inter->normalOut->y = (inter->out->y - spheres[sphereNum]->location->y) / spheres[sphereNum]->radius;
    inter->normalOut->z = (inter->out->z - spheres[sphereNum]->location->z) / spheres[sphereNum]->radius;

    inter->sphereNum = sphereNum; //store which sphere we intersected
  }
  free(tempPoint);
  return inter;
}


void drawScene(SceneInfo* scene)
{
}

void setupOriginVectors(RayInfo* ri)
{
  for(int i = 0; i < ri->totalRays; i++){
    ri->rays[i]->origin->point->x = camX;
    ri->rays[i]->origin->point->y = camY;
    ri->rays[i]->origin->point->z = camZ;
  }
}

void setupDirectionVectors(RayInfo* ri)
{
  float incX = viewWidth / ri->raysX;
  float incY = viewHeight / ri->raysY;
  int curRay = 0;
  for(int i = 0; i < ri->raysX; i++){
    for(int j = 0; j < ri->raysY; j++){
      normalize(ri->rays[curRay]->origin->point->x, ri->rays[curRay]->origin->point->y, ri->rays[curRay]->origin->point->z,
                incX*i + planeX, incY*j + planeY, planeZ,
                &ri->rays[curRay]->direction->point->x, &ri->rays[curRay]->direction->point->y, &ri->rays[curRay]->direction->point->z);
      curRay++;
    }
  }
}

void display (void)
{
    GLfloat blue[]  = {0.0, 0.0, 1.0, 1.0};
    GLfloat red[]   = {1.0, 0.0, 0.0, 1.0};
    GLfloat green[] = {0.0, 1.0, 0.0, 1.0};
    GLfloat white[] = {1.0, 1.0, 1.0, 1.0};
    GLfloat gray[] = {0.5, 0.5, 0.5, 1.0};

    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


    /* set starting location of objects */
    glPushMatrix ();
    gluLookAt(camX, camY, camZ,
              0, 0, 0,
              0, 1, 0);


    drawScene(sceneInfo);

    glPopMatrix();




    glFlush ();

}


//reshape the window
void reshape(int w, int h)
{
   glViewport (0, 0, (GLsizei) w, (GLsizei) h);
   glMatrixMode (GL_PROJECTION);
   glLoadIdentity ();
   gluPerspective(45.0, (GLfloat)w/(GLfloat)h, 1.0, 10.0);
   glMatrixMode (GL_MODELVIEW);
   glLoadIdentity ();
}



//keyboard input event
void keyboard(unsigned char key, int x, int y)
{
   switch (key) {
      case 27:
      case 'q':
         exit(0);
         break;
   }
}



//reads the map from file
SceneInfo* loadScene(char* fname) {
    printf("Loading shapes\n");
    FILE *fp;
    char instr[10000];
    char temp[10000];

    if ((fp = fopen(fname, "r")) == 0) {
        printf("Error, failed to find the file named %s.\n", fname);
        exit(0);
    }

    int noSpheres = 0;
    int noLights = 0;
    int isSphere = 0; //flag for if we're reading a sphere

    //read file character by character first to count the number of size

    //if string is "light" add to light counter, if sphere add to sphere counter

    SceneInfo* scene = (SceneInfo*) malloc(sizeof(SceneInfo));
    scene->numSpheres = noSpheres;
    scene->light = NULL;
    scene->spheres = (SphereInfo**)malloc(sizeof(SphereInfo*)*scene->numSpheres);
    for(int i = 0; i < scene->numSpheres; i++)
      scene->spheres[i] = NULL;

    rewind(fp);

    //read file character by character again to get the values
    //if we detect sphere, flag sphere and read in 7 floats into a single temp string and send to function to create info
    //otherwise read in 6 floats into a single temp string and send to function to create info
    //if we hit the EOF or a non-alphabetic character before expected, fatal error
    //if we have extra numbers at the end, fatal error

    fclose(fp);

    return scene;
}

LightInfo* readLightLine(char* line)
{
  LightInfo* li = (LightInfo*)malloc(sizeof(LightInfo));
  li->location = (Point*) malloc(sizeof(Point));
  sscanf(line, "%f %f %f %f %f %f", &li->location->x, &li->location->y, &li->location->z, &li->r, &li->g, &li->b);
  return li;
}

SphereInfo* readSphereLine(char* line)
{
  SphereInfo* si = (SphereInfo*)malloc(sizeof(SphereInfo));
  si->location = (Point*) malloc(sizeof(Point));
  sscanf(line, "%f %f %f %f %f %f %f %f", &si->location->x, &si->location->y, &si->location->z, &si->radius, &si->r, &si->g, &si->b);
  return si;
}


RayInfo* allocateRays(LightInfo* li)
{
  RayInfo* ri = (RayInfo*)malloc(sizeof(RayInfo));
  ri->rays = (Ray**)malloc(sizeof(Ray*)*ri->numRays);

  for(int i = 0; i < ri->numRays; i++){
    ri->rays[i] = (Ray*) malloc(sizeof(Ray));
    ri->rays[i]->location = (Point*) malloc(sizeof(Point));
    ri->rays[i]->direction = (Point*) malloc(sizeof(Point));
  }
}

/*  Main Loop
 *  Open window with initial window size, title bar,
 *  RGBA display mode, and handle input events.
 */
int main(int argc, char** argv)
{
   glutInit(&argc, argv);
   glutInitDisplayMode (GLUT_SINGLE | GLUT_RGBA | GLUT_DEPTH);
   glutInitWindowSize (1024, 768);
   glutCreateWindow (argv[0]);

   sceneInfo = loadScene(argv[1]);
   sceneInfo->rays = calculateRays(sceneInfo->light);

   init();
   glutReshapeFunc (reshape);
   glutDisplayFunc(display);
   glutKeyboardFunc (keyboard);
   glutMainLoop();
   return 0;
}
