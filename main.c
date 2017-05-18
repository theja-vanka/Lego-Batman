#include <GL/glut.h>    // Header File For The GLUT Library
#include <GL/gl.h>	// Header File For The OpenGL32 Library
#include <GL/glu.h>	// Header File For The GLu32 Library
#include <unistd.h>     // Header file for sleeping.
#include <stdio.h>      // Header file for standard file i/o.
#include <stdlib.h>     // Header file for malloc/free.

/* ascii code for the escape key */
#define ESCAPE 27

/* The number of our GLUT window */
int window;
GLfloat     rot;                       // Angle For The Triangle ( NEW )
GLfloat     zoom = -20.0f;
/* storage for one texture  */
int texture[1];

struct Image {
    unsigned long sizeX;
    unsigned long sizeY;
    char *data;
};
typedef struct Image Image;

int ImageLoad(char *filename, Image *image) {
    FILE *file;
    unsigned long size;                 // size of the image in bytes.
    unsigned long i;                    // standard counter.
    unsigned short int planes;          // number of planes in image (must be 1)
    unsigned short int bpp;             // number of bits per pixel (must be 24)
    char temp;                          // temporary color storage for bgr-rgb conversion.

    // make sure the file is there.
    if ((file = fopen(filename, "rb"))==NULL)
    {
	printf("File Not Found : %s\n",filename);
	return 0;
    }

    // seek through the bmp header, up to the width/height:
    fseek(file, 18, SEEK_CUR);

    // read the width
    if ((i = fread(&image->sizeX, 4, 1, file)) != 1) {
	printf("Error reading width from %s.\n", filename);
	return 0;
    }
    printf("Width of %s: %lu\n", filename, image->sizeX);

    // read the height
    if ((i = fread(&image->sizeY, 4, 1, file)) != 1) {
	printf("Error reading height from %s.\n", filename);
	return 0;
    }
    printf("Height of %s: %lu\n", filename, image->sizeY);

    // calculate the size (assuming 24 bits or 3 bytes per pixel).
    size = image->sizeX * image->sizeY * 3;

    // read the planes
    if ((fread(&planes, 2, 1, file)) != 1) {
	printf("Error reading planes from %s.\n", filename);
	return 0;
    }
    if (planes != 1) {
	printf("Planes from %s is not 1: %u\n", filename, planes);
	return 0;
    }

    // read the bpp
    if ((i = fread(&bpp, 2, 1, file)) != 1) {
	printf("Error reading bpp from %s.\n", filename);
	return 0;
    }
    if (bpp != 24) {
	printf("Bpp from %s is not 24: %u\n", filename, bpp);
	return 0;
    }

    // seek past the rest of the bitmap header.
    fseek(file, 24, SEEK_CUR);

    // read the data.
    image->data = (char *) malloc(size);
    if (image->data == NULL) {
	printf("Error allocating memory for color-corrected image data");
	return 0;
    }

    if ((i = fread(image->data, size, 1, file)) != 1) {
	printf("Error reading image data from %s.\n", filename);
	return 0;
    }

    for (i=0;i<size;i+=3) { // reverse all of the colors. (bgr -> rgb)
	temp = image->data[i];
	image->data[i] = image->data[i+2];
	image->data[i+2] = temp;
    }

    // we're done.
    return 1;
}

// Load Bitmaps And Convert To Textures
void LoadGLTextures() {
    // Load Texture
    Image *image1;

    // allocate space for texture
    image1 = (Image *) malloc(sizeof(Image));
    if (image1 == NULL) {
	printf("Error allocating space for image");
	exit(0);
    }

    if (!ImageLoad("/home/vanka/Documents/Lego-Batman/media/logo.bmp", image1)) {
	exit(1);
    }

    // Create Texture
    glGenTextures(1, &texture[0]);
    glBindTexture(GL_TEXTURE_2D, texture[0]);   // 2d texture (x and y size)

    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR); // scale linearly when image bigger than texture
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR); // scale linearly when image smalled than texture

    // 2d texture, level of detail 0 (normal), 3 components (red, green, blue), x size from image, y size from image,
    // border 0 (normal), rgb color data, unsigned byte data, and finally the data itself.
    glTexImage2D(GL_TEXTURE_2D, 0, 3, image1->sizeX, image1->sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE, image1->data);
};
/* A general OpenGL initialization function.  Sets all of the initial parameters. */
void InitGL(int Width, int Height)	        // We call this right after our OpenGL window is created.
{
  LoadGLTextures();				// Load The Texture(s)
  glEnable(GL_TEXTURE_2D);			// Enable Texture Mapping
  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);		// This Will Clear The Background Color To Black
  glClearDepth(1.0);				// Enables Clearing Of The Depth Buffer
  glDepthFunc(GL_LESS);				// The Type Of Depth Test To Do
  glEnable(GL_DEPTH_TEST);			// Enables Depth Testing
  glShadeModel(GL_SMOOTH);			// Enables Smooth Color Shading

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();				// Reset The Projection Matrix

  gluPerspective(45.0f,(GLfloat)Width/(GLfloat)Height,0.1f,100.0f);	// Calculate The Aspect Ratio Of The Window

  glMatrixMode(GL_MODELVIEW);
}

