#ifdef __APPLE__
  #include <GLUT/glut.h>
#else
  #include <GL/glut.h>
#endif

#include <stdio.h>
#include <vector>

#define SCREEN_WIDTH 600
#define SCREEN_HEIGHT 600
#define WIN_STARTX 100
#define WIN_STARTY 100

#define VIEWPORT_EXTENT 50

struct Point
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
void readInPointData(char *filename, std::vector<Point> &pointList);

int main(int argc, char* argv[])
{
    glutInit(&argc, argv);
    glInit();

    std::vector<Point> points;

    readInPointData("vase.txt", points);

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

void readInPointData(char *filename, std::vector<Point> &pointList)
{
    FILE *fp;
    fp = fopen(filename, "r");

    if (fp)
    {

        Point point;

	while(fscanf(fp, "%f %f %f %f\n", &point.x, &point.y, &point.z, &point.w) != EOF)
	{
	    pointList.push_back(point);
	}

	if(!fclose(fp))
	{
	    return;
	}
	else {
	    printf("Problem closing the file.");
	}
    }
    else
    {
	printf("Problem opening the file.");
    }

}
