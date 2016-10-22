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

#define VIEWPORT_EXTENT 35

#define DEGREE_INCREMENT 2

#define PI 3.14159265359

struct Vertex
{
    float x;
    float y;
    float z;
    float w;
};

struct Vector3 {
    float x;
    float y;
    float z;
};

struct Face
{
    int p1;
    int p2;
    int p3;
    Vector3 normal;
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
void calculateNormals(Mesh &mesh);
Vector3 Vector3From2Points(Vertex p1, Vertex p2);

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
    calculateNormals(vaseMesh);

    displayMesh = vaseMesh;

    glutMainLoop();

    return(0);
}

void glInit()
{
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(SCREEN_WIDTH, SCREEN_HEIGHT);
    glutInitWindowPosition(WIN_STARTX, WIN_STARTY);

    glutCreateWindow("Assignment 2");

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glShadeModel(GL_SMOOTH);

    // Lighting and Materials

    GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
    GLfloat mat_shininess[] = { 50.0 };
    GLfloat light_position[] = { 1.0, 1.0, 1.0, 0.0 };
    GLfloat light1_position[] = {0.0, 0.0, -1.0, 0.0};
    
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, mat_shininess);
    glLightfv(GL_LIGHT0, GL_POSITION, light1_position);
   
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_DEPTH_TEST);

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
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glPushMatrix();
    glRotatef(-90.0, 1.0, 0.0, 0.0);


    glColor3f(1.0, 0.0, 0.0);

    glBegin(GL_TRIANGLES);
    
    for (int i = 0; i < displayMesh.faces.size(); i++)
    {
	Vertex p1 = displayMesh.vertices[displayMesh.faces[i].p1];
	Vertex p2 = displayMesh.vertices[displayMesh.faces[i].p2];
	Vertex p3 = displayMesh.vertices[displayMesh.faces[i].p3];
	
	Vector3 normal = displayMesh.faces[i].normal;

	glNormal3f(normal.x, normal.y, normal.z);
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

	    face2.p2 = anchor + profileLength;
	    face2.p3 = anchor + profileLength + 1;
	    face2.p1 = anchor + 1;

	    if (i == wrapAroundProfile)
	    {
		face1.p2 %= profileLength;
		face2.p3 %= profileLength;
		face2.p2 %= profileLength;
	    }
	    

	    mesh.faces.push_back(face1);
	    mesh.faces.push_back(face2);

	    anchor++;
	}

	anchor++;
	
    }

}

void calculateNormals(Mesh &mesh)
{
    for (int i = 0; i < mesh.faces.size(); i++)
    {
	Vertex p3 = mesh.vertices[mesh.faces[i].p3];

	Vector3 v2 = Vector3From2Points(p3, mesh.vertices[mesh.faces[i].p1]);
	Vector3 v1 = Vector3From2Points(p3, mesh.vertices[mesh.faces[i].p2]);

	Vector3 crossProd;

	crossProd.x = (v1.y * v2.z) - (v1.z * v2.y);
	crossProd.y = (v1.z * v2.x) - (v1.x * v2.z);
	crossProd.z = (v1.x * v2.y) - (v1.y * v2.x);

	float crossProdMagnitude = sqrt((crossProd.x*crossProd.x) + (crossProd.y*crossProd.y) + (crossProd.z*crossProd.z));

	crossProd.x /= crossProdMagnitude;
	crossProd.y /= crossProdMagnitude;
	crossProd.z /= crossProdMagnitude;

	mesh.faces[i].normal = crossProd;

    }
}

Vector3 Vector3From2Points(Vertex p1, Vertex p2)
{
    Vector3 vec;

    vec.x = p2.x - p1.x;
    vec.y = p2.y - p1.y;
    vec.z = p2.z - p1.z;

    return vec;
}