/* The function called when our window is resized (which shouldn't happen, because we're fullscreen) */
void ReSizeGLScene(int Width, int Height)
{
  if (Height==0)				// Prevent A Divide By Zero If The Window Is Too Small
    Height=1;

  glViewport(0, 0, Width, Height);		// Reset The Current Viewport And Perspective Transformation

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();

  gluPerspective(45.0f,(GLfloat)Width/(GLfloat)Height,0.1f,100.0f);
  glMatrixMode(GL_MODELVIEW);
}

void Leg(GLfloat pos)
{
  glBegin(GL_POLYGON);
  glColor3f(0.5f,0.5f,0.5f);  //Front
  glVertex3f(1.0f+pos,2.0f,1.0f);  //Front
  glVertex3f(-1.0f+pos,2.0f,1.0f); //Front
  glVertex3f(-1.0f+pos,-2.0f,1.0f); //Front
  glVertex3f(1.0f+pos,-2.0f,1.0f);  //Front
  glEnd();
  glBegin(GL_POLYGON);
  glColor3f(0.5f,0.5f,0.5f);  //Top Knee
  glVertex3f(1.0+pos,-2.0f,1.0f);  //Top Knee
  glVertex3f(-1.0+pos,-2.0f,1.0f);  //Top Knee
  glVertex3f(-1.0+pos,-2.0f,3.0f);  //Top Knee
  glVertex3f(1.0+pos,-2.0f,3.0f);  //Top Knee
  glEnd();
  glBegin(GL_POLYGON);
  glColor3f(0.5f,0.5f,0.5f);  //Front
  glVertex3f(1.0f+pos,-2.0f,3.0f); //Front knee
  glVertex3f(-1.0f+pos,-2.0f,3.0f); //Front knee
  glVertex3f(-1.0f+pos,-3.0f,3.0f); //Front knee
  glVertex3f(1.0f+pos,-3.0f,3.0f); //Front knee
  glEnd();
  glBegin(GL_POLYGON);
  glColor3f(0.5f,0.5f,0.5f);  //Bottom Knee
  glVertex3f(1.0+pos,-3.0f,0.0f);  //Bottom Knee
  glVertex3f(-1.0+pos,-3.0f,0.0f);  //Bottom Knee
  glVertex3f(-1.0+pos,-3.0f,3.0f);  //Bottom Knee
  glVertex3f(1.0+pos,-3.0f,3.0f);  //Bottom Knee
  glEnd();
  glBegin(GL_POLYGON);
  glColor3f(0.5f,0.5f,0.5f);  //Back
  glVertex3f(1.0f+pos,2.0f,0.0f);  //Back
  glVertex3f(-1.0f+pos,2.0f,0.0f); //Back
  glVertex3f(-1.0f+pos,-3.0f,0.0f); //Back
  glVertex3f(1.0f+pos,-3.0f,0.0f);  //Back
  glEnd();
  glBegin(GL_POLYGON);
  glColor3f(0.5f,0.5f,0.5f);  //Top
  glVertex3f(1.0+pos,2.0f,1.0f);  //Top
  glVertex3f(-1.0+pos,2.0f,1.0f);  //Top
  glVertex3f(-1.0+pos,2.0f,0.0f);  //Top
  glVertex3f(1.0+pos,2.0f,0.0f);  //Top
  glEnd();
  glBegin(GL_POLYGON);
  glColor3f(0.5f,0.5f,0.5f);  //RightLong
  glVertex3f(1.0f+pos,2.0f,1.0f);  //RightLong
  glVertex3f(1.0f+pos,2.0f,0.0f);  //RightLong
  glVertex3f(1.0f+pos,-3.0f,0.0f);  //RightLong
  glVertex3f(1.0f+pos,-3.0f,1.0f);  //RightLong
  glVertex3f(1.0f+pos,2.0f,1.0f);   //RightLong
  glEnd();
  glBegin(GL_POLYGON);
  glColor3f(0.5f,0.5f,0.5f);  //RightShort
  glVertex3f(1.0f+pos,-3.0f,1.0f);  //RightShort
  glVertex3f(1.0f+pos,-3.0f,3.0f);  //RightShort
  glVertex3f(1.0f+pos,-2.0f,3.0f);  //RightShort
  glVertex3f(1.0f+pos,-2.0f,1.0f);  //RightShort
  //glVertex3f(1.0f,-3.0f,1.0f);   //RightShort
  glEnd();
  glBegin(GL_POLYGON);
  glColor3f(0.5f,0.5f,0.5f);  //LeftLong
  glVertex3f(-1.0f+pos,2.0f,1.0f);  //LeftLong
  glVertex3f(-1.0f+pos,2.0f,0.0f);  //LeftLong
  glVertex3f(-1.0f+pos,-3.0f,0.0f);  //LeftLong
  glVertex3f(-1.0f+pos,-3.0f,1.0f);  //LeftLong
  glVertex3f(-1.0f+pos,2.0f,1.0f);   //LeftLong
  glEnd();
  glBegin(GL_POLYGON);
  glColor3f(0.5f,0.5f,0.5f);  //LeftShort
  glVertex3f(-1.0f+pos,-3.0f,1.0f);  //LeftShort
  glVertex3f(-1.0f+pos,-3.0f,3.0f);  //LeftShort
  glVertex3f(-1.0f+pos,-2.0f,3.0f);  //LeftShort
  glVertex3f(-1.0f+pos,-2.0f,1.0f);  //LeftShort
  //glVertex3f(1.0f,-3.0f,1.0f);   //LeftShort
  glEnd();
}

