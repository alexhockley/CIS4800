
/* Derived from scene.c in the The OpenGL Programming Guide */

/*
 Alex Hockley
 0758114
 Feb 29 2016
 CIS 4800 Assignment 2
 Draws a heightmap using OpenGL and uses mouse events to scale/zoom
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

	/* flags used to control the appearance of the image */
int lineDrawing = 1;	// draw polygons as solid or lines
int lighting = 0;	// use diffuse and specular lighting
int smoothShading = 0;  // smooth or flat shading
int textures = 0;
int twidth = 0;
int theight = 0;
float maxVal = 0;
float zoomfactor = 1.0f;

int winWidth = 0;
int winHeight = 0;

float camX = 2;
float camY = 2;
float camZ = 2;

GLubyte  Image[64][64][4];
GLuint   textureID[1];

int oldMouseY = 0;
int mouseRightDown = 0;
int mouseLeftDown = 0;


typedef struct Map {
    int width;
    int height;
    int depth;
    int** vals;
} Map;

typedef struct Point3D{
    float x;
    float y;
    float z;
} Point3D;

typedef struct Triangle {
    Point3D** points;
    Point3D* normalPoint;
} Triangle;



Point3D** vertexNormals = NULL;
Triangle*** globTriangles = NULL;
Map* heightMap = NULL;

void drawTriangles(Triangle***);

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
}

void display (void)
{
    GLfloat blue[]  = {0.0, 0.0, 1.0, 1.0};
    GLfloat red[]   = {1.0, 0.0, 0.0, 1.0};
    GLfloat green[] = {0.0, 1.0, 0.0, 1.0};
    GLfloat white[] = {1.0, 1.0, 1.0, 1.0};
    GLfloat gray[] = {0.5, 0.5, 0.5, 1.0};
    
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
    
    /* set starting location of objects */
    glPushMatrix ();
    gluLookAt(camX, camY, camZ,
              0.5, 0.5, 0.5,
              0, 1, 0);
    
    /* give all objects the same shininess value */
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 30.0);
    
    /* set colour of cone */
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, red);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, white);
    /* move to location for object then draw it */
    glPushMatrix ();
    glTranslatef (-0.75, -0.5, 0.0);
    glRotatef (270.0, 1.0, 0.0, 0.0);
    //glutSolidCone (1.0, 2.0, 15, 15);
    glPopMatrix ();
    
    /* set colour of sphere */
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, green);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, white);
    /* move to location for object then draw it */
    glPushMatrix ();
    glTranslatef (0.75, 0.0, -1.0);
    
    /* turn texturing on */
    if (textures == 1) {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, textureID[0]);
        /* if textured, then use white as base colour */
        glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, white);
    }
    
    //glutSolidSphere (1.0, 15, 15);
    
    if (textures == 1)
        glDisable(GL_TEXTURE_2D);
    glPopMatrix ();
    
    /* set colour of torus */
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, gray);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, white);
    /* move to location for object then draw it */
    glPushMatrix ();
    glTranslatef (-0.75, 0.5, 0.0);
    glRotatef (90.0, 1.0, 0.0, 0.0);
    //glutSolidTorus (0.275, 0.85, 15, 15);
    
    glPopMatrix ();
    drawTriangles(globTriangles);
    //gluLookAt(0,0,0,  0.5,0.5,0.5, 0,0,0);
    
    glPopMatrix ();
   
    glFlush ();
    
    
}

