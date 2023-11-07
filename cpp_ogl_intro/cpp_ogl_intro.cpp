#include <iostream>
#include <chrono>
#include <vector>
#include <GL/freeglut.h>

float WW=800.0f, WH=600.0f;

struct pt
{
	float x, y;
};

float xmn, ymn, xmx, ymx, cX, cY, sX, sY; //scaling variables
float x1 = 0.0f, x2 = 0.0f, res = 0.0f;

void callbackIdle();
void callbackReshape(int w, int h);
void callbackDisplay();
void genDataVector(std::vector<pt> &cData, float xmin, float xmax, float res);
void makeScalingVars(float x1, float x2);

float triAngle = 0.0f; //triangle rotation angle

std::vector<pt> chartData = {};

//timer class
class in_timer
{
private:
	std::chrono::steady_clock::time_point lastUpdate;

public:
	in_timer()
	{
		lastUpdate = std::chrono::steady_clock::now();
	}
	float dt;
	float getdeltatime()
	{
		auto cm = std::chrono::steady_clock::now();
		auto pm = lastUpdate;

		std::chrono::duration<double, std::micro> fp_ms = cm - pm;

		float dt = fp_ms.count() / 1000000.0f;

		lastUpdate = cm;

		return dt;
	}

} tmr;

void makeScalingVars(float x1, float x2)
{
	xmn = x1;
	xmx = x2;
	sX = (x2 - x1)/2.0f;

	ymn = chartData[0].y;
	ymx = chartData[0].y;

	//find max and min
	for (pt c : chartData)
	{
		if (c.y < ymn) ymn = c.y;
		if (c.y > ymx) ymx = c.y;
	}
	sY = (ymx - ymn) / 2.0f;

	cX = WW / (xmx - xmn);
	cY = WH / (ymx - ymn);

	sX = -(xmn*cX+0.5*WW);
	sY = -(sY*cY-0.5*WH);
}

void genDataVector(std::vector<pt> &cData, float xmin, float xmax, float res)
{
	cData.clear();
	pt cpt = { 0,0 };
	float sx = (xmax - xmin) / res;
	float cx = xmin;
	while (cx <= xmax)
	{
		cpt.x = cx;
		cpt.y = sin((8*cpt.x+10*triAngle)*3.14 / 180);
		cData.push_back(cpt);
		cx += sx;
	}
}


void callbackIdle()
{
	float dt = tmr.getdeltatime();
	triAngle += dt * 10.0f;
	genDataVector(chartData,x1,x2,res);
	callbackDisplay();
}

void callbackReshape(int w, int h)
{
	glViewport(0, 0, w, h);
	WW = w;
	WH = h;
	makeScalingVars(x1, x2);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-(float)w/2.0f, (float)w / 2.0f, -(float)h / 2.0f, (float)h / 2.0f, -10, 10);
	glMatrixMode(GL_MODELVIEW);
}


void callbackDisplay()
{
	glClear(GL_COLOR_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glPushMatrix();
	glLoadIdentity();

	glTranslatef(100.0f*sin(10.0f * triAngle * 3.140f / 180.0f),0.0f,0.0f);
	glRotatef(triAngle,0.0f,0.0f,1.0f);

	glBegin(GL_TRIANGLES);
	glColor3f(1.0f,0.0f,0.0f);
	glVertex2f(-100,-100);
	glColor3f(0.0f, 1.0f, 0.0f);
	glVertex2f( 100, -100);
	glColor3f(0.0f, 0.0f, 1.0f);
	glVertex2f(0, 100);
	glEnd();

	glPopMatrix();

	glPushMatrix();
	glLoadIdentity();

	int l = chartData.size();
	glLineWidth(3);

	glBegin(GL_LINE_STRIP);
	for (int i = 0; i < l; i++)
	{
		float chX = (chartData[i].x*cX + sX);
		float chY = (chartData[i].y*cY + sY);
		//std::cout << i << ") chX = " << chX << "chY = " << chY << std::endl;
		glVertex2f(chX, chY);
	}
	glEnd();

	glPopMatrix();

	glutSwapBuffers();
}


int main(int argc, char **argv)
{
	//input data
	std::cout << "Enter x1, x2, res:";
	std::cin >> x1 >> x2 >> res;

	//generate chart data
	genDataVector(chartData,x1,x2,res);
	makeScalingVars(x1,x2);

	//init glut
	glutInit(&argc, argv);

	//rendering mode
	glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
	
	//setup window
	glutInitWindowSize(trunc(WW), trunc(WH));

	//create window
	glutCreateWindow("GLUT window");

	//setup viewport
	glViewport(0, 0, 800, 600);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-400,400,-300,300,-10,10);
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

	//CALLBACKS FOR MAIN LOOP
	glutIdleFunc(callbackIdle);
	glutReshapeFunc(callbackReshape);
	glutDisplayFunc(callbackDisplay);

	//run main loop
	glutMainLoop();
}