void torso()
{
glBindTexture(GL_TEXTURE_2D, texture[0]);
  glBegin(GL_POLYGON);
  glColor3f(0.5f,0.5f,0.5f);  //Back
  glTexCoord2f(1.0f, 0.0f); glVertex3f(1.0f,1.0f,1.01f); //Back
  glTexCoord2f(1.0f, 1.0f); glVertex3f(1.0f,3.0f,1.01f); //Back
  glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f,3.0f,1.01f); //Back
  glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f,1.0f,1.01f); //Back
  glEnd();
  glBegin(GL_POLYGON);
  glColor3f(0.5f,0.5f,0.5f);  //Back
  glVertex3f(2.5f,0.0f,1.0f); //Back
  glVertex3f(1.5f,4.0f,1.0f); //Back
  glVertex3f(-1.5f,4.0f,1.0f); //Back
  glVertex3f(-2.5f,0.0f,1.0f); //Back
  glEnd();

  glBegin(GL_POLYGON);
  glColor3f(0.5f,0.5f,0.5f);  //Front
  glVertex3f(2.5f,0.0f,0.0f); //Front
  glVertex3f(1.5f,4.0f,0.0f); //Front
  glVertex3f(-1.5f,4.0f,0.0f); //Front
  glVertex3f(-2.5f,0.0f,0.0f); //Front
  glEnd();
  glBegin(GL_POLYGON);
  glColor3f(0.5f,0.5f,0.5f);  //Left
  glVertex3f(-1.5f,4.0f,1.0f); //Left
  glVertex3f(-1.5f,4.0f,0.0f); //Left
  glVertex3f(-2.5f,0.0f,0.0f); //Left
  glVertex3f(-2.5f,0.0f,1.0f); //Left
  glEnd();
  glBegin(GL_POLYGON);
  glColor3f(0.5f,0.5f,0.5f);  //Right
  glVertex3f(1.5f,4.0f,1.0f); //Right
  glVertex3f(1.5f,4.0f,0.0f); //Right
  glVertex3f(2.5f,0.0f,0.0f); //Right
  glVertex3f(2.5f,0.0f,1.0f); //Right
  glEnd();
  glBegin(GL_POLYGON);
  glColor3f(0.5f,0.5f,0.5f);  //RodFront
  glVertex3f(-0.25f,4.5f,0.2f); //RodFront
  glVertex3f(-0.25f,4.0f,0.2f); //RodFront
  glVertex3f(0.25f,4.0f,0.2f); //RodFront
  glVertex3f(0.25f,4.5f,0.2f); //RodFront
  glEnd();
  glBegin(GL_POLYGON);
  glColor3f(0.5f,0.5f,0.5f);  //RodBack
  glVertex3f(-0.25f,4.5f,0.8f); //RodBack
  glVertex3f(-0.25f,4.0f,0.8f); //RodBack
  glVertex3f(0.25f,4.0f,0.8f); //RodBack
  glVertex3f(0.25f,4.5f,0.8f); //RodBack
  glEnd();
  glBegin(GL_POLYGON);
  glColor3f(0.5f,0.5f,0.5f);  //RodBack
  glVertex3f(0.25f,4.0f,0.2f); //RodBack
  glVertex3f(0.25f,4.0f,0.8f); //RodBack
  glVertex3f(0.25f,4.5f,0.8f); //RodBack
  glVertex3f(0.25f,4.5f,0.2f); //RodBack
  glEnd();
  glBegin(GL_POLYGON);
  glColor3f(0.5f,0.5f,0.5f);  //RodBack
  glVertex3f(-0.25f,4.0f,0.2f); //RodBack
  glVertex3f(-0.25f,4.0f,0.8f); //RodBack
  glVertex3f(-0.25f,4.5f,0.8f); //RodBack
  glVertex3f(-0.25f,4.5f,0.2f); //RodBack
  glEnd();
}

