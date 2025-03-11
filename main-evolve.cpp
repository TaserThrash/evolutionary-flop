/**************************
 * Includes
 *
 **************************/

#include <windows.h>
#include <gl/gl.h>
#include <vector>
#include <bits/stdc++.h>
#include <cmath>

using namespace std;


/**************************
 * Function Declarations
 *
 **************************/

LRESULT CALLBACK WndProc (HWND hWnd, UINT message,
WPARAM wParam, LPARAM lParam);
void EnableOpenGL (HWND hWnd, HDC *hDC, HGLRC *hRC);
void DisableOpenGL (HWND hWnd, HDC hDC, HGLRC hRC);

/*
			glPushMatrix ();
            glRotatef (theta, 0.0f, 0.0f, 1.0f);
            glBegin (GL_TRIANGLES);
            glColor3f (1.0f, 0.0f, 0.0f);   glVertex2f (0.0f, 1.0f);
            glColor3f (0.0f, 1.0f, 0.0f);   glVertex2f (0.87f, -0.5f);
            glColor3f (0.0f, 0.0f, 1.0f);   glVertex2f (-0.87f, -0.5f);
            glEnd ();
            glPopMatrix ();
*/

class Plant{
	public:
		
	float x, y, a;
	
	// x, y, speed, vision, turn
	Plant(float ax, float ay){
		x = ax;
		y = ay;
		a = rand() % 20;
		
		x = max(-1.0f, min(1.0f, ax));
    	y = max(-1.0f, min(1.0f, ay));
	}
		
	
	void show(){
		glPushMatrix();
		glTranslatef(x, y, 0);
		glRotatef(0, 0, 0, 1); // Convert radians to degrees
		glBegin(GL_QUADS);
		glColor3f(0.0, 1.0, 0.0);
		glVertex2f(-0.05, -0.05);
		glVertex2f(0.05, -0.05);
		glVertex2f(0.05, 0.05);
		glVertex2f(-0.05, 0.05);
		glEnd();
		glPopMatrix();
	}
	
	void update(){
		a++;
	}
};

vector<Plant> plants;

class Prey{
	public:
		
	float x, y, s, v, t, r, f, p;
	//sees food
	bool sF = false;
	
	// x, y, speed, vision, turn
	Prey(float ax, float ay, float as, float av, float at){
		x = ax;
		y = ay;
		s = as;
		v = av;
		t = at;
		r = 0;
		f = 20;
		p = -1;
	}
	
	void show(){
		glPushMatrix();
		glTranslatef(x, y, 0);
		glRotatef(r * 180.0 / M_PI, 0, 0, 1); // Convert radians to degrees
		glBegin(GL_QUADS);
		glColor3f(0.0, 0.0, 1.0);
		glVertex2f(-0.05, -0.05);
		glVertex2f(0.05, -0.05);
		glVertex2f(0.05, 0.05);
		glVertex2f(-0.05, 0.05);
		glEnd();
		glPopMatrix();
	}
	
	void update(){
		x += cos(r) * s;
		y += sin(r) * s;
		
		if(!sF){
			r += t - (rand() % 2) * 2 * t;
		}
		
		f -= (v + s + t) / 3;
		
		if(abs(x) >= 1){
			x -= 2 * x;
		}
		if(abs(y) >= 1){
			y -= 2 * y;
		}
		
		p--;
	}
	
	void seePlant(){
		for(int i = 0; i < plants.size(); i++){
			if(sqrt(pow(plants[i].x - x, 2) + pow(plants[i].y - y, 2)) < v){
				sF = true;
				r = atan2(plants[i].y - y, plants[i].x - x);
			}
		}
	}
	
	void eat(){
		for(int i = 0; i < plants.size(); i++){
			if(abs(plants[i].x - x) < 0.1 && abs(plants[i].y - y) < 0.1){
				f += sqrt(plants[i].a / 4);
				plants.erase(plants.begin() + i);
				i--;
			}
		}
	}
};

vector<Prey> prey;


class Predator{
	public:
		
	float x, y, s, v, t, r, f, p;
	//sees food
	bool sF = false;
	
	// x, y, speed, vision, turn
	Predator(float ax, float ay, float as, float av, float at){
		x = ax;
		y = ay;
		s = as;
		v = av;
		t = at;
		r = 0;
		f = 20;
		p = -1;
	}
	
	void show(){
		glPushMatrix();
		glTranslatef(x, y, 0);
		glRotatef(r * 180.0 / M_PI, 0, 0, 1); // Convert radians to degrees
		glBegin(GL_QUADS);
		glColor3f(1.0, 0.0, 0.0);
		glVertex2f(-0.05, -0.05);
		glVertex2f(0.05, -0.05);
		glVertex2f(0.05, 0.05);
		glVertex2f(-0.05, 0.05);
		glEnd();
		glPopMatrix();
	}
	
	void update(){
		if(!sF){
			r += t - (rand() % 2) * 2 * t;
		}
		
		x += cos(r) * s;
		y += sin(r) * s;
		
		f -= v + s;
		
		if(abs(x) >= 1){
			x -= 2 * x;
		}
		if(abs(y) >= 1){
			y -= 2 * y;
		}
		
		p--;
	}
	