//generate the triangle points
Triangle*** generateTrianglePoints(Map* map)
{
    int i=0, j=0;
    Point3D* point = NULL;
    Triangle* tri = NULL;
    int toggle = 0;
    int trow = 0;
    int tcol = 0;
    int cnt = 0;
    int nopoint = 0;
    theight = (map->height)-1;
    twidth = ((map->width*2)-2);
    
    printf("total %d\n", theight * twidth);
    
    printf("Generating triangle points\n");
    printf("theight: %d\n", theight);
    printf("twidth: %d\n", twidth);
    
    Triangle*** triangles = (Triangle***)malloc(sizeof(Triangle**)* theight);
    for(int i = 0; i < map->height; i++)
    {
        triangles[i] = (Triangle**)malloc(sizeof(Triangle*)* twidth);
    }
    for (int i = 0; i < map->height-1; i++){
        for(int j = 0; j < map->width; j++){
            
            //init triangle memory
            tri = (Triangle*)malloc(sizeof(Triangle));
            tri->points = (Point3D**)malloc(sizeof(Point3D*) * 3);
            tri->normalPoint = NULL;
            
            
            //get 3 points
            for(int n = 0; n < 3; n++){
                point = (Point3D*)malloc(sizeof(Point3D));
                
                if(toggle == 0){
                    
                    if(n == 0){
                        point->x = i/(float)map->height;
                        point->z = j/(float)map->width;
                        point->y = map->vals[i][j]/maxVal;
                    }
                    else if(n == 1 && i+1 < map->height){
                        point->x = (i+1)/(float)map->height;
                        point->z = j/(float)map->width;
                        point->y = map->vals[i+1][j]/maxVal;
                    }
                    else if(n == 2 && j+1 < map->width){
                        point->x = i/(float)map->height;
                        point->z = (j+1)/(float)map->width;
                        point->y = map->vals[i][j+1]/maxVal;
                    }
                    else{
                        nopoint++;
                        free(point);
                        point = NULL;
                    }
                }
                else{
                    
                    if(n == 0){
                        point->x = i/(float)map->height;
                        point->z = j/(float)map->width;
                        point->y = map->vals[i][j]/maxVal;
                    }
                    else if(n == 1 && i+1 < map->height && j-1 >= 0){
                        point->x = (i+1)/(float)map->height;
                        point->z = (j-1)/(float)map->width;
                        point->y = map->vals[i+1][j-1]/maxVal;
                    }
                    else if(n == 2 && i+1 < map->height){
                        point->x = (i+1)/(float)map->height;
                        point->z = j/(float)map->width;
                        point->y = map->vals[i+1][j]/maxVal;
                    }
                    else{
                        nopoint++;
                        free(point);
                        point = NULL;
                    }
                }
                
                if(point == NULL){ //if we didnt build a triangle here, clean up memory and set to null and stop looking for points
                    free(tri->points);
                    free(tri);
                    tri = NULL;
                    break;
                }
                
                tri->points[n] = point;
                point = NULL;
                
                
            }
            
            
            if(tri != NULL)
            {
                triangles[trow][tcol] = tri;
                cnt++;
                tcol++;
                if(tcol >= twidth)
                {
                    tcol = 0;
                    trow++;
                }
            }
            
            
            
            point = NULL;
            tri = NULL;
            
            if(toggle == 1){
                toggle = 0;
                j--;
            }
            else if(toggle == 0)
                toggle = 1;
        }
        
                
    }
    printf("Done generating triangles\n");
    printf("Generated %d triangles\n", cnt);
    printf("nopoint %d \n", nopoint);
    return triangles;
    
}

//calculate the surface normals and store them in the triangle
void calculateSurfaceNormals(Triangle*** triangles)
{
    printf("Calculating surface normals\n");
    Point3D* point = NULL;
    for(int i = 0; i < theight; i++){
        for(int j = 0; j < twidth; j++){
            if(triangles[i][j]->normalPoint == NULL)
                point = (Point3D*)malloc(sizeof(Point3D));
            else
                point = triangles[i][j]->normalPoint;
            
            float Ux = triangles[i][j]->points[1]->x - triangles[i][j]->points[0]->x;
            float Uy = triangles[i][j]->points[1]->y - triangles[i][j]->points[0]->y;
            float Uz = triangles[i][j]->points[1]->z - triangles[i][j]->points[0]->z;
            
            float Vx = triangles[i][j]->points[2]->x - triangles[i][j]->points[0]->x;
            float Vy = triangles[i][j]->points[2]->x - triangles[i][j]->points[0]->x;
            float Vz = triangles[i][j]->points[2]->x - triangles[i][j]->points[0]->x;
            
            point->x = (Uy*Vz) - (Uz*Vy);
            point->y = (Uz*Vx) - (Ux*Vz);
            point->z = (Ux*Vy) - (Uy*Vx);
            
            
            triangles[i][j]->normalPoint = point;
        }
    }
}