void handr(GLfloat off)
{
  glBegin(GL_POLYGON);
  glColor3f(0.5f,0.5f,0.5f);  //Front
  glVertex3f(-1.5f+off,6.0f,1.0f);  //Front
  glVertex3f(-1.5f+off,5.0f,1.0f); //Front
  glVertex3f(-0.5f+off,5.0f,1.0f); //Front
  glVertex3f(-0.5f+off,6.0f,1.0f);  //Front
  glEnd();
  glBegin(GL_POLYGON);
  glColor3f(0.5f,0.5f,0.5f);  //Front
  glVertex3f(0.5f+off,0.0f,1.0f);  //Front
  glVertex3f(2.0f+off,0.0f,1.0f); //Front
  glVertex3f(2.0f+off,1.0f,1.0f); //Front
  glVertex3f(0.5f+off,1.0f,1.0f);  //Front
  glEnd();
  glBegin(GL_POLYGON);
  glColor3f(0.5f,0.5f,0.5f);  //Front
  glVertex3f(-0.5f+off,6.0f,1.0f);  //Front
  glVertex3f(-0.8f+off,5.0f,1.0f); //Front
  glVertex3f(0.7f+off,1.0f,1.0f); //Front
  glVertex3f(1.8f+off,1.0f,1.0f);  //Front
  glEnd();

  glBegin(GL_POLYGON);
  glColor3f(0.5f,0.5f,0.5f);  //Back
  glVertex3f(-1.5f+off,6.0f,0.0f);  //Back
  glVertex3f(-1.5f+off,5.0f,0.0f); //Back
  glVertex3f(-0.5f+off,5.0f,0.0f); //Back
  glVertex3f(-0.5f+off,6.0f,0.0f);  //Back
  glEnd();
  glBegin(GL_POLYGON);
  glColor3f(0.5f,0.5f,0.5f);  //Back
  glVertex3f(0.5f+off,0.0f,0.0f);  //Back
  glVertex3f(2.0f+off,0.0f,0.0f); //Back
  glVertex3f(2.0f+off,1.0f,0.0f); //Back
  glVertex3f(0.5f+off,1.0f,0.0f);  //Back
  glEnd();
  glBegin(GL_POLYGON);
  glColor3f(0.5f,0.5f,0.5f);  //Back
  glVertex3f(-0.5f+off,6.0f,0.0f);  //Back
  glVertex3f(-0.8f+off,5.0f,0.0f); //Back
  glVertex3f(0.7f+off,1.0f,0.0f); //Bac0
  glVertex3f(1.8f+off,1.0f,0.0f);  //Back
  glEnd();
  glBegin(GL_POLYGON);
  glColor3f(0.5f,0.5f,0.5f);  //SideOuter
  glVertex3f(-0.5f+off,6.0f,0.0f);  //SideOuter
  glVertex3f(1.8f+off,1.0f,0.0f); //SideOuter
  glVertex3f(1.8f+off,1.0f,1.0f); //SideOuter
  glVertex3f(-0.5f+off,6.0f,1.0f);  //SideOuter
  glEnd();
  glBegin(GL_POLYGON);
  glColor3f(0.5f,0.5f,0.5f);  //SideOuter
  glVertex3f(2.0f+off,1.0f,0.0f);  //SideOuter
  glVertex3f(2.0f+off,0.0f,0.0f); //SideOuter
  glVertex3f(2.0f+off,0.0f,1.0f); //SideOuter
  glVertex3f(2.0f+off,1.0f,1.0f);  //SideOuter
  glEnd();
  glBegin(GL_POLYGON);
  glColor3f(0.5f,0.5f,0.5f);  //SideOuter
  glVertex3f(-1.5f+off,6.0f,0.0f);  //SideOuter
  glVertex3f(-0.5f+off,6.0f,0.0f); //SideOuter
  glVertex3f(-0.5f+off,6.0f,1.0f); //SideOuter
  glVertex3f(-1.5f+off,6.0f,1.0f);  //SideOuter
  glEnd();
}
void handl(GLfloat off)
{
  glBegin(GL_POLYGON);
  glColor3f(0.5f,0.5f,0.5f);  //Front
  glVertex3f(1.5f+off,6.0f,1.0f);  //Front
  glVertex3f(1.5f+off,5.0f,1.0f); //Front
  glVertex3f(0.5f+off,5.0f,1.0f); //Front
  glVertex3f(0.5f+off,6.0f,1.0f);  //Front
  glEnd();
  glBegin(GL_POLYGON);
  glColor3f(0.5f,0.5f,0.5f);  //Front
  glVertex3f(-0.5f+off,0.0f,1.0f);  //Front
  glVertex3f(-2.0f+off,0.0f,1.0f); //Front
  glVertex3f(-2.0f+off,1.0f,1.0f); //Front
  glVertex3f(-0.5f+off,1.0f,1.0f);  //Front
  glEnd();
  glBegin(GL_POLYGON);
  glColor3f(0.5f,0.5f,0.5f);  //Front
  glVertex3f(0.5f+off,6.0f,1.0f);  //Front
  glVertex3f(0.8f+off,5.0f,1.0f); //Front
  glVertex3f(-0.7f+off,1.0f,1.0f); //Front
  glVertex3f(-1.8f+off,1.0f,1.0f);  //Front
  glEnd();

  glBegin(GL_POLYGON);
  glColor3f(0.5f,0.5f,0.5f);  //Back
  glVertex3f(1.5f+off,6.0f,0.0f);  //Back
  glVertex3f(1.5f+off,5.0f,0.0f); //Back
  glVertex3f(0.5f+off,5.0f,0.0f); //Back
  glVertex3f(0.5f+off,6.0f,0.0f);  //Back
  glEnd();
  glBegin(GL_POLYGON);
  glColor3f(0.5f,0.5f,0.5f);  //Back
  glVertex3f(-0.5f+off,0.0f,0.0f);  //Back
  glVertex3f(-2.0f+off,0.0f,0.0f); //Back
  glVertex3f(-2.0f+off,1.0f,0.0f); //Back
  glVertex3f(-0.5f+off,1.0f,0.0f);  //Back
  glEnd();
  glBegin(GL_POLYGON);
  glColor3f(0.5f,0.5f,0.5f);  //Back
  glVertex3f(0.5f+off,6.0f,0.0f);  //Back
  glVertex3f(0.8f+off,5.0f,0.0f); //Back
  glVertex3f(-0.7f+off,1.0f,0.0f); //Bac0
  glVertex3f(-1.8f+off,1.0f,0.0f);  //Back
  glEnd();
  glBegin(GL_POLYGON);
  glColor3f(0.5f,0.5f,0.5f);  //SideOuter
  glVertex3f(0.5f+off,6.0f,0.0f);  //SideOuter
  glVertex3f(-1.8f+off,1.0f,0.0f); //SideOuter
  glVertex3f(-1.8f+off,1.0f,1.0f); //SideOuter
  glVertex3f(0.5f+off,6.0f,1.0f);  //SideOuter
  glEnd();
  glBegin(GL_POLYGON);
  glColor3f(0.5f,0.5f,0.5f);  //SideOuter
  glVertex3f(-2.0f+off,1.0f,0.0f);  //SideOuter
  glVertex3f(-2.0f+off,0.0f,0.0f); //SideOuter
  glVertex3f(-2.0f+off,0.0f,1.0f); //SideOuter
  glVertex3f(-2.0f+off,1.0f,1.0f);  //SideOuter
  glEnd();
  glBegin(GL_POLYGON);
  glColor3f(0.5f,0.5f,0.5f);  //SideOuter
  glVertex3f(1.5f+off,6.0f,0.0f);  //SideOuter
  glVertex3f(0.5f+off,6.0f,0.0f); //SideOuter
  glVertex3f(0.5f+off,6.0f,1.0f); //SideOuter
  glVertex3f(1.5f+off,6.0f,1.0f);  //SideOuter
  glEnd();
}