	void seePrey(){
		for(int i = 0; i < prey.size(); i++){
			if(sqrt(pow(prey[i].x - x, 2) + pow(prey[i].y - y, 2)) < v){
				sF = true;
				r = atan2(prey[i].y - y, prey[i].x - x);
			}
		}
	}
	
	void eat(){
		for(int i = 0; i < prey.size(); i++){
			if(abs(prey[i].x - x) < 0.1 && abs(prey[i].y - y) < 0.1){
				f += sqrt(prey[i].f / 16);
				prey.erase(prey.begin() + i);
				break;
			}
		}
	}
};

vector<Predator> predators;


/*
	*#*#*#*#*#*#*#
	BEGINNING
	#*#*#*#*#*#*#*
*/

void init(){
	srand(time(0));
	
	for(int i = 0; i < 5; i++){
		prey.push_back(Prey(
			float(rand() % 2000) / 1000 - 1,
			float(rand() % 2000) / 1000 - 1,
			float(rand() % 1000) / 10000,
			float(rand() % 1000) / 1000,
			float(rand() % 10000) / 10000));
		plants.push_back(Plant(
			float(rand() % 2000) / 1000 - 1,
			float(rand() % 2000) / 1000 - 1));
		predators.push_back(Predator(
			float(rand() % 2000) / 1000 - 1,
			float(rand() % 2000) / 1000 - 1,
			float(rand() % 1000) / 10000,
			float(rand() % 1000) / 1000,
			float(rand() % 10000) / 10000));
	}
}

/*
	*#*#*#*#*#*#*#
	MAIN LOOP
	#*#*#*#*#*#*#*
*/

int f = 0;

Prey flee(Prey p) {
    float totalX = 0, totalY = 0;
    int predatorCount = 0;
    
    for(int i = 0; i < predators.size(); i++) {
        float dist = sqrt(pow(predators[i].x - p.x, 2) + pow(predators[i].y - p.y, 2));
        if(dist < p.v) {
            // Add vector away from predator
            totalX += (p.x - predators[i].x) / dist;  // Normalize by distance
            totalY += (p.y - predators[i].y) / dist;
            predatorCount++;
        }
    }
    
    if(predatorCount > 0) {
        p.r = atan2(totalY, totalX);  // Average flee direction
        p.sF = false;
    }
    return p;
}

void run(){
	for (int i = 0; i < prey.size(); i++) {
	    prey[i].update();
	    prey[i].show();
	    prey[i].seePlant();
	    prey[i].eat();
	    prey[i] = flee(prey[i]);
	    if (prey[i].f <= 0) {
	        prey.erase(prey.begin() + i);
	        i--; // Prevent skipping next element
	    }
	    
	    if (prey[i].f > 30 && prey[i].p < 0) {
            prey[i].p = 0;
            prey[i].f -= 20;
	    }
	    
	    if (prey[i].p == 0) {
	    	Prey p = prey[i];
	        prey.push_back(Prey(
				p.x,
				p.y,
				p.s + float(rand() % 1000) / 2500000 - 1000 / 5000000,
				p.v + float(rand() % 1000) / 25000 - 1000 / 50000,
				p.t + float(rand() % 10000) / 250000 - 1000 / 500000));
	    }
	}
	
	for (int i = 0; i < predators.size(); i++) {
	    predators[i].update();
	    predators[i].show();
	    predators[i].seePrey();
	    predators[i].eat();
	    if (predators[i].f <= 0) {
	        predators.erase(predators.begin() + i);
	        i--; // Prevent skipping next element
	    }
	    
	    if (predators[i].f > 30) {
	    	predators[i].p = 0;
            predators[i].f -= 20;
	    }
	    
	    if (predators[i].p == 0) {
	    	Predator p = predators[i];
	        predators.push_back(Predator(
				p.x,
				p.y,
				p.s + float(rand() % 1000) / 5000000,
				p.v + float(rand() % 1000) / 50000,
				p.t + float(rand() % 10000) / 500000));
			p.f -= 20;
	    }
	}
	
	for (int i = 0; i < plants.size(); i++) {
	    plants[i].update();
	    plants[i].show();
	    
	    if(int(plants[i].a) % 40 == 0 && rand() % 100 < 100){
			plants.push_back(Plant(
				plants[i].x + sin(float(rand() % 1000) / 100) / 6,
				plants[i].y + cos(float(rand() % 1000) / 100) / 6
			));
			
			float x = plants[plants.size() - 1].x;
			float y = plants[plants.size() - 1].y;
			
			if(abs(x) > 1 || abs(y) > 1){
				plants.erase(plants.begin() + i);
				i--;
				continue;
			}
			
			for(int i = 0; i < plants.size() - 1; i++){
				if(abs(plants[i].x - x) < 0.1 && abs(plants[i].y - y) < 0.1){
					plants.erase(plants.begin() + i);
					i--;
				}
			}
		}
		
		if(int(plants[i].a) > 120){
			plants.erase(plants.begin() + i);
			i--;
		}
	}
	
	f++;
	if(f % 40 == 0){
		plants.push_back(Plant(
			float(rand() % 2000) / 1000 - 1,
			float(rand() % 2000) / 1000 - 1));
	}
	
	if(f % 60 == 0){
		prey.push_back(Prey(
			float(rand() % 2000) / 1000 - 1,
			float(rand() % 2000) / 1000 - 1,
			float(rand() % 1000) / 10000,
			float(rand() % 1000) / 1000,
			float(rand() % 10000) / 10000));
	}
	if(predators.size() == 0){
		predators.push_back(Predator(
			float(rand() % 2000) / 1000 - 1,
			float(rand() % 2000) / 1000 - 1,
			float(rand() % 1000) / 10000,
			float(rand() % 1000) / 1000,
			float(rand() % 10000) / 10000));
	}
};


