
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

float planeX = 0;
float planeY = 0;
float planeZ = 0;


float viewX = 0;
float viewY = 0;
float viewZ = 0;
float viewWidth = 500;
float viewHeight = 500;

int rtX = 1024;
int rtY = 768;


//ambient light constants
float IaR = 1.0; //ambient light source
float IaG = 1.0;
float IaB = 1.0;
float Ka = 1.0; //ambient coefficient for objects


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

typedef struct RayInfo{
  int totalRays;
  int raysX = rtX;
  int raysY = rtY;
  Ray** rays;
}RayInfo;


typedef struct Ray{
  Point* origin;
  Point* direction;
}Ray;

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
    Bt = 2*(ray->direction->point->x * (ray->origin->point->x * spheres[i]->location->x) +
           ray->direction->point->y * (ray->origin->point->y * spheres[i]->location->y) +
           ray->direction->point->z * (ray->origin->point->z * spheres[i]->location->z));
    Ct = pow((ray->origin->point->x - spheres[i]->location->x),2) +
        pow((ray->origin->point->y - spheres[i]->location->y),2) +
        pow((ray->origin->point->z - spheres[i]->location->z),2) - pow(spheres[i]->radius,2);

    if((pow(Bt,2)-4*Ct) == 0) //no intersections
      continue;
    flag = 1;
      
    //determine the entry and exit t values using the quadratic formula
    Tot = -Bt - (sqrt(pow(Bt,2) - (4*Ct) )) / 2;
    Tft = -Bt + (sqrt(pow(Bt,2) - (4*Ct) )) / 2;

    //calculate the point of the entry in the world
    tempPoint->x = ray->origin->x + ray->direction->x*Tot;
    tempPoint->y = ray->origin->y + ray->direction->y*Tot;
    tempPoint->z = ray->origin->z + ray->direction->z*Tot;

    //check it is closer to the camera than the last intersection if it is, replace the A B C To Td values with the last calculated ones and continue
    tempDistance = calcDistance(tempPoint, ray->origin);
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
    for(int i = 0; i < scene->rays->totalRays; i++)
    {
        Intersection* inter = calculateIntersection(scene->spheres, scene->numSpheres, scene->rays[i]);
        if(inter != NULL){
            ii->intersections[intersectionNum] = inter;
            ii->numIntersections++;
            intersectionNum++;
        }
    }
    return ii;
}

Point* calculateReflectionVector (Point* normal, Point* lightVector)
{
    Point* r = (Point*)malloc(sizeof(Point));
    //float angle = 0;
    
    /*angle = acos(dotProduct(normal,lightVector) / (calculateLength(intersection->x, intersection->y, intersection->z, normal->x, normal->y, normal->z) * calculateLength(intersection->x, intersection->y, intersection->z, lightVector->x, lightVector->y, lightVector->z));*/
    
    float dot = dotProduct(lightVector, normal);
    
    r->x = lightVector->x - 2 * dot * normal->x;
    r->y = lightVector->y - 2 * dot * normal->y;
    r->y = lightVector->z - 2 * dot * normal->z;
    return r;
}