void pelvis()
{
  glBegin(GL_POLYGON);
  glColor3f(0.5f,0.5f,0.5f);  //Front
  glVertex3f(-2.5f,1.5f,1.0f); //Front
  glVertex3f(-2.5f,1.0f,1.0f); //Front
  glVertex3f(2.5f,1.0f,1.0f); //Front
  glVertex3f(2.5f,1.5f,1.0f); //Front
  glEnd();
  glBegin(GL_POLYGON);
  glColor3f(0.5f,0.5f,0.5f);  //Front
  glVertex3f(-0.5f,1.0f,1.0f); //Front
  glVertex3f(-0.5f,0.0f,1.0f); //Front
  glVertex3f(0.5f,0.0f,1.0f); //Front
  glVertex3f(0.5f,1.0f,1.0f); //Front
  glEnd();

  glBegin(GL_POLYGON);
  glColor3f(0.5f,0.5f,0.5f);  //Back
  glVertex3f(-2.5f,1.5f,0.0f); //Back
  glVertex3f(-2.5f,1.0f,0.0f); //Back
  glVertex3f(2.5f,1.0f,0.0f); //Back
  glVertex3f(2.5f,1.5f,0.0f); //Back
  glEnd();
  glBegin(GL_POLYGON);
  glColor3f(0.5f,0.5f,0.5f);  //Back
  glVertex3f(-0.5f,1.0f,0.0f); //Back
  glVertex3f(-0.5f,0.0f,0.0f); //Back
  glVertex3f(0.5f,0.0f,0.0f); //Back
  glVertex3f(0.5f,1.0f,0.0f); //Back
  glEnd();

  glBegin(GL_POLYGON);
  glColor3f(0.5f,0.5f,0.5f);  //Right
  glVertex3f(2.5f,1.5f,0.0f); //Right
  glVertex3f(2.5f,1.0f,0.0f); //Right
  glVertex3f(2.5f,1.0f,1.0f); //Right
  glVertex3f(2.5f,1.5f,1.0f); //Right
  glEnd();
  glBegin(GL_POLYGON);
  glColor3f(0.5f,0.5f,0.5f);  //Right
  glVertex3f(0.5f,0.0f,0.0f); //Right
  glVertex3f(0.5f,0.0f,1.0f); //Right
  glVertex3f(0.5f,1.0f,1.0f); //Right
  glVertex3f(0.5f,1.0f,0.0f); //Right
  glEnd();

  glBegin(GL_POLYGON);
  glColor3f(0.5f,0.5f,0.5f);  //Left
  glVertex3f(-2.5f,1.5f,0.0f); //Left
  glVertex3f(-2.5f,1.0f,0.0f); //Left
  glVertex3f(-2.5f,1.0f,1.0f); //Left
  glVertex3f(-2.5f,1.5f,1.0f); //Left
  glEnd();
  glBegin(GL_POLYGON);
  glColor3f(0.5f,0.5f,0.5f);  //Left
  glVertex3f(-0.5f,0.0f,0.0f); //Left
  glVertex3f(-0.5f,0.0f,1.0f); //Left
  glVertex3f(-0.5f,1.0f,1.0f); //Left
  glVertex3f(-0.5f,1.0f,0.0f); //Left
  glEnd();

  glBegin(GL_POLYGON);
  glColor3f(0.5f,0.5f,0.5f);  //Top
  glVertex3f(-2.5f,1.5f,0.0f); //Top
  glVertex3f(2.5f,1.5f,0.0f); //Top
  glVertex3f(2.5f,1.5f,1.0f); //Top
  glVertex3f(-2.5f,1.5f,1.0f); //Top
  glEnd();

  glBegin(GL_POLYGON);
  glColor3f(1.0f,1.0f,1.0f);  //RHP
  glVertex3f(0.5f,0.5f,0.25f); //RHP
  glVertex3f(2.0f,0.5f,0.25f); //RHP
  glVertex3f(2.0f,0.75f,0.25f); //RHP
  glVertex3f(0.5f,0.75f,0.25f); //RHP
  glEnd();

  glBegin(GL_POLYGON);
  glColor3f(1.0f,1.0f,1.0f);  //RHP
  glVertex3f(0.5f,0.5f,0.50f); //RHP
  glVertex3f(2.0f,0.5f,0.50f); //RHP
  glVertex3f(2.0f,0.75f,0.50f); //RHP
  glVertex3f(0.5f,0.75f,0.50f); //RHP
  glEnd();

  glBegin(GL_POLYGON);
  glColor3f(1.0f,1.0f,1.0f);  //RHP
  glVertex3f(2.0f,0.5f,0.25f); //RHP
  glVertex3f(2.0f,0.75f,0.25f); //RHP
  glVertex3f(2.0f,0.75f,0.50f); //RHP
  glVertex3f(2.0f,0.5f,0.50f); //RHP
  glEnd();

  //-------------------
  glBegin(GL_POLYGON);
  glColor3f(1.0f,1.0f,1.0f);  //RHP
  glVertex3f(-0.5f,0.5f,0.25f); //RHP
  glVertex3f(-2.0f,0.5f,0.25f); //RHP
  glVertex3f(-2.0f,0.75f,0.25f); //RHP
  glVertex3f(-0.5f,0.75f,0.25f); //RHP
  glEnd();

  glBegin(GL_POLYGON);
  glColor3f(1.0f,1.0f,1.0f);  //RHP
  glVertex3f(-0.5f,0.5f,0.50f); //RHP
  glVertex3f(-2.0f,0.5f,0.50f); //RHP
  glVertex3f(-2.0f,0.75f,0.50f); //RHP
  glVertex3f(-0.5f,0.75f,0.50f); //RHP
  glEnd();

  glBegin(GL_POLYGON);
  glColor3f(1.0f,1.0f,1.0f);  //RHP
  glVertex3f(-2.0f,0.5f,0.25f); //RHP
  glVertex3f(-2.0f,0.75f,0.25f); //RHP
  glVertex3f(-2.0f,0.75f,0.50f); //RHP
  glVertex3f(-2.0f,0.5f,0.50f); //RHP
  glEnd();

  glBegin(GL_POLYGON);
  glColor3f(1.0f,1.0f,1.0f);  //HR
  glVertex3f(-0.5f,1.5f,0.5f); //HR
  glVertex3f(-0.5f,2.5f,0.5f); //HR
  glVertex3f(-1.5f,2.5f,0.50f); //HR
  glVertex3f(-1.5f,1.5f,0.50f); //HR
  glEnd();
  glBegin(GL_POLYGON);
  glColor3f(1.0f,1.0f,1.0f);  //HR
  glVertex3f(-0.5f,1.5f,0.25f); //HR
  glVertex3f(-0.5f,2.5f,0.25f); //HR
  glVertex3f(-1.5f,2.5f,0.250f); //HR
  glVertex3f(-1.5f,1.5f,0.250f); //HR
  glEnd();

  glBegin(GL_POLYGON);
  glColor3f(1.0f,1.0f,1.0f);  //HR
  glVertex3f(0.5f,1.5f,0.5f); //HR
  glVertex3f(0.5f,2.5f,0.5f); //HR
  glVertex3f(1.5f,2.5f,0.50f); //HR
  glVertex3f(1.5f,1.5f,0.50f); //HR
  glEnd();
  glBegin(GL_POLYGON);
  glColor3f(1.0f,1.0f,1.0f);  //HR
  glVertex3f(0.5f,1.5f,0.25f); //HR
  glVertex3f(0.5f,2.5f,0.25f); //HR
  glVertex3f(1.5f,2.5f,0.250f); //HR
  glVertex3f(1.5f,1.5f,0.250f); //HR
  glEnd();

  glBegin(GL_POLYGON);
  glColor3f(1.0f,1.0f,1.0f);  //HR
  glVertex3f(1.5f,1.5f,0.5f); //HR
  glVertex3f(1.5f,1.5f,0.25f); //HR
  glVertex3f(1.5f,2.5f,0.250f); //HR
  glVertex3f(1.5f,2.5f,0.50f); //HR
  glEnd();

  glBegin(GL_POLYGON);
  glColor3f(1.0f,1.0f,1.0f);  //HR
  glVertex3f(-1.5f,2.5f,0.5f); //HR
  glVertex3f(-1.5f,1.5f,0.5f); //HR
  glVertex3f(-1.5f,1.5f,0.250f); //HR
  glVertex3f(-1.5f,2.5f,0.250f); //HR
  glEnd();
}

