
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
float camX = 0;
float camY = 0;
float camZ = 0;

float viewX = 0;
float viewY = 0;
float viewZ = -1;
float viewWidth = 2;
float viewHeight = 2;

int rtX = 1024;
int rtY = 768;


//ambient light constants
float IaR = 1.0; //ambient light source
float IaG = 1.0;
float IaB = 1.0;
float Ka = 0.8; //ambient coefficient for objects


//light constants
float IpR = 1.0;
float IpG = 1.0;
float IpB = 1.0;

//diffuse constants
float Kd = 1.0;

//specular constants
float Ks = 1.0;
float OsR = 1.0;
float OsG = 1.0;
float OsB = 1.0;
float specN = 2.0;

GLubyte pixbuff[1024*768*3];

typedef struct Point{
    float x;
    float y;
    float z;
}Point;

typedef struct Intersection{
  Point* in; //where the intersection goes into the object
  Point* out; //where the intersection leaves the object
  Point* normalIn; //normal direction vector from origin to [x y z]
  Point* normalOut; //normal direction
  int sphereNum;
}Intersection;

typedef struct IntersectionInfo{
  int numIntersections;
  Intersection** intersections;
}IntersectionInfo;


typedef struct Ray{
    Point* origin;
    Point* direction;
}Ray;


typedef struct RayInfo{
  int totalRays;
  int raysX;
  int raysY;
  Ray** rays;
}RayInfo;

typedef struct SphereInfo{
    Point* location;
    float radius;
    float r;
    float g;
    float b;
}SphereInfo;

typedef struct LightInfo{
    Point* location;
    float r;
    float g;
    float b;
}LightInfo;

typedef struct SceneInfo{
  int numSpheres;
  LightInfo* light;
  SphereInfo** spheres;
  RayInfo* rays;
  IntersectionInfo* intersectInfo;
}SceneInfo;


SceneInfo* sceneInfo = NULL;
int pixNum = 0;

void generatePixelValue(SceneInfo*, Intersection*);

/*  Initialize material property and light source.
 */
void init (void)
{
   
}

void freeIntersections(IntersectionInfo* ii)
{
    if(ii == NULL)
        return;
    
    if(ii->intersections != NULL){
        for(int i = 0; i < ii->numIntersections; i++){
            if(ii->intersections[i]->in != NULL)
                free(ii->intersections[i]->in);
            if(ii->intersections[i]->out != NULL)
                free(ii->intersections[i]->out);
            if(ii->intersections[i]->normalIn != NULL)
                free(ii->intersections[i]->normalIn);
            if(ii->intersections[i]->normalOut != NULL)
                free(ii->intersections[i]->normalOut);
            free(ii->intersections[i]);
        }
        free(ii->intersections);
    }
    free(ii);
    ii = NULL;
}

void freeScene(SceneInfo* scene)
{
    freeIntersections(scene->intersectInfo);
}

//calculate the length of a vector as defined by starting and destination points
float calculateLength(float sX, float sY, float sZ, float dX, float dY, float dZ)
{
    return fabs(sqrt(pow(dX-sX,2) + pow(dY-sY,2) + pow(dZ-sZ,2)));
}

//normalize a vector and return the points back into pointers supplied for fX, fY, fZ
void normalize(float sX, float sY, float sZ, float dX, float dY, float dZ, float* fX, float* fY, float* fZ)
{
  float length = calculateLength(sX, sY, sZ, dX, dY, dZ);
  *fX = (dX-sX) / length;
  *fY = (dY-sY) / length;
  *fZ = (dZ-sZ) / length;
}

//calculate the distance between two points
float calcDistance(Point* origin, Point* destination)
{
  return sqrt(pow(destination->x - origin->x,2) + pow(destination->y - origin->y,2) + pow(destination->z - origin->z,2));
}

//calculate the dot product of two points
float dotProduct(Point* p1, Point* p2)
{
    return (p1->x*p1->y + p1->y*p2->y + p1->z*p2->z);
}


