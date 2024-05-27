#pragma once

#include <iostream>
#include <vector>
#include <GLFW/glfw3.h>
#include <assert.h>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "MAP_Param_In.h"

#define RANGE_NUM 10

class MapIn
{
public:
	MapIn(std::string p_mapName);

	MAP_Param_From_Inputs_t* getMapIn()
	{
		return &_mapIn;
	}
	const std::string &getmapName()
	{
		return _mapName;
	}
	void setMapMode(bool p_isRoam)
	{
		if (p_isRoam) {
			_mapIn.map_mode = 1;
		}
		else {
			_mapIn.map_mode = 0;
		}
	}
	bool getMapMode()
	{
		return _mapIn.map_mode;
	}
	void setRoamKeyUp(bool p_upPress)
	{
		_mapIn.upPress = p_upPress;
	}
	void setRoamKeyDown(bool p_downPress)
	{
		_mapIn.downPress = p_downPress;
	}
	void setRoamKeyLeft(bool p_leftPress)
	{
		_mapIn.leftPress = p_leftPress;
	}
	void setRoamKeyRight(bool p_rightPress)
	{
		_mapIn.rightPress = p_rightPress;
	}
	void setWindowPos(bool p_mouseClick, int32_t x, int32_t y)
	{
		_mapIn.mouseClick = p_mouseClick;
		_mapIn.xPixel = x;
		_mapIn.yPixel = y;
	}
	int32_t getWindowPosX()
	{
		return _mapIn.xPixel;
	}
	int32_t getWindowPosY()
	{
		return _mapIn.yPixel;
	}
	void setRange(bool p_rise)
	{
		if (p_rise) {
			_rangIndex++;
			if (_rangIndex > RANGE_NUM - 1) {
				_rangIndex = RANGE_NUM - 1;
			}
		}
		else {
			_rangIndex--;
			if (_rangIndex < 0) {
				_rangIndex = 0;
			}
		}
		_mapIn.ring_range = s_range[_rangIndex];
	}

	void autoRunTest();

private:
	void _getDisAndAngleByGeo(Geo_Wgs84_t* s, Geo_Wgs84_t* e, double* dis, double* alpha);

private:
	static const float s_range[RANGE_NUM];
	int32_t _rangIndex = 8;

	MAP_Param_From_Inputs_t _mapIn;
	std::string _mapName;
};

class ControlPanel
{
public:
	ControlPanel();

	bool init(GLFWwindow* glfwWindow, uint32_t mapNum);

	MAP_Param_From_Inputs_t* getMapIn(uint32_t p_mapId);

	void preRender();
	void render(uint32_t p_mapId, uint32_t p_texId, float p_width, float p_height);
	void renderDemo();
	void postRender();

private:
	bool _imguiInit(GLFWwindow* glfwWindow);
	void _renderControlPanel(MapIn* p_dmapIn);

private:
	std::vector<MapIn *> _dmapIns;
};
