/*

EECS 366/466 COMPUTER GRAPHICS
Template for Assignment 8-MAPPING
Spring 2006

*/

#include "stdio.h"
#include "stdlib.h"
#include "iostream"
#include "math.h"

#include "string.h"
#include "glut.h"
#include "windows.h"
#include "GL/glu.h"
#include "GL/gl.h"

#include "glprocs.h"
# include "assign8_temp.h"
#include "read_tga.h"

#define PI 3.14159265359

#define PrintOpenGLError()::PrintOGLError(__FILE__, __LINE__)

using namespace std;


//object related information
int verts, faces, norms;    // Number of vertices, faces and normals in the system
point *vertList, *normList; // Vertex and Normal Lists
faceStruct *faceList;	    // Face List


//Illimunation and shading related declerations
char *shaderFileRead(char *fn);
GLuint vertex_shader,fragment_shader,p;
int program=-1;
int algorithm = 0;
int object = 0;
int texture = 0;
int mapping = 0;

//Parameters for Copper (From: "Computer Graphics Using OpenGL" BY F.S. Hill, Jr.) 
GLfloat ambient_cont [] = {0.19125,0.0735,0.0225};
GLfloat diffuse_cont [] = {0.7038,0.27048,0.0828};
GLfloat specular_cont [] = {0.256777,0.137622,0.086014};
GLfloat exponent = 12.8;


//Projection, camera contral related declerations
int WindowWidth,WindowHeight;
bool LookAtObject = false;
bool ShowAxes = true;



float CameraRadius = 10;
float CameraTheta = PI / 2;
float CameraPhi = PI / 2;
int MouseX = 0;
int MouseY = 0;
bool MouseLeft = false;
bool MouseRight = false;

GLUquadric *quad;

void drawTriangle(point v1, point v2, point v3)
{
	float x1, y1;
	glBegin(GL_TRIANGLES);
				point n1, n2, n3;
				n1 = v1;
				n2 = v2;
				n3 = v3;
				glNormal3f(n1.x, n1.y, n1.z);
				if(texture == 0)
					glTexCoord2f (v1.x, v1.y);
				else
				{
					x1 = atan2(n1.x,n1.z)/(2 * PI) + .5;
					y1 = asin(n1.y)/PI + .5;
					glTexCoord2f (x1, y1);
				}
				glVertex3f(v1.x, v1.y, v1.z);
				glNormal3f(n2.x, n2.y, n2.z);
				if(texture == 0)
					glTexCoord2f (v2.x, v2.y);
				else
				{
					float x = atan2(n2.x,n2.z)/(2 * PI) + .5;
					float y =  asin(n2.y)/PI + .5;
					glTexCoord2f (x,y);
				}
				glVertex3f(v2.x, v2.y, v2.z);
				glNormal3f(n3.x, n3.y, n3.z);
				if(texture == 0)
					glTexCoord2f (v3.x, v3.y);
				else
				{
					float x = atan2(n3.x,n3.z)/(2 * PI) + .5;
					float y =  asin(n3.y)/PI + .5;
					glTexCoord2f (x,y);
				}
				glVertex3f(v3.x, v3.y, v3.z);
			glEnd();
}

void DisplayFunc(void) 
{
    GLuint id ;
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//load projection and viewing transforms
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
        
	gluPerspective(60,(GLdouble) WindowWidth/WindowHeight,0.01,10000);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(CameraRadius*cos(CameraTheta)*sin(CameraPhi),
			  CameraRadius*cos(CameraPhi),
			  CameraRadius*sin(CameraTheta)*sin(CameraPhi),
			  0,0,0,
			  0,1,0);

	glEnable(GL_DEPTH_TEST);	
	glEnable(GL_TEXTURE_2D);

		setParameters(program);
	
	// Load image from tga file
	char* tgaFile = "";
	if(texture == 0 && mapping == 0)
		tgaFile = "./planartexturemap/abstract2.tga";
	else if(texture == 1 && mapping == 0)
		tgaFile = "./sphericaltexturemap/earth2.tga";
	else if(mapping == 1)
		tgaFile = "./sphericalenvironmentmap/house2.tga";
	else
		tgaFile = "./planartexturemap/abstract2.tga";
		//tgaFile = "./planarbumpmap/abstract_gray2.tga";

	TGA *TGAImage = new TGA(tgaFile);

	// Use to dimensions of the image as the texture dimensions
	uint width	= TGAImage->GetWidth();
	uint height	= TGAImage->GetHeigth();
	
	// The parameters for actual textures are changed

	glGenTextures(1, &id);

	glBindTexture(GL_TEXTURE_2D, id);


	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);

	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);


	// Finaly build the mipmaps
	glTexImage2D (GL_TEXTURE_2D, 0, 4, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, TGAImage->GetPixels());

	gluBuild2DMipmaps(GL_TEXTURE_2D, 4, width, height, GL_RGBA, GL_UNSIGNED_BYTE, TGAImage->GetPixels());

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glEnable( GL_TEXTURE_2D );

	glBindTexture (GL_TEXTURE_2D, id); 

    delete TGAImage;
	if (object == 0)
	{
		float w = .1;
		for (float i = -20; i < 20; i += w)
			for (float j = -20; j < 20; j += w)
			{
				point v1, v2, v3, v4;
				v1.x = i;
				v1.y = j;
				v1.z = 0;
				v2.x = i+w;
				v2.y = j;
				v2.z = 0;
				v3.x = i;
				v3.y = j+w;
				v3.z = 0;
				v4.x = i+w;
				v4.y = j+w;
				v4.z = 0;
				drawTriangle(v1, v2, v3);
				drawTriangle(v2, v3, v4);
			}	
	}
	else if(object == 1)
	{
		gluQuadricTexture(quad,1);
		gluSphere(quad,2,20,20);
	}
	else if (object == 2)
		for (int i = 0; i < faces; i++)
			drawTriangle(vertList[faceList[i].v1], vertList[faceList[i].v2], vertList[faceList[i].v3]);



	//glutSolidTeapot(1);
	//setParameters(program);
	glutSwapBuffers();
}