//calculate the intersection of a ray and the spheres
Intersection* calculateIntersection(SphereInfo** spheres, int n, Ray* ray)
{

  Intersection* inter = NULL;

  float A = 0,B = 0,C = 0,To = 0,Tf = 0;

  float At = 0,Bt = 0,Ct = 0,Tot = 0, Tft = 0;
  Point* tempPoint = (Point*)malloc(sizeof(Point));
  int flag = 0, firstFlag = 1;
  int sphereNum = -1;

  float lastDistance = 0, tempDistance = 0;
    
    
  //for each sphere, calculate the different components and store them in a temporary variable
  for(int i = 0; i < n; i++){
    Bt = 2*(ray->direction->x * (ray->origin->x - spheres[i]->location->x) +
           ray->direction->y * (ray->origin->y - spheres[i]->location->y) +
           ray->direction->z * (ray->origin->z - spheres[i]->location->z));
    Ct = pow((ray->origin->x - spheres[i]->location->x),2) +
        pow((ray->origin->y - spheres[i]->location->y),2) +
        pow((ray->origin->z - spheres[i]->location->z),2) - pow(spheres[i]->radius,2);

      
    if((pow(Bt,2)-4*Ct) < 0){ //no intersections, so skip
      continue;
    }
    flag = 1;
      
    //determine the entry and exit t values using the quadratic formula
    Tot = (-Bt - (sqrt(fabs(pow(Bt,2) - (4.0*Ct) )))) / 2.0;
    Tft = (-Bt + (sqrt(fabs(pow(Bt,2) - (4.0*Ct) )))) / 2.0;
      
      
    //calculate the point of the entry in the world
    tempPoint->x = ray->origin->x + ray->direction->x*Tot;
    tempPoint->y = ray->origin->y + ray->direction->y*Tot;
    tempPoint->z = ray->origin->z + ray->direction->z*Tot;
      
      
    //check it is closer to the camera than the last intersection if it is, replace the A B C To Td values with the last calculated ones and continue
    tempDistance = calcDistance(tempPoint, ray->origin);
      //printf("Distance: %f\n",tempDistance);
      //exit(1);
    if(firstFlag == 1 || fabsf(tempDistance) < fabsf(lastDistance)){
      firstFlag = 0;
      B = Bt;
      C = Ct;
      To = Tot;
      Tf = Tft;
      lastDistance = tempDistance;
      sphereNum = i;
    }

  }
  //we had an intersection
  if(flag == 1){
    //allocate memory and assign values of the intersection
    inter = (Intersection*) malloc(sizeof(Intersection));
      
    //entry intersection point
    inter->in = (Point*)malloc(sizeof(Point));
    inter->in->x = ray->origin->x + ray->direction->x*To;
    inter->in->y = ray->origin->y + ray->direction->y*To;
    inter->in->z = ray->origin->z + ray->direction->z*To;

    //exit intersection point
    inter->out = (Point*)malloc(sizeof(Point));
    inter->out->x = ray->origin->x + ray->direction->x*Tf;
    inter->out->y = ray->origin->y + ray->direction->y*Tf;
    inter->out->z = ray->origin->z + ray->direction->z*Tf;

    //normal direction vector for the entry point
    inter->normalIn = (Point*)malloc(sizeof(Point));
    inter->normalIn->x = (inter->in->x - spheres[sphereNum]->location->x) / spheres[sphereNum]->radius;
    inter->normalIn->y = (inter->in->y - spheres[sphereNum]->location->y) / spheres[sphereNum]->radius;
    inter->normalIn->z = (inter->in->z - spheres[sphereNum]->location->z) / spheres[sphereNum]->radius;

    //normal direction vector for the exit point (may be pointless)
    inter->normalOut = (Point*)malloc(sizeof(Point));
    inter->normalOut->x = (inter->out->x - spheres[sphereNum]->location->x) / spheres[sphereNum]->radius;
    inter->normalOut->y = (inter->out->y - spheres[sphereNum]->location->y) / spheres[sphereNum]->radius;
    inter->normalOut->z = (inter->out->z - spheres[sphereNum]->location->z) / spheres[sphereNum]->radius;

    //store which sphere we intersected, we will need this later
    inter->sphereNum = sphereNum;
  }
  //free up the temporary point
  free(tempPoint);
  return inter;
}

//calculate the intersections for each ray
IntersectionInfo* calculateIntersections(SceneInfo* scene)
{
    IntersectionInfo* ii = (IntersectionInfo*)malloc(sizeof(IntersectionInfo));
    ii->intersections = (Intersection**)malloc(sizeof(Intersection*)*scene->rays->totalRays);
    ii->numIntersections = 0;
    int intersectionNum = 0;
    int curRay = 0;
    for(int i = 0; i < scene->rays->totalRays; i++)
    {
        Intersection* inter = calculateIntersection(scene->spheres, scene->numSpheres, scene->rays->rays[i]);
        if(inter != NULL){
            ii->intersections[intersectionNum] = inter;
            ii->numIntersections++;
            intersectionNum++;
            generatePixelValue(scene, inter);
        }
        else{
            pixbuff[pixNum++] = 0;
            pixbuff[pixNum++] = 0;
            pixbuff[pixNum++] = 0;
        }
        
        
    }
    printf("Number intersections: %d\n", intersectionNum);
    return ii;
}

