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

#define PI 3.14159265359

struct Vertex
{
    float x;
    float y;
    float z;
    float w;
};

struct Face
{
    int p1;
    int p2;
    int p3;
};

struct Mesh
{
    std::vector<Vertex> vertices;
    std::vector<Face> faces;
};

void glInit();
void reshape(int width, int height);
void setupMVPMatrices();
void display();
void readInVertexData(char *filename, std::vector<Vertex> &vertexList);
void generateDiscreteProfiles(std::vector<std::vector<Vertex> > &profileList, std::vector<Vertex> &originalProfile,
			      Mesh &mesh);
void calculateMeshFaces(Mesh &mesh, int profileLength);

static Mesh displayMesh;

int main(int argc, char* argv[])
{

    glutInit(&argc, argv);
    glInit();

    std::vector<Vertex> verts;
    std::vector<std::vector<Vertex> > profiles;

    Mesh vaseMesh;

    readInVertexData("vase.txt", verts);
    generateDiscreteProfiles(profiles, verts, vaseMesh);
    calculateMeshFaces(vaseMesh, verts.size());

    displayMesh = vaseMesh;

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

    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

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
    glOrtho(-VIEWPORT_EXTENT, VIEWPORT_EXTENT, -VIEWPORT_EXTENT, VIEWPORT_EXTENT, -VIEWPORT_EXTENT, VIEWPORT_EXTENT);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

}

void display()
{
    glClear(GL_COLOR_BUFFER_BIT);

    glPushMatrix();
    glRotatef(-90.0, 1.0, 0.0, 0.0);

    glBegin(GL_TRIANGLES);
    
    for (int i = 0; i < displayMesh.faces.size(); i++)
    {
	Vertex p1 = displayMesh.vertices[displayMesh.faces[i].p1];
	Vertex p2 = displayMesh.vertices[displayMesh.faces[i].p2];
	Vertex p3 = displayMesh.vertices[displayMesh.faces[i].p3];

	glVertex4f(p1.x, p1.y, p1.z, p1.w);
	glVertex4f(p2.x, p2.y, p2.z, p2.w);
	glVertex4f(p3.x, p3.y, p3.z, p3.w);
    } 

    glEnd();
    glPopMatrix();
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

void generateDiscreteProfiles(std::vector<std::vector<Vertex> > &profileList, std::vector<Vertex> &originalProfile, Mesh &mesh)
{

    int numberOfProfiles = 360 / DEGREE_INCREMENT;

    profileList.push_back(originalProfile);

    for (int i = 0; i < originalProfile.size(); i++)
    {
	mesh.vertices.push_back(originalProfile[i]);
    }

    for (int i = 1; i < numberOfProfiles; i++)
    {

	float angle = (i * DEGREE_INCREMENT) * (PI / 180.0);
	std::vector<Vertex> profile;

	for (int i = 0; i < originalProfile.size(); i++)
	{
	    Vertex vert;

	    vert.x = (originalProfile[i].x * cos(angle)) - (originalProfile[i].y * sin(angle));
	    vert.y = (originalProfile[i].x * sin(angle)) + (originalProfile[i].y * cos(angle));
	    vert.z = originalProfile[i].z;
	    vert.w = originalProfile[i].w;

	    profile.push_back(vert);
	    mesh.vertices.push_back(vert);
	}

	profileList.push_back(profile);

    }
}

void calculateMeshFaces(Mesh &mesh, int profileLength)
{
    int numberOfProfiles = 360 / DEGREE_INCREMENT;

    int anchor = 0;
    int wrapAroundProfile = numberOfProfiles-1;

    // Do the triangulation in 2 triangle "chunks"
    for (int i = 0; i <= wrapAroundProfile; i++)
    {

	for (int j = 0; j < profileLength - 1; j++)
	{

	    Face face1;
	    Face face2;

	    face1.p1 = anchor;
	    face1.p2 = anchor + profileLength;
	    face1.p3 = anchor + 1;

	    face2.p1 = anchor + profileLength;
	    face2.p2 = anchor + 1;
	    face2.p3 = anchor + profileLength + 1;

	    if (i == wrapAroundProfile)
	    {
		face1.p2 %= profileLength;
		face2.p1 %= profileLength;
		face2.p3 %= profileLength;
	    }
	    

	    mesh.faces.push_back(face1);
	    mesh.faces.push_back(face2);

	    anchor++;
	}

	anchor++;
	
    }

}