//calculate the vertex normals and store them in an array
void calculateVertexNormals(Triangle*** triangles)
{
    printf("Calculating vertex normals\n");
    Point3D* vertexNormal = NULL;
    Point3D** vertexNormals = (Point3D**)malloc(sizeof(Point3D*)*(twidth*theight));
    int normalCounter = 0;
    float xTot = 0.0f;
    float yTot = 0.0f;
    float zTot = 0.0f;
    int numSurfaces = 0;
    
    //handles everything except the left and bottom border vertices
    for(int i = 0; i < theight; i++)
    {
        for(int j = 0; j < twidth; j+=2)
        {
            vertexNormal = (Point3D*)malloc(sizeof(Point3D));
            
            xTot = 0.0f;
            yTot = 0.0f;
            zTot = 0.0f;
            
            //this index always exists (cur row and cur col)
            xTot += triangles[i][j]->normalPoint->x;
            yTot += triangles[i][j]->normalPoint->y;
            zTot += triangles[i][j]->normalPoint->z;
            numSurfaces++;
            
            if(! (j-1 < 0)){ //currow -1col
                xTot += triangles[i][j-1]->normalPoint->x;
                yTot += triangles[i][j-1]->normalPoint->y;
                zTot += triangles[i][j-1]->normalPoint->z;
                numSurfaces++;
            }
            if(! (j-2 < 0)){ //currow -2 col
                xTot += triangles[i][j-2]->normalPoint->x;
                yTot += triangles[i][j-2]->normalPoint->y;
                zTot += triangles[i][j-2]->normalPoint->z;
                numSurfaces++;
            }
            if(! (i-1 < 0)){ //-1 row curcol
                if(! (j+1 >= twidth)){ //-1 row +1 col
                    xTot += triangles[i-1][j+1]->normalPoint->x;
                    yTot += triangles[i-1][j+1]->normalPoint->y;
                    zTot += triangles[i-1][j+1]->normalPoint->z;
                    numSurfaces++;
                }
                if(! (j-1 < 0)){ //-1 row -1col
                    xTot += triangles[i-1][j-1]->normalPoint->x;
                    yTot += triangles[i-1][j-1]->normalPoint->y;
                    zTot += triangles[i-1][j-1]->normalPoint->z;
                    numSurfaces++;
                }
                xTot += triangles[i-1][j]->normalPoint->x;
                yTot += triangles[i-1][j]->normalPoint->y;
                zTot += triangles[i-1][j]->normalPoint->z;
                numSurfaces++;
            }
            vertexNormal->x = xTot / numSurfaces;
            vertexNormal->y = yTot / numSurfaces;
            vertexNormal->z = zTot / numSurfaces;
            
            vertexNormals[normalCounter] = vertexNormal;
            vertexNormal = NULL;
            normalCounter++;
        }
    }
               
    //now we need to calculate for the special border cases
    //right column border
    for(int i = 0; i < theight; i++){
        xTot = 0.0f;
        yTot = 0.0f;
        zTot = 0.0f;
        vertexNormal = (Point3D*)malloc(sizeof(Point3D));
        
        numSurfaces = 0;
        
        if(twidth-1 > 0){ //always do last triangle
            xTot += triangles[i][twidth-1]->normalPoint->x;
            yTot += triangles[i][twidth-1]->normalPoint->y;
            zTot += triangles[i][twidth-1]->normalPoint->z;
            numSurfaces++;
        }
        
        if(twidth-2 > 0 && i < (theight - 1)){ //do one to the left if we arent on the bottom right point
            xTot += triangles[i][twidth-2]->normalPoint->x;
            yTot += triangles[i][twidth-2]->normalPoint->y;
            zTot += triangles[i][twidth-2]->normalPoint->z;
            numSurfaces++;
        }
        
        
        if(i > 0 && i < (theight-1)){ //not top left point and not bottom right point
            //do one triangle above as well
            if((i - 1) >= 0)
            {
                xTot += triangles[i-1][twidth-1]->normalPoint->x;
                yTot += triangles[i-1][twidth-1]->normalPoint->y;
                zTot += triangles[i-1][twidth-1]->normalPoint->z;
                numSurfaces++;
            }
        }
        
        vertexNormal->x = xTot / numSurfaces;
        vertexNormal->y = yTot / numSurfaces;
        vertexNormal->z = zTot / numSurfaces;
        
        vertexNormals[normalCounter] = vertexNormal;
        normalCounter++;
        vertexNormal = NULL;
    }
    
    //bottom row border, except the final one which was already calculated
    for(int i = 0; i < twidth-1; i+=2){
        xTot = 0.0f;
        yTot = 0.0f;
        zTot = 0.0f;
        vertexNormal = (Point3D*)malloc(sizeof(Point3D));
        numSurfaces = 0;
        
        //current triangle
        xTot += triangles[theight-1][i]->normalPoint->x;
        yTot += triangles[theight-1][i]->normalPoint->y;
        zTot += triangles[theight-1][i]->normalPoint->z;
        numSurfaces++;
        
        if(i + 1 < twidth-1){ //if there is a triangle to the right
            xTot += triangles[theight-1][i+1]->normalPoint->x;
            yTot += triangles[theight-1][i+1]->normalPoint->y;
            zTot += triangles[theight-1][i+1]->normalPoint->z;
            numSurfaces++;
        }
        
        if(i - 1 > 0 && i < twidth - 1){ //if there is a triangle to the left and we're not on the last one
            xTot += triangles[theight-1][i-1]->normalPoint->x;
            yTot += triangles[theight-1][i-1]->normalPoint->y;
            zTot += triangles[theight-1][i-1]->normalPoint->z;
            numSurfaces++;
        }
        
        
        vertexNormal->x = xTot / numSurfaces;
        vertexNormal->y = yTot / numSurfaces;
        vertexNormal->z = zTot / numSurfaces;
        vertexNormals[normalCounter] = vertexNormal;
        normalCounter++;
    }
    
}

