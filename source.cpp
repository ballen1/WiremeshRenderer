#ifdef __APPLE__
  #include <GLUT/glut.h>
#else
  #include <GL/glut.h>
#endif

#include <stdio.h>
#include <math.h>
#include <vector>

#define SCREEN_WIDTH 600
#define SCREEN_HEIGHT 600
#define WIN_STARTX 100
#define WIN_STARTY 100

#define VIEWPORT_EXTENT 50

#define DEGREE_INCREMENT 2

struct Vertex
{
  float x;
  float y;
  float z;
  float w;
};

void glInit();
void reshape(int width, int height);
void setupMVPMatrices();
void display();
void readInVertexData(char *filename, std::vector<Vertex> &vertexList);
void generateDiscreteProfiles(std::vector<std::vector<Vertex> > &profileList, std::vector<Vertex> &originalProfile);

int main(int argc, char* argv[])
{

    glutInit(&argc, argv);
    glInit();

    std::vector<Vertex> verts;
    std::vector<std::vector<Vertex> > profiles;

    readInVertexData("vase.txt", verts);
    generateDiscreteProfiles(profiles, verts);

    glutMainLoop();

    return(0);
}

void glInit()
{
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(SCREEN_WIDTH, SCREEN_HEIGHT);
    glutInitWindowPosition(WIN_STARTX, WIN_STARTY);

    glutCreateWindow("Assignment 2");

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    glutReshapeFunc(reshape);
    glutDisplayFunc(display);

}

void reshape(int width, int height)
{

  glViewport(0, 0, width, height);
  setupMVPMatrices();

}


void setupMVPMatrices()
{

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(-VIEWPORT_EXTENT, VIEWPORT_EXTENT, -VIEWPORT_EXTENT, VIEWPORT_EXTENT, -1.0, 1.0);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

}

void display()
{
    glClear(GL_COLOR_BUFFER_BIT);
    glutSwapBuffers();
}

void readInVertexData(char *filename, std::vector<Vertex> &vertexList)
{
    FILE *fp;
    fp = fopen(filename, "r");

    if (fp)
    {

         Vertex vertex;

	       while(fscanf(fp, "%f %f %f %f\n", &vertex.x, &vertex.y, &vertex.z, &vertex.w) != EOF)
	       {
	           vertexList.push_back(vertex);
	       }

	       if(!fclose(fp))
	       {
	         return;
	       }
	       else {
	         printf("Problem closing the file.\n");
	       }
    }
    else
    {
	       printf("Problem opening the file.\n");
    }

}

void generateDiscreteProfiles(std::vector<std::vector<Vertex> > &profileList, std::vector<Vertex> &originalProfile)
{

  int numberOfProfiles = 360 / DEGREE_INCREMENT;

  profileList.push_back(originalProfile);

  for (int i = 1; i < numberOfProfiles; i++)
  {

    float angle = (i * DEGREE_INCREMENT) * (M_PI / 180.0);
    std::vector<Vertex> profile;

    for (int i = 0; i < originalProfile.size(); i++)
    {
      Vertex vert;

      vert.x = (originalProfile[i].x * cos(angle)) - (originalProfile[i].y * sin(angle));
      vert.y = (originalProfile[i].x * sin(angle)) + (originalProfile[i].y * cos(angle));
      vert.z = originalProfile[i].z;
      vert.w = originalProfile[i].w;

      profile.push_back(vert);
    }

    profileList.push_back(profile);

  }
}