void ReshapeFunc(int x,int y)
{
    glViewport(0,0,x,y);
    WindowWidth = x;
    WindowHeight = y;
}


void MouseFunc(int button,int state,int x,int y)
{
	MouseX = x;
	MouseY = y;

    if(button == GLUT_LEFT_BUTTON)
		MouseLeft = !(bool) state;
	if(button == GLUT_RIGHT_BUTTON)
		MouseRight = !(bool) state;
}

void MotionFunc(int x, int y)
{
	if(MouseLeft)
	{
        CameraTheta += 0.01*PI*(MouseX - x);
		CameraPhi += 0.01*PI*(MouseY - y);
		if (CameraPhi > (PI - 0.01))
			CameraPhi = PI - 0.01;
		if (CameraPhi < 0.01)
			CameraPhi = 0.01;
	}
	if(MouseRight)
	{
        CameraRadius += 0.2*(MouseY-y);
		if(CameraRadius <= 0)
			CameraRadius = 0.2;
	}
    
	MouseX = x;
	MouseY = y;

	glutPostRedisplay();
}





//Motion and camera controls
void KeyboardFunc(unsigned char key, int x, int y)
{
    switch(key)
	{
	case 'A':
	case 'a':
		if(algorithm >= 7)
			algorithm = 0;
		else
			algorithm ++;
		break;
	case 'Q':
	case 'q':
		exit(1);
		break;
    default:
		break;
    }
	if(algorithm == 0 || algorithm == 7)
		object = 0;
	else if (algorithm == 1 || algorithm == 3 || algorithm == 5)
		object = 1;
	else
		object = 2;
	if(algorithm >= 3 && algorithm <= 6)
		texture = 1;
	else
		texture = 0;
	if(algorithm <= 4)
		mapping = 0;
	else if (algorithm <= 6)
		mapping = 1;
	else
		mapping = 2;

	glutPostRedisplay();
}

int main(int argc, char **argv) 
{			  
	quad = gluNewQuadric();
	meshReader("teapot.obj", 1);
	//meshReader("plane.obj", 1);

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100,100);
	glutInitWindowSize(320,320);
	glutCreateWindow("Assignment 6");



	glutDisplayFunc(DisplayFunc);
	glutReshapeFunc(ReshapeFunc);
	glutMouseFunc(MouseFunc);
    glutMotionFunc(MotionFunc);
    glutKeyboardFunc(KeyboardFunc);

	
	setShaders();

	glutMainLoop();

	return 0;
}

/*************************************************************
Shader related methods,
Setting the shader files
Setting the shader variables
*************************************************************/

void error_exit(int status, char *text)
{

	// Print error message

	fprintf(stderr,"Internal Error %i: ", status);
	fprintf(stderr,text);
	printf("\nTerminating as Result of Internal Error.\nPress Enter to exit.\n");

	// Keep the terminal open

	int anyKey = getchar();

	// Exit program

	exit(status);
}

int PrintOGLError(char *file, int line)
{
    GLenum glErr;
    int    retCode = 0;

    glErr = glGetError();
    while (glErr != GL_NO_ERROR)
    {
        printf("glError in file %s @ line %d: %s\n", file, line, gluErrorString(glErr));
        retCode = 1;
        glErr = glGetError();
    }
    return retCode;
}