/**************************
 * WinMain
 *
 **************************/

int WINAPI WinMain (HINSTANCE hInstance,
                    HINSTANCE hPrevInstance,
                    LPSTR lpCmdLine,
                    int iCmdShow)
{
    WNDCLASS wc;
    HWND hWnd;
    HDC hDC;
    HGLRC hRC;        
    MSG msg;
    BOOL bQuit = FALSE;
    float theta = 0.0f;
    bool sleep = true;
    bool space = false;

    /* register window class */
    wc.style = CS_OWNDC;
    wc.lpfnWndProc = WndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hInstance;
    wc.hIcon = LoadIcon (NULL, IDI_APPLICATION);
    wc.hCursor = LoadCursor (NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH) GetStockObject (BLACK_BRUSH);
    wc.lpszMenuName = NULL;
    wc.lpszClassName = "GLSample";
    RegisterClass (&wc);

    /* create main window */
    hWnd = CreateWindow (
      "GLSample", "OpenGL Sample", 
      WS_CAPTION | WS_POPUPWINDOW | WS_VISIBLE,
      0, 0, 256, 256,
      NULL, NULL, hInstance, NULL);

    /* enable OpenGL for the window */
    EnableOpenGL (hWnd, &hDC, &hRC);
    
    init();

    /* program main loop */
    while (!bQuit)
    {
        /* check for messages */
        if (PeekMessage (&msg, NULL, 0, 0, PM_REMOVE))
        {
            /* handle or dispatch messages */
            if (msg.message == WM_QUIT)
            {
                bQuit = TRUE;
            }
            else
            {
                TranslateMessage (&msg);
                DispatchMessage (&msg);
            }
        }
        else
        {
            /* OpenGL animation code goes here */

            glClearColor (0.0f, 0.0f, 0.0f, 0.0f);
            glClear (GL_COLOR_BUFFER_BIT);

            run();

            SwapBuffers (hDC);
            
            if(GetAsyncKeyState(VK_SPACE)){
            	if(!space){
            		sleep = !sleep;
				}
				
				space = true;
			} 
			else{
				space = false;
			}

            if(sleep){
				Sleep (1000 / 60);
			}
        }
    }

    /* shutdown OpenGL */
    DisableOpenGL (hWnd, hDC, hRC);

    /* destroy the window explicitly */
    DestroyWindow (hWnd);

    return msg.wParam;
}


/********************
 * Window Procedure
 *
 ********************/

LRESULT CALLBACK WndProc (HWND hWnd, UINT message,
                          WPARAM wParam, LPARAM lParam)
{

    switch (message)
    {
    case WM_CREATE:
        return 0;
    case WM_CLOSE:
        PostQuitMessage (0);
        return 0;

    case WM_DESTROY:
        return 0;

    case WM_KEYDOWN:
        switch (wParam)
        {
        case VK_ESCAPE:
            PostQuitMessage(0);
            return 0;
        }
        return 0;

    default:
        return DefWindowProc (hWnd, message, wParam, lParam);
    }
}


/*******************
 * Enable OpenGL
 *
 *******************/

void EnableOpenGL (HWND hWnd, HDC *hDC, HGLRC *hRC)
{
    PIXELFORMATDESCRIPTOR pfd;
    int iFormat;

    /* get the device context (DC) */
    *hDC = GetDC (hWnd);

    /* set the pixel format for the DC */
    ZeroMemory (&pfd, sizeof (pfd));
    pfd.nSize = sizeof (pfd);
    pfd.nVersion = 1;
    pfd.dwFlags = PFD_DRAW_TO_WINDOW | 
      PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 24;
    pfd.cDepthBits = 16;
    pfd.iLayerType = PFD_MAIN_PLANE;
    iFormat = ChoosePixelFormat (*hDC, &pfd);
    SetPixelFormat (*hDC, iFormat, &pfd);

    /* create and enable the render context (RC) */
    *hRC = wglCreateContext( *hDC );
    wglMakeCurrent( *hDC, *hRC );

}


/******************
 * Disable OpenGL
 *
 ******************/

void DisableOpenGL (HWND hWnd, HDC hDC, HGLRC hRC)
{
    wglMakeCurrent (NULL, NULL);
    wglDeleteContext (hRC);
    ReleaseDC (hWnd, hDC);
}
