
/* Derived from scene.c in the The OpenGL Programming Guide */

/*
 Alex Hockley
 0758114
 March 10 2016
 CIS 4800 Assignment 3
 Cube Shadows and textures
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

//location data for the light
float lightX = 0.0;
float lightY = 0.5;
float lightZ = 0.0;
float angle = 0.0;

//location data for the static cube
float cubeX = 0.0;
float cubeY = 0.5;
float cubeZ = 0.0;

//location data for the static plane
float planeX = 0.0;
float planeY = 0.0;
float planeZ = 0.0;

float planeScale = 1.0f;


int oldMouseY = 0;
int mouseRightDown = 0;
int mouseLeftDown = 0;

typedef struct TexturePixel{
    int r;
    int g;
    int b;
}TexturePixel;

typedef struct TextureData{
    int width;
    int height;
    int depth;
    TexturePixel*** image;
}TextureData;

int texCounter = 0;

TextureData** textureDatas = NULL;

GLubyte** textureImage = NULL;
GLuint* textureID = NULL;

//load the texture data from file into the texture values, this could have been done a lot cleaner but yolo
void loadTextureDataGlobal(TextureData** td, int count)
{
    
    if(td == NULL)
        printf("Provided texture data was empty\n");
    if(textureImage != NULL){
        free(textureImage);
        textureImage = NULL;
    }
    
    textureImage = (GLubyte**)malloc(sizeof(GLubyte*)*count);
    
    for(int i =0; i < count; i++)
        textureImage[i] = (GLubyte*) malloc(sizeof(GLubyte)*(td[i]->width*td[i]->height));
    
    textureID = (GLuint*)malloc(sizeof(GLuint)*count);
    
    for(int curTex = 0; curTex < count; curTex++){
        int cnt = 0;
        for(int i = 0; i < td[curTex]->height; i++){
            for(int j = 0; j < td[curTex]->width; j++){
                textureImage[curTex][cnt] = (GLubyte)td[curTex]->image[i][j]->r;
                cnt++;
                textureImage[curTex][cnt] = (GLubyte)td[curTex]->image[i][j]->g;
                cnt++;
                textureImage[curTex][cnt] = (GLubyte)td[curTex]->image[i][j]->b;
                cnt++;
                textureImage[curTex][cnt] = (GLubyte)255;
                cnt++;
            }
        }
    }
    
    
    //glPixelStorei(GL_UNPACK_ALIGNMENT,1);
    glGenTextures(count,textureID);
    
    for(int i = 0; i < count; i++){
        glBindTexture(GL_TEXTURE_2D, textureID[i]);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        
        glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,
                     td[i]->width,td[i]->height,0,
                     GL_RGBA, GL_UNSIGNED_BYTE, textureImage[i]);
        
        
    }
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glEnable(GL_TEXTURE_GEN_S);
    glEnable(GL_TEXTURE_GEN_T);
}

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

void display (void)
{
    GLfloat blue[]  = {0.0, 0.0, 1.0, 1.0};
    GLfloat red[]   = {1.0, 0.0, 0.0, 1.0};
    GLfloat green[] = {0.0, 1.0, 0.0, 1.0};
    GLfloat white[] = {1.0, 1.0, 1.0, 1.0};
    GLfloat gray[] = {0.5, 0.5, 0.5, 1.0};
    
    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    glShadeModel(GL_SMOOTH);
    
    /* draw polygons as either solid or outlines */
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    
    /* set starting location of objects */
    glPushMatrix ();
    gluLookAt(camX, camY, camZ,
              cubeX, cubeY, cubeZ,
              0, 1, 0);
    
    /* give all objects the same shininess value */
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 100.0);
    
    
    
    /* if textured, then use white as base colour */
    glDisable(GL_LIGHT0);
    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, gray);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, white);

    glPushMatrix ();
    
    //draw the plane
    glBegin(GL_QUADS);
    glVertex3f(1.0*planeScale, 0*planeScale, 1.0*planeScale);
    glVertex3f(1.0*planeScale, 0.0*planeScale, -1.0*planeScale);
    glVertex3f(-1.0*planeScale, 0.0*planeScale, -1.0*planeScale);
    glVertex3f(-1.0*planeScale, 0.0*planeScale, 1.0*planeScale);
    glEnd();
    glPopMatrix ();
    glEnable(GL_LIGHT0);
    
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, white);
    //glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, white);
    
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, textureID[texCounter]);
    
        //draw the cube
    glPushMatrix();
    glTranslatef(cubeX,cubeY,cubeZ);
    glutSolidCube(0.2);
    glPopMatrix();
    
    glDisable(GL_TEXTURE_2D);
    
    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, red);
    glPushMatrix();
    glTranslatef(lightX, lightY, lightZ);
    glutSolidSphere(0.05, 10, 10);
    glPopMatrix();
    
    glDisable(GL_LIGHT0);
    
    //draw the shadow, it looks a bit messed up, i think its on a 45 degree angle or something
    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, white);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, white);
    glPushMatrix();
    GLfloat shadowMat[16] = {
        lightY,-lightX,0,0,
        0,0,0,0,
        0, -lightZ, lightY ,0,
        0,0,0,lightY
    };
    glMultMatrixf(shadowMat);
    glutSolidCube(0.2);
    glPopMatrix();
    glEnable(GL_LIGHT0);
    
    glPopMatrix();
    glFlush ();
    glDisable(GL_TEXTURE_2D);
    
}

//using math and stuff, calculate the light position based on angle in radians
void calculateLightPosition(float angle)
{
    lightZ = sin(angle);
    lightX = cos(angle);
}

//idle function, runs every tick
void tickFunction()
{
    angle += 0.02f;
    if(angle > M_PI*2)
        angle = 0;
    calculateLightPosition(angle);
    init();
    display();
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
       case 49:
           texCounter++;
           if(texCounter>=7)
               texCounter = 0;
           init();
           display();
           break;
   }
}


//reads the map from file
TextureData* loadTexture(char* fname) {
    printf("Loading texture\n");
    FILE *fp;
    char instr[10000];

    if ((fp = fopen(fname, "r")) == 0) {
        printf("Error, failed to find the file named %s.\n", fname);
        exit(0);
    }
    TextureData* td = NULL;
    int lineNo = 0;
    while(fgets(instr,10000,fp) != NULL){ //only do this for the first real line
        if(instr[0] == '#') //ignore comment
            continue;
        if(strlen(instr) >= 2){
            if(instr[0] != 'P' || instr[1] != '3'){
                printf("Wrong file format specified, must be P3");
                exit(0);
            }
        }
        else{
            printf("Wrong file format specified, must be P3");
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
                    if(curCol >= td->width){
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
  
    //load the texture data
   textureDatas = (TextureData**)malloc(sizeof(TextureData*)*7);
   textureDatas[0] = loadTexture("brick.ppm");
   textureDatas[1] = loadTexture("horrible.ppm");
   textureDatas[2] = loadTexture("moon.ppm");
   textureDatas[3] = loadTexture("mud.ppm");
   textureDatas[4] = loadTexture("psych.ppm");
   textureDatas[5] = loadTexture("spots.ppm");
   textureDatas[6] = loadTexture("wood.ppm");
    
   loadTextureDataGlobal(textureDatas, 7);
    
   init();
   glutReshapeFunc (reshape);
   glutDisplayFunc(display);
   glutIdleFunc(tickFunction);
   glutKeyboardFunc (keyboard);
   glutMainLoop();
   return 0; 
}