void setShaders() 
{

	char *vs = NULL,*fs = NULL;

	//create the empty shader objects and get their handles
	vertex_shader = glCreateShaderObjectARB(GL_VERTEX_SHADER_ARB);
	fragment_shader = glCreateShaderObjectARB(GL_FRAGMENT_SHADER_ARB);
	

	//read the shader files and store the strings in corresponding char. arrays.
	//vs = shaderFileRead("sampleshader.vert");
	//fs = shaderFileRead("sampleshader.frag");
	vs = shaderFileRead("textMapShadder.vert");
	fs = shaderFileRead("textMapShadder.frag");

	const char * vv = vs;
	const char * ff = fs;

	GLint       vertCompiled, fragCompiled;

	//set the shader's source code by using the strings read from the shader files.
	glShaderSourceARB(vertex_shader, 1, &vv,NULL);
	glShaderSourceARB(fragment_shader, 1, &ff,NULL);

	free(vs);free(fs);

	//Compile the shader objects
	glCompileShaderARB(vertex_shader);
	glCompileShaderARB(fragment_shader);

	glGetObjectParameterivARB(fragment_shader, GL_OBJECT_COMPILE_STATUS_ARB, &fragCompiled);
	glGetObjectParameterivARB(vertex_shader, GL_OBJECT_COMPILE_STATUS_ARB, &vertCompiled);
    if (!vertCompiled)
	{
        cout<<"not compiled: vert"<<endl;
	}
	if (!fragCompiled)
		cout<<"not compiled: frag"<<endl;
	
	//create an empty program object to attach the shader objects
	p = glCreateProgramObjectARB();

	program =p;
	//attach the shader objects to the program object
	glAttachObjectARB(p,vertex_shader);
	glAttachObjectARB(p,fragment_shader);

	/*
	**************
	Programming Tip:
	***************
	Delete the attached shader objects once they are attached.
	They will be flagged for removal and will be freed when they are no more used.
	*/
	glDeleteObjectARB(vertex_shader);
	glDeleteObjectARB(fragment_shader);

	//Link the created program.
	/*
	**************
	Programming Tip:
	***************
	You can trace the status of link operation by calling 
	"glGetObjectParameterARB(p,GL_OBJECT_LINK_STATUS_ARB)"
	*/
	glLinkProgramARB(p);


	//Start to use the program object, which is the part of the current rendering state
	glUseProgramObjectARB(p);

	    
	setParameters(p);

}

//Gets the location of the uniform variable given with "name" in the memory
//and tests whether the process was successfull.
//Returns the location of the queried uniform variable
int getUniformVariable(GLuint program,char *name)
{
	int location = glGetUniformLocationARB(program, name);
	
	if (location == -1)
	{
 		error_exit(1007, "No such uniform variable");
	}
	PrintOpenGLError();
	return location;
}

void update_Light_Position()
{
	
	// Create light components
	GLfloat light_position[] = { CameraRadius*cos(CameraTheta)*sin(CameraPhi),			  
			  CameraRadius*cos(CameraPhi) , 
			  CameraRadius*sin(CameraTheta)*sin(CameraPhi),0.0 };
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);
	

}

//Sets the light positions, etc. parameters for the shaders
void setParameters(GLuint program)
{
	int light_loc;
	int ambient_loc,diffuse_loc,specular_loc;
	int exponent_loc;

	//sample variable used to demonstrate how attributes are used in vertex shaders.
	//can be defined as gloabal and can change per vertex
	float tangent = 0.0;
	float tangent_loc;

	update_Light_Position();
	/*
	//Access uniform variables in shaders
	ambient_loc = getUniformVariable(program, "AmbientContribution");	
	glUniform3fvARB(ambient_loc,1, ambient_cont);

	diffuse_loc = getUniformVariable(program, "DiffuseContribution");
	glUniform3fvARB(diffuse_loc,1, diffuse_cont);

	specular_loc = getUniformVariable(program, "SpecularContribution");
	glUniform3fvARB(specular_loc,1,specular_cont);

	exponent_loc = getUniformVariable(program, "exponent");
	glUniform1fARB(exponent_loc,exponent);

	//Access attributes in vertex shader
	tangent_loc = glGetAttribLocationARB(program,"tang");
	glVertexAttrib1fARB(tangent_loc,tangent);
	
	GLint units;
	glGetIntegerv (GL_MAX_TEXTURE_UNITS, &units);
	glActiveTexture (GL_TEXTURE0);*/

}


