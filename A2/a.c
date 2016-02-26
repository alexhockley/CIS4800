
/* Derived from scene.c in the The OpenGL Programming Guide */

#include <stdio.h>
#include <stdlib.h>
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
int thieght = 0;

GLubyte  Image[64][64][4];
GLuint   textureID[1];


typedef struct Map {
    int width;
    int height;
    int depth;
    int** vals;
} Map;

typedef struct Triangle {
    Point3D** points;
    Point3D* normalPoint;
} VertexNormal;

typedef struct Point3D{
    float x;
    float y;
    float z;
} Point3D;


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
    
    glPopMatrix ();
   glFlush ();
}

Triangle** generateTrianglePoints(Map map)
{
    int i=0, j=0;
    Point3D* point = NULL;
    Triangle* tri = NULL;
    int toggle = 0;
    int trow = 0;
    int tcol = 0;
    
    theight = (map->height-1);
    twidth = ((map->width*2)-2);
    
    Triangle*** triangles = (Triangle**)malloc(sizeof(Triangle**)* theight);
    for(int i = 0; i < map->height -1; i++)
    {
        triangles[i] = (Triangle**)malloc(sizeof(Triangle*)* twidth);
    }
    
    for (int i = 0; i < map->height-1; i++){
        for(int j = 0; j < map->width; j++){
            
            //init triangle memory
            tri = (Triangle*)malloc(sizeof(Triangle));
            tri->points = (Point3D**)malloc(sizeof(Point3D*) * 3);
            
            //get 3 points
            for(int n = 0; n < 3; n++){
                point = (Point3D*)malloc(sizeof(Point3D));
                if(toggle == 0){
                    if(j == map->width - 1){
                        free(point);//get rid of garbage point memory since we wont have reference later
                        break;
                    }
                    toggle = 1;
                    if(n == 0){
                        point->x = i;
                        point->z = j;
                        point->y = map->vals[i][j];
                    }
                    else if(n == 1){
                        point->x = i+1;
                        point->z = j;
                        point->y = map->vals[i+1][j];
                    }
                    else if(n == 2){
                        point->x = i;
                        point->z = j+1;
                        point->y = map->vals[i][j+1];
                    }
                }
                else{
                    toggle = 0;
                    if(n == 0){
                        point->x = i;
                        point->z = j;
                        point->y = map->vals[i][j];
                    }
                    else if(n == 1){
                        point->x = i+1;
                        point->y = j-1;
                        point->y = map->vals[i+1][j-1];
                    }
                    else if(n == 2){
                        point->x = i+1;
                        point->y = j;
                        point->y = map->vals[i+1][j];
                    }
                }
                
                if(toggle == 0 && j == (map->width - 1)){ //if we didnt build a triangle here, clean up memory and set to null and stop looking for points
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
                tcol++;
                if(tcol >= twidth)
                {
                    tcol = 0;
                    trow++;
                }
            }
            
            point = NULL;
            tri = NULL;
            toggle = 0;
            
        }
                
                
    }
    return triangles;
    
}

void calculateSurfaceNormals(Triangle*** triangles)
{
    Point3D* point = NULL;
    for(int i = 0; i < theight; i++){
        for(int j = 0; j < twidth; j++){
            point = (Point3D*)malloc(sizeof(Point3D));
            
            /*calculate cross product
             point->x = crossx;
             point->y = crossy;
             point->z = crossz;
             */
            
            triangles->normalPoint = point;
        }
    }
}

void calculateVertexNormals(Triangle*** triangles)
{
    Point3D* vertexNormal = NULL;
    //handles everything except the left and bottom border vertices
    for(int i = 0; i < theight; i++){
        for(int j = 0; j < twidth; j+=2){
            vertexNormal = (Point3D*)malloc(sizeof(Point3D));
            int numSurfaces = 0;
            float xTot = 0;
            float yTot = 0;
            float zTot = 0;
            
            if()
        }
    }
}

void drawTriangles(Triangle*** triangles, float heightmod)
{
    glBegin(GL_TRIANGLES);
    for(int i = 0; i < theight; i++){
        for(int j = 0; j < twidth; j++){
            for(int k = 0; k < 3; k++){
                glVertex3f(triangles[i][j]->points[k]->x,triangles[i][j]->points[k]->y * heightmod,triangles[i][j]->points[k]->z);
            }
        }
    }
    glEnd();
}

void freeTriangles(Triangle*** triangles)
{
    for(int i = 0; i < theight; i++){
        for(int j = 0; j < twidth; j++){
            for(int k = 0; k < 3; k++){
                free(triangles[i][j]->points[k]);
            }
            free(triangles[i][j]->points);
            free(triangles[i][j]->normalPoint);
            
        }
    }
    free(triangles);
    triangles = NULL;
}

void reshape(int w, int h)
{
   glViewport (0, 0, (GLsizei) w, (GLsizei) h);
   glMatrixMode (GL_PROJECTION);
   glLoadIdentity ();
   gluPerspective(45.0, (GLfloat)w/(GLfloat)h, 1.0, 10.0);
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

Map* loadMap(char* fname) {
    
    FILE *fp;
    char instr[10000];

    if ((fp = fopen(fname, "r")) == 0) {
        printf("Error, failed to find the file named %s.\n", fname);
        exit(0);
    }
    
    int lineNo = 0;
    Map* heightmap = (Map*)malloc(sizeof(Map));
    
    while(fgets(instr,10000,fp) != NULL){ //only do this for the first two real lines
        if(instr[0] == '#') //ignore comment
            continue;
        
        if(lineNo == 0){ //check if first line that is not a comment is the file format
            if(instr != NULL && strlen(instr) >= 2){
                if(instr[0] != 'P' || instr[1] != '2'){
                    printf("Wrong file format specified, must be P2");
                    exit(0);
                }
            }
            else{
                printf("Wrong file format specified, must be P2");
                exit(0);
            }
        }
        
        if(lineNo == 1){ //width height depth values
            sscanf(instr, "%d %d %d",
                   &heightmap->width,
                   &heightmap->height,
                   &heightmap->depth);
            int i = 0;
            heightmap->map = (int**)malloc(sizeof(int*)*heightmap->height);
            for(i = 0; i < heightmap->height; i++)
                heightmap->map[i] = (int*) malloc(sizeof(int)*heightmap->width);
            
            break;
        }
    }
    
    //start character-by-character parsing
    char c = '';
    int lineSkip = 0;
    int curRow = 0;
    int curCol = 0;
    int numCounter = 0;
    int tempStr[1024];
    int strCnt = 0;
    int inVal = 0;
    
    //potential issue: If EOF comes right after the last number with no whitespace
    while(c = fgetc(fp) != EOF)
    {
        if(c == '\n' && inVal == 0){ //if we hit newline, set skip to false
            lineSkip = 0;
            strCnt = 0;
            inVal = 0;
        }
        else if(c == '#' || lineSkip == 1){ //read until end of line
            lineSkip = 1;
        }
        else if((c == '\t' || c == ' ' || c == '\n') && inVal == 1){ //end of number
            strCnt = 0;
            inVal = 0;
            
            //copy and clear
            sscanf(tempStr,"%d",heightmap->map[curRow][curCol]);
            memset(tempStr, '\0', 1024);
            
            //next number counter
            curCol++;
            if(curCol >= heightmap->width){
                curCol = 0;
                curRow++;
            }
            
        }
        
        else if(isdigit(c) != 0){ //if digit
            tempStr[strCnt] = c; //add digit to string
            strCnt++; //increment string position counter
        }
        
    }
    
    fclose(fp);
    return heightmap;
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
   loadMap();
   glutReshapeFunc (reshape);
   glutDisplayFunc(display);
   glutKeyboardFunc (keyboard);
   glutMainLoop();
   return 0; 
}