//draw the triangles and assignm the normals (for some reason it is not shading properly
void drawTriangles(Triangle*** triangles)
{
    printf("Drawing triangles\n");
    glBegin(GL_TRIANGLES);
    for(int i = 0; i < theight; i++){
        for(int j = 0; j < twidth; j++){
            for(int k = 0; k < 3; k++){
                glVertex3f(triangles[i][j]->points[k]->x,triangles[i][j]->points[k]->y,triangles[i][j]->points[k]->z);
            }
            glNormal3f(triangles[i][j]->normalPoint->x, triangles[i][j]->normalPoint->y, triangles[i][j]->normalPoint->z);
        }
    }
    glEnd();
}

void freeTriangles(Triangle*** triangles)
{
    for(int i = 0; i < theight; i++){
        for(int j = 0; j < twidth; j++){
            for(int k = 0; k < 3; k++){
                if(triangles[i][j]->points[k] != NULL){
                    free(triangles[i][j]->points[k]);
                    triangles[i][j]->points[k] = NULL;
                }
                
            }
            if(triangles[i][j]->points != NULL){
                free(triangles[i][j]->points);
                triangles[i][j]->points = NULL;
            }
            
            if(triangles[i][j]->normalPoint != NULL){
                free(triangles[i][j]->normalPoint);
                triangles[i][j]->normalPoint = NULL;
            }
            
        }
    }
    
    if(triangles != NULL)
        free(triangles);
    triangles = NULL;
}