Point* calculateReflectionVector (Point* normal, Point* lightVector)
{
    Point* r = (Point*)malloc(sizeof(Point));
    
    float dot = dotProduct(lightVector, normal);
    
    r->x = lightVector->x - ((2 * dot) * normal->x);
    r->y = lightVector->y - ((2 * dot) * normal->y);
    r->z = lightVector->z - ((2 * dot) * normal->z);
    
    return r;
}


void generatePixelValue(SceneInfo* scene, Intersection* inter)
{
    float r, g ,b;
    Point* diffuseLightVector = (Point*)malloc(sizeof(Point));
    
    
    //calculate the vectors
    //diffuse
    normalize(inter->in->x, inter->in->y, inter->in->z,
              sceneInfo->light->location->x, sceneInfo->light->location->y, sceneInfo->light->location->z,
              &diffuseLightVector->x, &diffuseLightVector->y, &diffuseLightVector->z);
    
    //calculate r ambient, diffuse and specular combination colour
    r = IaR * Ka * scene->spheres[inter->sphereNum]->r +
        IpR * (Kd * scene->spheres[inter->sphereNum]->r * dotProduct(inter->normalIn, diffuseLightVector));
    
    //calculate g ambient, diffuse and
    g = IaG * Ka * scene->spheres[inter->sphereNum]->g +
    IpG * (Kd * scene->spheres[inter->sphereNum]->g * dotProduct(inter->normalIn, diffuseLightVector));

    
    //calculate b
    b = IaB * Ka * scene->spheres[inter->sphereNum]->b +
        IpB * (Kd * scene->spheres[inter->sphereNum]->b * dotProduct(inter->normalIn, diffuseLightVector));
    
    
    //cap the values so they dont mod themselves magically
    if(r > 255)
        r = 255;
    if(g > 255)
        g = 255;
    if(b > 255)
        b = 255;
    
    //floor the values so they dont mod themselves magically
    if(r < 0)
        r = 0;
    if(g < 0)
        g = 0;
    if(b < 0)
        b = 0;
    
    pixbuff[pixNum++] = r;
    pixbuff[pixNum++] = g;
    pixbuff[pixNum++] = b;
    
    
    //free the temporary calculation vectors
    free(diffuseLightVector);
}

void drawScene()
{
    //draw the pixel buffer to the screen
    glDrawPixels(1024, 768, GL_RGB, GL_UNSIGNED_BYTE, pixbuff);
}

void setupOriginVectors(RayInfo* ri)
{
    //setup the viewing plane
  int curRay = 0;
  for(int i = 0; i < ri->raysY; i++){
      for(int j = 0; j < ri->raysX; j++){
          ri->rays[curRay]->origin->x = camX - (viewWidth/2.0) + j*(viewWidth/rtX);
          ri->rays[curRay]->origin->y = camY - (viewHeight/2.0) + i*(viewHeight/rtY);
          ri->rays[curRay]->origin->z = camZ;
          curRay++;
      }
  }
}

void setupDirectionVectors(RayInfo* ri)
{
    //vectors move in the Z direction
    for(int i = 0; i < ri->totalRays; i++){
        ri->rays[i]->direction->x = 0;
        ri->rays[i]->direction->y = 0;
        ri->rays[i]->direction->z = 1;
    }
}

