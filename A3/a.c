
/* Derived from scene.c in the The OpenGL Programming Guide */

/*
 Alex Hockley
 0758114
 March 10 2016
 CIS 4800 Assignment 3
 Cube Shadows
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

float lightX = 1.0;
float lightY = 1.0;
float lightZ = 1.0;

float cubeX = 0.5;
float cubeY = 0.5;
float cubeZ = 0.5;

float planeX = 0.0;
float planeY = 0.0;
float planeZ = 0.0;

float shadowX = 0.25;
float shadowY = 0.25;
float shadowZ = 0.25;

GLubyte  Image[64][64][4];
GLuint   textureID[1];

int oldMouseY = 0;
int mouseRightDown = 0;
int mouseLeftDown = 0;

struct typedef TexturePixel{
    int r;
    int g;
    int b;
}TexturePixel;

struct typedef TextureData{
    int width;
    int height;
    int depth;
    TexturePixel*** image;
} TextureData;


TextureData* textureData = NULL;

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
              cubeX, cubeY, cubeZ,
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
    
    glPopMatrix ();
    
    glPopMatrix ();
   
    glFlush ();
    
    
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
TextureData* loadTexture(char* fname) {
    
    FILE *fp;
    TextureData* td = NULL;
    char instr[10000];

    if ((fp = fopen(fname, "r")) == 0) {
        printf("Error, failed to find the file named %s.\n", fname);
        exit(0);
    }
    
    int lineNo = 0;
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
    
    int numCounter = 0;
    char tempStr[1024];
    int strCnt = 0;
    int inVal = 0;
    
    int rgbCounter = 0;
    int curRow = 0;
    int curCol = 0;
    
    memset(tempStr, '\0', 1024);
    
    int whdCounter = 0;
    
    td = (TextureData*)malloc(sizeof(TextureData));
    
    //start state based parsing
    while((c = fgetc(fp)))
    {
        if(c == '\n' && inVal == 0){ //if we hit newline, set skip to false
            lineSkip = 0;
            strCnt = 0;
            inVal = 0;
        }
        else if(c == '#' || lineSkip == 1){ //read until end of line
            lineSkip = 1;
        }
        else if((c == '\t' || c == ' ' || c == '\n' || c == EOF) && inVal == 1){ //end of number

            strCnt = 0;
            inVal = 0;
            
            //if we arent looking for width height depth
            if(whdCounter >= 3){
                numCounter++;
            
                //copy and clear
                if(rgbCounter==0)
                    sscanf(tempStr,"%d",&td->image[curRow][curCol]->r); //red value
                else if(rgbCounter==1)
                    sscanf(tempStr,"%d",&td->image[curRow][curCol]->g); //green value
                else if(rgbCounter==2)
                    sscanf(tempStr,"%d",&td->image[curRow][curCol]->b); //blue value
                
                rgbCounter++;
                
                if(rgbCounter>2){ //rgb set, go back to r and move to next pixel
                    rgbCounter=0;
                    curCol++;
                    if(curCol >= heightmap->width){
                        curCol = 0;
                        curRow++;
                    }
                }
                memset(tempStr, '\0', 1024);
            }
            else //hack for width height and depth, its not pretty
            {
                if(whdCounter == 0)
                    sscanf(tempStr,"%d",&td->width);
                if(whdCounter == 1)
                    sscanf(tempStr,"%d",&td->height);
                if(whdCounter == 2){
                    sscanf(tempStr,"%d",&td->depth);
                    td->image = (TexturePixel***)malloc(sizeof(TexturePixel**)*td->height); //allocate rows
                    for(int i = 0; i < td->height; i++)
                        td->image[i] = (TexturePixel**)malloc(sizeof(TexturePixel*)*td->width); //allocate cols
                    for(int i = 0; i < td->height; i++){
                        for(int j = 0; j < td->width; j++){
                            td->image[i][j] = (TexturePixel*)malloc(sizeof(TexturePixel)); //allocate each pixel
                        }
                    }
                }
                
                memset(tempStr, '\0', 1024);
                whdCounter++;
            }
            
        }
        
        else if(isdigit(c) != 0){ //if digit
            inVal = 1;
            tempStr[strCnt] = c; //add digit to string
            strCnt++; //increment string position counter
        }
        
        if(c == EOF)
            break;
    }
    fclose(fp);
    return td;
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
   textureData = loadMap(argv[1]);
   glutReshapeFunc (reshape);
   glutDisplayFunc(display);
   glutKeyboardFunc (keyboard);
    glutMouseFunc(mouseEvent);
    glutMotionFunc(mouseMotion);
   glutMainLoop();
   return 0; 
}