void belt()
{
  glBegin(GL_POLYGON);
  glColor3f(1.0f,1.0f,0.0f);  //Front
  glVertex3f(-2.5f,0.5f,1.01f);  //Front
  glVertex3f(-2.5f,0.0f,1.01f); //Front
  glVertex3f(-1.5f,0.0f,1.01f); //Front
  glVertex3f(-1.5f,0.5f,1.01f);  //Front
  glEnd();
  glBegin(GL_POLYGON);
  glColor3f(1.0f,1.0f,0.0f);  //Front
  glVertex3f(2.5f,0.5f,1.01f);  //Front
  glVertex3f(2.5f,0.0f,1.01f); //Front
  glVertex3f(1.5f,0.0f,1.01f); //Front
  glVertex3f(1.5f,0.5f,1.01f);  //Front
  glEnd();

  glBegin(GL_POLYGON);
  glColor3f(1.0f,1.0f,0.0f);  //Front
  glVertex3f(-0.5f,0.3f,1.01f);  //Front
  glVertex3f(-1.5f,0.3f,1.01f); //Front
  glVertex3f(-1.5f,0.2f,1.01f); //Front
  glVertex3f(-0.5f,0.2f,1.01f);  //Front
  glEnd();

  glBegin(GL_POLYGON);
  glColor3f(1.0f,1.0f,0.0f);  //Front
  glVertex3f(0.5f,0.3f,1.01f);  //Front
  glVertex3f(1.5f,0.3f,1.01f); //Front
  glVertex3f(1.5f,0.2f,1.01f); //Front
  glVertex3f(0.5f,0.2f,1.01f);  //Front
  glEnd();

  glBegin(GL_POLYGON);
  glVertex3f(-0.5f,0.4f,1.01f);  //Front
  glColor3f(1.0f,1.0f,0.0f);  //Front
  glVertex3f(-0.5f,0.1f,1.01f); //Front
  glVertex3f(0.5f,0.1f,1.01f); //Front
  glVertex3f(0.5f,0.4f,1.01f);  //Front
  glEnd();

  //--------------------------------

  glBegin(GL_POLYGON);
  glColor3f(1.0f,1.0f,0.0f);  //Front
  glVertex3f(-2.5f,0.5f,-0.01f);  //Front
  glVertex3f(-2.5f,0.0f,-0.01f); //Front
  glVertex3f(-1.5f,0.0f,-0.01f); //Front
  glVertex3f(-1.5f,0.5f,-0.01f);  //Front
  glEnd();
  glBegin(GL_POLYGON);
  glColor3f(1.0f,1.0f,0.0f);  //Front
  glVertex3f(2.5f,0.5f,-0.01f);  //Front
  glVertex3f(2.5f,0.0f,-0.01f); //Front
  glVertex3f(1.5f,0.0f,-0.01f); //Front
  glVertex3f(1.5f,0.5f,-0.01f);  //Front
  glEnd();

  glBegin(GL_POLYGON);
  glColor3f(1.0f,1.0f,0.0f);  //Front
  glVertex3f(-0.5f,0.3f,-0.01f);  //Front
  glVertex3f(-1.5f,0.3f,-0.01f); //Front
  glVertex3f(-1.5f,0.2f,-0.01f); //Front
  glVertex3f(-0.5f,0.2f,-0.01f);  //Front
  glEnd();

  glBegin(GL_POLYGON);
  glColor3f(1.0f,1.0f,0.0f);  //Front
  glVertex3f(0.5f,0.3f,-0.01f);  //Front
  glVertex3f(1.5f,0.3f,-0.01f); //Front
  glVertex3f(1.5f,0.2f,-0.01f); //Front
  glVertex3f(0.5f,0.2f,-0.01f);  //Front
  glEnd();

  glBegin(GL_POLYGON);
  glColor3f(1.0f,1.0f,0.0f);  //Front
  glVertex3f(-0.5f,0.4f,-0.01f);  //Front
  glVertex3f(-0.5f,0.1f,-0.01f); //Front
  glVertex3f(0.5f,0.1f,-0.01f); //Front
  glVertex3f(0.5f,0.4f,-0.01f);  //Front
  glEnd();

  glBegin(GL_POLYGON);
  glColor3f(1.0f,1.0f,0.0f);  //right
  glVertex3f(2.51f,0.0f,-0.01f);  //right
  glVertex3f(2.51f,0.0f,1.01f); //right
  glVertex3f(2.51f,0.5f,1.01f); //right
  glVertex3f(2.51f,0.5f,-0.01f);  //right
  glEnd();

  glBegin(GL_POLYGON);
  glColor3f(1.0f,1.0f,0.0f);  //Left
  glVertex3f(-2.51f,0.0f,-0.01f);  //Left
  glVertex3f(-2.51f,0.0f,1.01f); //Left
  glVertex3f(-2.51f,0.5f,1.01f); //Left
  glVertex3f(-2.51f,0.5f,-0.01f);  //Left
  glEnd();
}

