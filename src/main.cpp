#include <OpenGL/gl.h>
#include <GLUT/glut.h>

void Render()
{
	glClear(GL_COLOR_BUFFER_BIT);
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(1024, 768);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
	glutCreateWindow("ERM");
	
	glutDisplayFunc(Render);
	
	glutMainLoop();
	
	return 0;
}
