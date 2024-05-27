//========================================================================
// Context sharing example
// Copyright (c) Camilla Löwy <elmindreda@glfw.org>
//
// This software is provided 'as-is', without any express or implied
// warranty. In no event will the authors be held liable for any damages
// arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented; you must not
//    claim that you wrote the original software. If you use this software
//    in a product, an acknowledgment in the product documentation would
//    be appreciated but is not required.
//
// 2. Altered source versions must be plainly marked as such, and must not
//    be misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source
//    distribution.
//
//========================================================================
#include <glad/gl.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <stdio.h>
#include <stdlib.h>

#include "getopt.h"
#include "linmath.h"

#include "MAP_App.h"
#include "MAP_Types.h"
#include "MAP_Mode_Ctrl.h"
#include "MAP_Monitoring.h"
#include "MAP_Param_In.h"
#include "controlPanel.h"


#define EARTH_RADIUS (6371393.0)
#define RAD2DEG (180.0/3.1415926535)
#define NM2DEG (1852.8*RAD2DEG/EARTH_RADIUS)


MAP_Param_From_Inputs_t in[2];     // input params
//extern Int8 fill_mode[WINDOW_NUM];
static GLFWwindow* sg_windows[2];

static void error_callback(int error, const char* description)
{
	fprintf(stderr, "Error: %s\n", description);
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	#define RANGE_NUM (10)
	static float range[RANGE_NUM] = { 2500,5000,10000,20000,40000,80000,160000,240000,400000,750000 };
#if (WINDOW_NUM == 1)
	static short idx[WINDOW_NUM] = { 8 };
#else
	static short idx[WINDOW_NUM] = { 8, 8 };
#endif
	static unsigned char s_winNum = 0;

	if (action == GLFW_PRESS && key == GLFW_KEY_ESCAPE)
		glfwSetWindowShouldClose(window, GLFW_TRUE);

	if (window == sg_windows[0]) { s_winNum = 0; }
	else if (window == sg_windows[1]) { s_winNum = 1; }
	else { return; }

	if (action == GLFW_PRESS) {
		switch (key)
		{
		case GLFW_KEY_LEFT:
			in[s_winNum].longitude -= 0.01 * (NM2DEG * in[s_winNum].ring_range) / 1000.0;
			break;
		case GLFW_KEY_RIGHT:
			in[s_winNum].longitude += 0.01 * (NM2DEG * in[s_winNum].ring_range) / 1000.0;
			break;
		case GLFW_KEY_UP:
			in[s_winNum].latitude += 0.01 * (NM2DEG * in[s_winNum].ring_range) / 1000.0;
			break;
		case GLFW_KEY_DOWN:
			in[s_winNum].latitude -= 0.01 * (NM2DEG * in[s_winNum].ring_range) / 1000.0;
			break;
		case GLFW_KEY_Q:
			in[s_winNum].heading -= 0.5;
			break;
		case GLFW_KEY_E:
			in[s_winNum].heading += 0.5;
			break;
		case GLFW_KEY_R:
			in[s_winNum].longitude = 121.0;
			in[s_winNum].latitude = 31.0;
			break;
		case GLFW_KEY_L:
			idx[s_winNum]++;
			if (idx[s_winNum] >= RANGE_NUM)
			{
				idx[s_winNum] = RANGE_NUM - 1;
			}
			idx[s_winNum] %= RANGE_NUM;
			in[s_winNum].ring_range = range[idx[s_winNum]];
			break;
		case GLFW_KEY_K:
			idx[s_winNum]--;
			if (idx[s_winNum] <= 0)
			{
				idx[s_winNum] = 0;
			}
			idx[s_winNum] %= RANGE_NUM;
			in[s_winNum].ring_range = range[idx[s_winNum]];
			break;
		case GLFW_KEY_A:
			in[s_winNum].longitude -= (NM2DEG * in[s_winNum].ring_range) / 1000.0;
			break;
		case GLFW_KEY_D:
			in[s_winNum].longitude += (NM2DEG * in[s_winNum].ring_range) / 1000.0;
			break;
		case GLFW_KEY_W:
			in[s_winNum].latitude += (NM2DEG * in[s_winNum].ring_range) / 1000.0;
			break;
		case GLFW_KEY_S:
			in[s_winNum].latitude -= (NM2DEG * in[s_winNum].ring_range) / 1000.0;
			break;
		case GLFW_KEY_F:
			//fill_mode[s_winNum] = !fill_mode[s_winNum];
			break;
		default:
			break;
		}
	}
}