void illuminate(SceneInfo* scene)
{
    float r, g ,b;
    Point* diffuseLightVector = (Point*)malloc(sizeof(Point));
    Point* specularViewVector = (Point*)malloc(sizeof(Point));
    Point* specularReflectVector = NULL;
    
    
    
    for(int i = 0; i < scene->intersectInfo->numIntersections; i++){
        
        //calculate the vectors
        //diffuse
        normalize(scene->intersectInfo->intersections[i]->in->x, scene->intersectInfo->intersections[i]->in->y, scene->intersectInfo->intersections[i]->in->z,
                  sceneInfo->light->location->x, sceneInfo->light->location->y, sceneInfo->light->location->z,
                  &diffuseLightVector->x, &diffuseLightVector->y, &diffuseLightVector->z);
        
        //specular view
        normalize(scene->intersectInfo->intersections[i]->in->x, scene->intersectInfo->intersections[i]->in->y, scene->intersectInfo->intersections[i]->in->z,
                  camX, camY, camZ,
                  &specularViewVector->x, &specularViewVector->y, &specularViewVector->z);
        
        //specular reflection
        specularReflectVector = calculateReflectionVector(scene->intersectInfo->intersections[i]->normalIn, scene->light);
        
        //calculate r ambient, diffuse and specular combination colour
        r = IaR * Ka * scene->spheres[scene->intersectInfo->intersections[i]->sphereNum]->r +
            IpR * (Kd * scene->spheres[scene->intersectInfo->intersections[i]->sphereNum]->r * dotProduct(scene->intersectInfo->intersections[i]->normalIn, diffuseLightVector) +
                   Ks * scene->light->r * pow(dotProduct(specularReflectVector, specularViewVector), specN));
        
        //calculate g ambient, diffuse and
        g = IaG * Ka * scene->spheres[scene->intersectInfo->intersections[i]->sphereNum]->g +
            IpG * (Kd * scene->spheres[scene->intersectInfo->intersections[i]->sphereNum]->g * dotProduct(scene->intersectInfo->intersections[i]->normalIn, diffuseLightVector) +
                   Ks * scene->light->g * pow(dotProduct(specularReflectVector, specularViewVector), specN));

        
        //calculate b
        b = IaB * Ka * scene->spheres[scene->intersectInfo->intersections[i]->sphereNum]->b +
            IpB * (Kd * scene->spheres[scene->intersectInfo->intersections[i]->sphereNum]->b * dotProduct(scene->intersectInfo->intersections[i]->normalIn, diffuseLightVector) +
                   Ks * scene->light->b * pow(dotProduct(specularReflectVector, specularViewVector),specN));

        
        free(specularReflectVector);
        specularReflectVector = NULL;
        
        float pixel[3] = {r,g,b};
        
        //fill pixel with rgb value at desired location
        glDrawPixels(1, 1, GL_RGB, GL_UNSIGNED_BYTE, pixel);
                   
    }
    
    //free the temporary calculation vectors
    free(diffuseLightVector);
    free(specularViewVector);
    free(specularReflectVector);
    
    
}

void drawScene(SceneInfo* scene)
{
    illuminate(scene);
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

RayInfo* calculateRays(SceneInfo* scene)
{
    RayInfo* rays = (RayInfo*)malloc(sizeof(RayInfo));
    rays->totalRays = rtX*rtY;
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
            memset(temp,10000,'\0');
        }
        else{
            temp[strCnt] = c;
            strCnt++;
        }
    }
    strCnt = 0;
    memset(temp,10000,'\0');

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
                    
                    while((c = fgetc(fp))!= EOF){
                        if(c == '\n' || c == '\t' || c == ' '){
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
                            memset(temp,10000,'\0');
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
                    
                    while((c = fgetc(fp))!= EOF){
                        if(c == '\n' || c == '\t' || c == ' '){
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
                            memset(temp,10000,'\0');
                        }
                        else{
                            temp[strCnt] = c;
                            strCnt++;
                        }
                        if(tempCounter > 5){
                            tempCounter = 0;
                            sphereCounter++;
                            break;
                        }
                    }
                }
            }
            tempCounter = 0;
            strCnt = 0;
            memset(temp,10000,'\0');
        }
        else{
            temp[strCnt] = c;
            strCnt++;
        }
    }
    fclose(fp);

    return scene;
}


RayInfo* allocateRays(LightInfo* li)
{
  RayInfo* ri = (RayInfo*)malloc(sizeof(RayInfo));
  ri->totalRays = rtX*rtY;
  ri->rays = (Ray**)malloc(sizeof(Ray*)*ri->totalRays);

  for(int i = 0; i < ri->totalRays; i++){
    ri->rays[i] = (Ray*) malloc(sizeof(Ray));
    ri->rays[i]->location = (Point*) malloc(sizeof(Point));
    ri->rays[i]->direction = (Point*) malloc(sizeof(Point));
  }
    return ri;
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