/****************************************************************
Utility methods:
shader file reader
mesh reader for objectt
****************************************************************/
//Read the shader files, given as parameter.
char *shaderFileRead(char *fn) {


	FILE *fp = fopen(fn,"r");
	if(!fp)
	{
		cout<< "Failed to load " << fn << endl;
		return " ";
	}
	else
	{
		cout << "Successfully loaded " << fn << endl;
	}
	
	char *content = NULL;

	int count=0;

	if (fp != NULL) 
	{
		fseek(fp, 0, SEEK_END);
		count = ftell(fp);
		rewind(fp);

		if (count > 0) 
		{
			content = (char *)malloc(sizeof(char) * (count+1));
			count = fread(content,sizeof(char),count,fp);
			content[count] = '\0';
		}
		fclose(fp);
	}
	return content;
}

void meshReader (char *filename,int sign)
{
  float x,y,z,len;
  int i;
  char letter;
  point v1,v2,crossP;
  int ix,iy,iz;
  int *normCount;
  FILE *fp;

  fp = fopen(filename, "r");
  if (fp == NULL) { 
    printf("Cannot open %s\n!", filename);
    exit(0);
  }

  // Count the number of vertices and faces
  while(!feof(fp))
   {
      fscanf(fp,"%c %f %f %f\n",&letter,&x,&y,&z);
      if (letter == 'v')
	  {
		  verts++;
	  }
      else
	  {
		  faces++;
	  }
   }

  fclose(fp);

  printf("verts : %d\n", verts);
  printf("faces : %d\n", faces);

  // Dynamic allocation of vertex and face lists
  faceList = (faceStruct *)malloc(sizeof(faceStruct)*faces);
  vertList = (point *)malloc(sizeof(point)*verts);
  normList = (point *)malloc(sizeof(point)*verts);

  fp = fopen(filename, "r");

  // Read the veritces
  for(i = 0;i < verts;i++)
    {
      fscanf(fp,"%c %f %f %f\n",&letter,&x,&y,&z);
      vertList[i].x = x;
      vertList[i].y = y;
      vertList[i].z = z;
    }

  // Read the faces
  for(i = 0;i < faces;i++)
    {
      fscanf(fp,"%c %d %d %d\n",&letter,&ix,&iy,&iz);
      faceList[i].v1 = ix - 1;
      faceList[i].v2 = iy - 1;
      faceList[i].v3 = iz - 1;
    }
  fclose(fp);


  // The part below calculates the normals of each vertex
  normCount = (int *)malloc(sizeof(int)*verts);
  for (i = 0;i < verts;i++)
    {
      normList[i].x = normList[i].y = normList[i].z = 0.0;
      normCount[i] = 0;
    }

  for(i = 0;i < faces;i++)
    {
      v1.x = vertList[faceList[i].v2].x - vertList[faceList[i].v1].x;
      v1.y = vertList[faceList[i].v2].y - vertList[faceList[i].v1].y;
      v1.z = vertList[faceList[i].v2].z - vertList[faceList[i].v1].z;
      v2.x = vertList[faceList[i].v3].x - vertList[faceList[i].v2].x;
      v2.y = vertList[faceList[i].v3].y - vertList[faceList[i].v2].y;
      v2.z = vertList[faceList[i].v3].z - vertList[faceList[i].v2].z;

      crossP.x = v1.y*v2.z - v1.z*v2.y;
      crossP.y = v1.z*v2.x - v1.x*v2.z;
      crossP.z = v1.x*v2.y - v1.y*v2.x;

      len = sqrt(crossP.x*crossP.x + crossP.y*crossP.y + crossP.z*crossP.z);

      crossP.x = -crossP.x/len;
      crossP.y = -crossP.y/len;
      crossP.z = -crossP.z/len;

      normList[faceList[i].v1].x = normList[faceList[i].v1].x + crossP.x;
      normList[faceList[i].v1].y = normList[faceList[i].v1].y + crossP.y;
      normList[faceList[i].v1].z = normList[faceList[i].v1].z + crossP.z;
      normList[faceList[i].v2].x = normList[faceList[i].v2].x + crossP.x;
      normList[faceList[i].v2].y = normList[faceList[i].v2].y + crossP.y;
      normList[faceList[i].v2].z = normList[faceList[i].v2].z + crossP.z;
      normList[faceList[i].v3].x = normList[faceList[i].v3].x + crossP.x;
      normList[faceList[i].v3].y = normList[faceList[i].v3].y + crossP.y;
      normList[faceList[i].v3].z = normList[faceList[i].v3].z + crossP.z;
      normCount[faceList[i].v1]++;
      normCount[faceList[i].v2]++;
      normCount[faceList[i].v3]++;
    }
  for (i = 0;i < verts;i++)
    {
      normList[i].x = (float)sign*normList[i].x / (float)normCount[i];
      normList[i].y = (float)sign*normList[i].y / (float)normCount[i];
      normList[i].z = (float)sign*normList[i].z / (float)normCount[i];
    }

}