//scale the triangles up and down with the height mod value
void scaleTriangles(Triangle*** triangles, float heightmod)
{
    for(int i = 0; i < theight; i++){
        for(int j = 0; j < twidth; j++){
            for(int k = 0; k < 3; k++){
                triangles[i][j]->points[k]->y = triangles[i][j]->points[k]->y * heightmod;
            }
        }
    }
}

//update the triangle values and normals
void updateTriangles(Triangle*** triangles, float heightmod)
{
    scaleTriangles(triangles,heightmod);
    
    if(vertexNormals != NULL){
        free(vertexNormals);
        vertexNormals = NULL;
    }
    
    calculateSurfaceNormals(triangles);
    
    calculateVertexNormals(triangles);
    
}


//reshape the window
void reshape(int w, int h)
{
   glViewport (0, 0, (GLsizei) w, (GLsizei) h);
   glMatrixMode (GL_PROJECTION);
   glLoadIdentity ();
   gluPerspective(45.0*zoomfactor, (GLfloat)w/(GLfloat)h, 1.0, 10.0);
   glMatrixMode (GL_MODELVIEW);
   glLoadIdentity ();
    winWidth = w;
    winHeight = h;
}

//zoom in on the model
void zoom()
{
    glMatrixMode (GL_PROJECTION);
    glLoadIdentity ();
    gluPerspective(45.0*zoomfactor, (GLfloat)winWidth/(GLfloat)winHeight, 1.0, 10.0);
    glMatrixMode (GL_MODELVIEW);
    glLoadIdentity ();
}


//keyboard input event
void keyboard(unsigned char key, int x, int y)
{
   switch (key) {
      case 27:
      case 'q':
           freeTriangles(globTriangles);
         exit(0);
         break;
      case '1':		// draw polygons as outlines
         lineDrawing = 1;
         lighting = 0;
         smoothShading = 0;
         textures = 0;
         init();
         display();
         break;
      case '2':		// draw polygons as filled
         lineDrawing = 0;
         lighting = 0;
         smoothShading = 0;
         textures = 0;
         init();
         display();
         break;
      case '3':		// diffuse and specular lighting, flat shading
         lineDrawing = 0;
         lighting = 1;
         smoothShading = 0;
         textures = 0;
         init();
         display();
         break;
      case '4':		// diffuse and specular lighting, smooth shading
         lineDrawing = 0;
         lighting = 1;
         smoothShading = 1;
         textures = 0;
         init();
         display();
         break;
      case '5':		// texture with  smooth shading
         lineDrawing = 0;
         lighting = 1;
         smoothShading = 1;
         textures = 1;
         init();
         display();
         break;
           
   }
}