RayInfo* calculateRays(SceneInfo* scene)
{
    RayInfo* rays = (RayInfo*)malloc(sizeof(RayInfo));
    rays->totalRays = rtX*rtY;
    rays->raysX = rtX;
    rays->raysY = rtY;
    rays->rays = (Ray**)malloc(sizeof(Ray*)*rays->totalRays);
    for(int i = 0; i < rays->totalRays; i++){
        rays->rays[i] = (Ray*)malloc(sizeof(Ray));
        rays->rays[i]->origin = (Point*)malloc(sizeof(Point));
        rays->rays[i]->direction = (Point*)malloc(sizeof(Point));
    }
    setupOriginVectors(rays);
    setupDirectionVectors(rays);
    return rays;
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


    drawScene();

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
    printf("Loading scene\n");
    FILE *fp;
    char instr[10000];
    char temp[10000];

    if ((fp = fopen(fname, "r")) == 0) {
        printf("Error, failed to find the file named %s.\n", fname);
        exit(0);
    }

    int noSpheres = 0;
    int noLights = 0;
    int strCnt = 0;
    

    //read file character by character first to count the number of size
    char c;
    while((c = fgetc(fp)) != EOF){
        
        if(c == '\n' || c == '\t' || c == ' '){ //if whitespace
            if(strCnt > 0){
                if(strcmp(temp,"sphere") == 0)
                    noSpheres++;
                else if(strcmp(temp,"light") == 0)
                    noLights++;
            }
            strCnt = 0;
            memset(temp,'\0',10000);
        }
        else{
            temp[strCnt] = c;
            strCnt++;
        }
    }
    strCnt = 0;
    memset(temp,'\0',10000);

    printf("%d spheres\n", noSpheres);
    printf("%d lights\n", noLights);
    //if string is "light" add to light counter, if sphere add to sphere counter

    SceneInfo* scene = (SceneInfo*) malloc(sizeof(SceneInfo));
    scene->numSpheres = noSpheres;
    scene->light = NULL;
    scene->spheres = (SphereInfo**)malloc(sizeof(SphereInfo*)*scene->numSpheres);
    for(int i = 0; i < scene->numSpheres; i++)
      scene->spheres[i] = NULL;

    rewind(fp);

    int tempCounter = 0;
    int sphereCounter = 0;
    int lightCounter = 0;
    int inVal = 0;
    
    int isSphere = 0; //flag for if we're reading a sphere
    int isLight = 0;
    while((c = fgetc(fp)) != EOF){
        
        if(c == '\n' || c == '\t' || c == ' '){ //if whitespace
            if(strCnt > 0){
                if(strcmp(temp,"sphere") == 0){
                    strCnt = 0;
                    memset(temp,'\0',10000);
                    while((c = fgetc(fp))){
                        if(c == '\n' || c == '\t' || c == ' ' || c == EOF){
                            if(strCnt > 0){
                                switch(tempCounter){
                                    case(0):
                                        scene->spheres[sphereCounter] = (SphereInfo*)malloc(sizeof(SphereInfo));
                                        scene->spheres[sphereCounter]->location = (Point*) malloc(sizeof(Point));
                                        sscanf(temp, "%f", &scene->spheres[sphereCounter]->location->x);
                                        break;
                                    case(1):
                                        sscanf(temp, "%f", &scene->spheres[sphereCounter]->location->y);
                                        break;
                                    case(2):
                                        sscanf(temp, "%f", &scene->spheres[sphereCounter]->location->z);
                                        break;
                                    case(3):
                                        sscanf(temp, "%f", &scene->spheres[sphereCounter]->radius);
                                        break;
                                    case(4):
                                        sscanf(temp, "%f", &scene->spheres[sphereCounter]->r);
                                        break;
                                    case(5):
                                        sscanf(temp, "%f", &scene->spheres[sphereCounter]->g);
                                        break;
                                    case(6):
                                        sscanf(temp, "%f", &scene->spheres[sphereCounter]->b);
                                        break;
                                }
                                tempCounter++;
                            }
                            strCnt = 0;
                            memset(temp,'\0',10000);
                        }
                        else{
                            temp[strCnt] = c;
                            strCnt++;
                        }
                        if(tempCounter > 6){
                            tempCounter = 0;
                            sphereCounter++;
                            break;
                        }
                    }
                }
                else if(strcmp(temp,"light") == 0){
                    strCnt = 0;
                    memset(temp,'\0',10000);
                    while((c = fgetc(fp))){
                        if(c == '\n' || c == '\t' || c == ' ' || c == EOF){
                            if(strCnt > 0){
                                switch(tempCounter){
                                    case(0):
                                        scene->light = (LightInfo*)malloc(sizeof(LightInfo));
                                        scene->light->location = (Point*) malloc(sizeof(Point));
                                        sscanf(temp, "%f", &scene->light->location->x);
                                        break;
                                    case(1):
                                        sscanf(temp, "%f", &scene->light->location->y);
                                        break;
                                    case(2):
                                        sscanf(temp, "%f", &scene->light->location->z);
                                        break;
                                    case(3):
                                        sscanf(temp, "%f", &scene->light->r);
                                        break;
                                    case(4):
                                        sscanf(temp, "%f", &scene->light->g);
                                        break;
                                    case(5):
                                        sscanf(temp, "%f", &scene->light->b);
                                        break;
                                }
                                tempCounter++;
                            }
                            strCnt = 0;
                            memset(temp,'\0',10000);
                        }
                        else{
                            temp[strCnt] = c;
                            strCnt++;
                        }
                        if(tempCounter > 5){
                            tempCounter = 0;
                            lightCounter++;
                            break;
                        }
                    }
                }
            }
            tempCounter = 0;
            strCnt = 0;
            memset(temp,'\0',10000);
        }
        else{
            temp[strCnt] = c;
            strCnt++;
        }
    }
    fclose(fp);
    return scene;
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
    
   sceneInfo->rays = calculateRays(sceneInfo);
    
   sceneInfo->intersectInfo = calculateIntersections(sceneInfo);
   init();
   glutReshapeFunc (reshape);
   glutDisplayFunc(display);
   glutKeyboardFunc (keyboard);
   glutMainLoop();
   return 0;
}