/* The main drawing function. */
void DrawGLScene()
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);		// Clear The Screen And The Depth Buffer
  glLoadIdentity();
  glTranslatef(0.0,0.0,0.0);
  glLoadIdentity();				// Reset The View
  glTranslatef(0.0f,0.0f,zoom);
  glRotatef(rot,0.0f,1.0f,0.0f);
  Leg(1.5);
  glLoadIdentity();				// Reset The View
  glTranslatef(0.0f,0.0f,zoom);
  glRotatef(rot,0.0f,1.0f,0.0f);
  Leg(-1.5);
  glLoadIdentity();				// Reset The View
  glTranslatef(0.0f,2.0f,zoom);
  glRotatef(rot,0.0f,1.0f,0.0f);
  belt();
  glLoadIdentity();				// Reset The View
  glTranslatef(0.0f,2.6f,zoom);
  glRotatef(rot,0.0f,1.0f,0.0f);
  torso();
  glLoadIdentity();				// Reset The View
  glTranslatef(0.0f,1.1f,zoom);
  glRotatef(rot,0.0f,1.0f,0.0f);
  pelvis();

  glLoadIdentity();				// Reset The View
  glTranslatef(0.0f,0.6f,zoom);
  glRotatef(rot,0.0f,1.0f,0.0f);
  handr(3.0);
  glLoadIdentity();
  glTranslatef(0.0f,0.6f,zoom);
  glRotatef(rot,0.0f,1.0f,0.0f);
  handl(-3.0);
  //Leg();


  // since this is double buffered, swap the buffers to display what just got drawn.
  glutSwapBuffers();
}