//reads the map from file
Map* loadMap(char* fname) {
    
    FILE *fp;
    char instr[10000];

    if ((fp = fopen(fname, "r")) == 0) {
        printf("Error, failed to find the file named %s.\n", fname);
        exit(0);
    }
    
    int lineNo = 0;
    Map* heightmap = (Map*)malloc(sizeof(Map));
    
    while(fgets(instr,10000,fp) != NULL){ //only do this for the first real line
        if(instr[0] == '#') //ignore comment
            continue;
        if(strlen(instr) >= 2){
            if(instr[0] != 'P' || instr[1] != '2'){
                printf("Wrong file format specified, must be P2");
                exit(0);
            }
        }
        else{
            printf("Wrong file format specified, must be P2");
            exit(0);
        }
        break;
        
    }
    //start character-by-character parsing
    char c;
    int lineSkip = 0;
    int curRow = 0;
    int curCol = 0;
    int numCounter = 0;
    char tempStr[1024];
    int strCnt = 0;
    int inVal = 0;
    
    memset(tempStr, '\0', 1024);
    
    int whdCounter = 0;
    
    //start state based parsing
    while((c = fgetc(fp)))
    {
        if(c == '\n' && inVal == 0){ //if we hit newline, set skip to false
            lineSkip = 0;
            strCnt = 0;
            inVal = 0;
            //printf("Hit newline, skip to false\n");
        }
        else if(c == '#' || lineSkip == 1){ //read until end of line
            lineSkip = 1;
            //printf("Reading until end of line\n");
        }
        else if((c == '\t' || c == ' ' || c == '\n' || c == EOF) && inVal == 1){ //end of number
            //printf("Found end of number\n");
            strCnt = 0;
            inVal = 0;
            printf("%s\n", tempStr);
            
            //if we arent looking for width height depth
            if(whdCounter >= 3){
                numCounter++;
            
                //copy and clear
                sscanf(tempStr,"%d",&heightmap->vals[curRow][curCol]);
                memset(tempStr, '\0', 1024);
            
            
                if(heightmap->vals[curRow][curCol] > maxVal)
                    maxVal =heightmap->vals[curRow][curCol];
                //next number counter
                curCol++;
                if(curCol >= heightmap->width){
                    curCol = 0;
                    curRow++;
                }
            }
            else //hack for width height and depth, its not pretty
            {
                if(whdCounter == 0)
                    sscanf(tempStr,"%d",&heightmap->width);
                if(whdCounter == 1)
                    sscanf(tempStr,"%d",&heightmap->height);
                if(whdCounter == 2){
                    sscanf(tempStr,"%d",&heightmap->depth);
                    heightmap->vals = (int**)malloc(sizeof(int*)*heightmap->height);
                    for(int i = 0; i < heightmap->height; i++)
                        heightmap->vals[i] = (int*) malloc(sizeof(int)*heightmap->width);
                    
                }
                
                memset(tempStr, '\0', 1024);
                whdCounter++;
            }
            
        }
        
        else if(isdigit(c) != 0){ //if digit
            //printf("Found a number\n");
            inVal = 1;
            tempStr[strCnt] = c; //add digit to string
            strCnt++; //increment string position counter
        }
        
        if(c == EOF)
            break;
    }
    fclose(fp);
    return heightmap;
}

//mouse event function
void mouseEvent(int button, int state, int x, int y)
{
    switch(button){
        case GLUT_RIGHT_BUTTON:
            if(state == GLUT_DOWN){
                oldMouseY = y;
                mouseRightDown = 1;
            }
            else
                mouseRightDown = 0;
            break;
        case GLUT_LEFT_BUTTON:
            if(state == GLUT_DOWN){
                oldMouseY = y;
                mouseLeftDown = 1;
            }
            else
                mouseLeftDown = 0;
            break;
    }
    
}

//mouse motion function
void mouseMotion(int x, int y)
{
    int updateHappened = 0;
    if(mouseRightDown == 1)
    {
        if(y < oldMouseY){
            oldMouseY = y;
            updateTriangles(globTriangles, 1.01f);
            updateHappened = 1;
        }
        else if( y > oldMouseY){
            oldMouseY = y;
            updateTriangles(globTriangles, 0.99f);
            updateHappened = 1;
        }
    }
    if(mouseLeftDown == 1)
    {
        if(y < oldMouseY){
            oldMouseY = y;
            zoomfactor -= 0.05f;
            zoom();
            updateHappened = 1;
        }
        else if( y > oldMouseY){
            oldMouseY = y;
            zoomfactor += 0.05f;
            zoom();
            updateHappened = 1;
        }

    }
    if(updateHappened == 1)
    {
        init();
        display();
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
   init();
   heightMap = loadMap(argv[1]);
   globTriangles = generateTrianglePoints(heightMap);
    updateTriangles(globTriangles,1);
   glutReshapeFunc (reshape);
   glutDisplayFunc(display);
   glutKeyboardFunc (keyboard);
    glutMouseFunc(mouseEvent);
    glutMotionFunc(mouseMotion);
   glutMainLoop();
   return 0; 
}