DWORD WINAPI MyThreadFunction(LPVOID lpParam)
{
	for (;;) {
		App_Update_Data_Background();
		Sleep(0.1);
	}
}

HANDLE CreateBackgoundThread() {
	DWORD  dwThreadId;
	HANDLE hThread;

	hThread = CreateThread(NULL,              // default security attributes
		0,                 // use default stack size
		MyThreadFunction,  // thread function name
		NULL,              // argument to thread function
		0,                 // use default creation flags
		&dwThreadId);      // returns the thread identifier

	if (hThread == NULL) {
		printf("Create thread error!\n");
	}

	return hThread;
}

#include "OpenGLFrameBuffer.h"
int main(int argc, char** argv)
{
	glfwSetErrorCallback(error_callback);

	if (!glfwInit())
		exit(EXIT_FAILURE);

	GLFWwindow* window = glfwCreateWindow(Config_Get_Screen_Width()*2 + 200, Config_Get_Screen_Height(), "MAP", nullptr, nullptr);
	if (!window) {
		glfwTerminate();
		exit(EXIT_FAILURE);
	}
	glfwMakeContextCurrent(window);

	gladLoadGL(glfwGetProcAddress);

	ControlPanel controlPanel;
	controlPanel.init(window, 2);

	OpenGLFrameBuffer openGLFrameBuffer[2];
	openGLFrameBuffer[0].createBuffers(Config_Get_Screen_Width(), Config_Get_Screen_Height());
	openGLFrameBuffer[1].createBuffers(Config_Get_Screen_Width(), Config_Get_Screen_Height());
	
	Param_In_Init(0, controlPanel.getMapIn(0));
	Param_In_Init(1, controlPanel.getMapIn(1));

	if (App_Init() != MAP_OK) {
		printf("MAP Init Error");
		return -1;
	}

	CreateBackgoundThread();

	while (!glfwWindowShouldClose(window)) {
		glfwMakeContextCurrent(window);

		glViewport(0, 0, Config_Get_Screen_Width()*2, Config_Get_Screen_Height());
		glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		//glDisable(GL_DEPTH_TEST);

		controlPanel.preRender();

		Param_In_Acquiring(0);
		openGLFrameBuffer[0].bind();
		App_Draw(0);
		openGLFrameBuffer[0].unBind();
		controlPanel.render(0, openGLFrameBuffer[0].getTexture(), Config_Get_Screen_Width(), Config_Get_Screen_Height());
		
		Param_In_Acquiring(1);
		openGLFrameBuffer[1].bind();
		App_Draw(1);
		openGLFrameBuffer[1].unBind();
		controlPanel.render(1, openGLFrameBuffer[1].getTexture(), Config_Get_Screen_Width(), Config_Get_Screen_Height());

		controlPanel.postRender();

		glfwPollEvents();
		glfwSwapBuffers(window);
		Sleep(0.1);
	}

#if 0
	GLuint texture, program, vertex_buffer;
	GLint mvp_location, vpos_location, color_location, texture_location;

	ControlPanel controlPanel[2];

	//MAP_Param_From_Inputs_t* cpIn = (MAP_Param_From_Inputs_t*)controlPanel->init();

	glfwSetErrorCallback(error_callback);

	if (!glfwInit())
		exit(EXIT_FAILURE);

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

	sg_windows[0] = glfwCreateWindow(Config_Get_Screen_Width(), Config_Get_Screen_Height(), "First", NULL, NULL);
	if (!sg_windows[0])
	{
		glfwTerminate();
		exit(EXIT_FAILURE);
	}
	glfwSetKeyCallback(sg_windows[0], key_callback);
	glfwMakeContextCurrent(sg_windows[0]);

	// Only enable vsync for the first of the windows to be swapped to
	// avoid waiting out the interval for each window
	glfwSwapInterval(1);

	// The contexts are created with the same APIs so the function
	// pointers should be re-usable between them
	gladLoadGL(glfwGetProcAddress);

	controlPanel[0].init(sg_windows[0], "DMAP1");

	in[0].longitude = 121.793333333948;// 103.5; //-95.2436; //-96.94097222222223; //37.598875; //8.5 * 0.853333333333333333;
	in[0].latitude = 31.1449999998139;// 31.0; //29.5204;  //33.630427777777776; //55.834456; //68.5 * 0.853333333333333333;
	in[0].altitude = 2000.0;
	in[0].pitch = -90.0;
	in[0].heading = 0.0;
	in[0].roll = 0.0;
	in[0].ring_range = 100.0 * 1000.0;
	in[0].map_mode = 0;

	Param_In_Init(0, controlPanel[0].getMapIn());


#if (WINDOW_NUM == 2)
	//sg_windows[1] = glfwCreateWindow(Config_Get_Screen_Width(), Config_Get_Screen_Width(), "Second", NULL, NULL);
	sg_windows[1] = glfwCreateWindow(Config_Get_Screen_Width(), Config_Get_Screen_Height(), "Second", NULL, sg_windows[0]);
	if (!sg_windows[1])
	{
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	// Place the second window to the right of the first
	{
		int xpos, ypos, left, right, width;

		glfwGetWindowSize(sg_windows[0], &width, NULL);
		glfwGetWindowFrameSize(sg_windows[0], &left, NULL, &right, NULL);
		glfwGetWindowPos(sg_windows[0], &xpos, &ypos);


		glfwSetWindowPos(sg_windows[1], xpos + width + left + right, ypos);
	}

	glfwSetKeyCallback(sg_windows[1], key_callback);
	glfwMakeContextCurrent(sg_windows[1]);
	glfwSwapInterval(1);

	controlPanel[1].init(sg_windows[1], "DMAP2");

	in[1].longitude = 121.793333333948;// 103.5; //-95.2436; //-96.94097222222223; //37.598875; //8.5 * 0.853333333333333333;
	in[1].latitude = 31.1449999998139;// 31.0; //29.5204;  //33.630427777777776; //55.834456; //68.5 * 0.853333333333333333;
	in[1].altitude = 2000.0;
	in[1].pitch = -90.0;
	in[1].heading = 0.0;
	in[1].roll = 0.0;
	in[1].ring_range = 100.0 * 1000.0;
	in[1].map_mode = 0;

	Param_In_Init(1, controlPanel[1].getMapIn());
#endif

	glfwMakeContextCurrent(sg_windows[0]);

	// init map 1
	if (App_Init() != MAP_OK)
	{
		printf("MAP [0] Init Error");
	}

	// While objects are shared, the global context state is not and will
	// need to be set up for each context

	CreateBackgoundThread();


#if 1
#if (WINDOW_NUM==1)
	while (!glfwWindowShouldClose(sg_windows[0]))
#else
	while (!glfwWindowShouldClose(sg_windows[0]) &&
		!glfwWindowShouldClose(sg_windows[1]))
#endif
	{
		glfwPollEvents();

		for (int i = 0; i < WINDOW_NUM; i++)
		{
			//glfwWaitEventsTimeout(1);

			glfwMakeContextCurrent(sg_windows[i]);

			Param_In_Acquiring(i);

			App_Draw(i);

			controlPanel[i].render();

			glfwSwapBuffers(sg_windows[i]);	
			//glfwPollEvents();
		}

		//glfwPollEvents();
	}
#endif

	glfwTerminate();
	exit(EXIT_SUCCESS);
#endif
}