/* The function called whenever a key is pressed. */
void keyPressed(unsigned char key, int x, int y)
{
    /* avoid thrashing this procedure */
    usleep(100);

    /* If escape is pressed, kill everything. */
    if (key == ESCAPE)
    {
	/* shut down our window */
	glutDestroyWindow(window);

	/* exit the program...normal termination. */
	exit(0);
    }
}
void specialKeys( int key, int x, int y ) {
  if (key == GLUT_KEY_LEFT)
    rot += 5.0f;
  else if (key == GLUT_KEY_RIGHT)
    rot -= 5.0f;
    else if (key == GLUT_KEY_UP)
      zoom += 0.5f;
      else if (key == GLUT_KEY_DOWN)
        zoom -= 0.5f;

  glutPostRedisplay();

}

int main(int argc, char **argv)
{
  /* Initialize GLUT state - glut will take any command line arguments that pertain to it or
     X Windows - look at its documentation at http://reality.sgi.com/mjk/spec3/spec3.html */
  glutInit(&argc, argv);

  /* Select type of Display mode:
     Double buffer
     RGBA color
     Alpha components supported
     Depth buffer */
  glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_ALPHA | GLUT_DEPTH);

  /* get a 640 x 480 window */
  glutInitWindowSize(640, 480);

  /* the window starts at the upper left corner of the screen */
  glutInitWindowPosition(0, 0);

  /* Open a window */
  window = glutCreateWindow("LEGO Batman");

  /* Register the function to do all our OpenGL drawing. */
  glutDisplayFunc(&DrawGLScene);

  /* Go fullscreen.  This is as soon as possible. */
  glutFullScreen();

  /* Even if there are no events, redraw our gl scene. */
  glutIdleFunc(&DrawGLScene);

  /* Register the function called when our window is resized. */
  glutReshapeFunc(&ReSizeGLScene);

  /* Register the function called when the keyboard is pressed. */
  glutKeyboardFunc(&keyPressed);
  glutSpecialFunc(&specialKeys);

  /* Initialize our window. */
  InitGL(640, 480);

  /* Start Event Processing Engine */
  glutMainLoop();

  return 1;
}
