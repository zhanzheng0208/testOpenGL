#include <windows.h>
#include <stdio.h>
#include <iostream>
#include <string>

#include "controlPanel.h"
#include "stdbool.h"


const float MapIn::s_range[RANGE_NUM] = { 2500,5000,10000,20000,40000,80000,160000,240000,400000,750000 };

MapIn::MapIn(std::string p_mapName) :
	_rangIndex(8)
{
	_mapName = p_mapName;

	_mapIn.longitude = 121.793333333948;
	_mapIn.latitude = 31.1449999998139;
	_mapIn.altitude = 2000.0;
	_mapIn.pitch = -90.0;
	_mapIn.heading = 0.0;
	_mapIn.roll = 0.0;
	_mapIn.ring_range = s_range[_rangIndex];//100.0 * 1000.0;
	_mapIn.map_mode = 0;
	_mapIn.upPress = 0;
	_mapIn.downPress = 0;
	_mapIn.leftPress = 0;
	_mapIn.rightPress = 0;
	_mapIn.mouseClick = 0;
	_mapIn.xPixel = 0;
	_mapIn.yPixel = 0;
}

void MapIn::autoRunTest()
{
	static uint32_t s_framecount;
	static int32_t  s_init = 0;
	static int32_t  s_alt = 1000;
	static int32_t  s_alt_dir = 1;
	static double   s_step = 50.0;

	Geo_Wgs84_t  lujiazui = { 121.49971691534425, 31.239658843127756, 3.0 };
	Geo_Wgs84_t  pudong = { 121.793333333948, 31.1449999998139, 3.0 };
	Geo_Wgs84_t  hangzhou = { 120.793333333948, 30.1449999998139, 3.0 };
	Geo_Wgs84_t  jinwan = { 113.39579, 22.01721, 3.0 };
	Geo_Wgs84_t  jiujiang = { 116.1, 29.5, 3.0 };
	Geo_Wgs84_t  temDes = { 113.39579, 42.01721, 3.0 };
	Geo_Wgs84_t  yueyang = { 113.1, 29.5, 3.0 };
	Geo_Wgs84_t  testP = { 112.6, 31.5, 3.0 };

	Geo_Wgs84_t* pDest = &pudong;
	Geo_Wgs84_t* pSrc = &jinwan;

	static double s_dis = 0.0;
	static double s_heading = 0.0;
	if (s_init == 0) {
		s_init = 1;
		_getDisAndAngleByGeo(pSrc, pDest, &s_dis, &s_heading);
	}
	double step_away = s_step * s_framecount;
	double ratio = step_away / s_dis;
	if (step_away > s_dis)
		s_framecount = 0;
	Geo_Wgs84_t pos;
	pos.lon = pSrc->lon + ratio * (pDest->lon - pSrc->lon);
	pos.lat = pSrc->lat + ratio * (pDest->lat - pSrc->lat);
	s_alt += s_alt_dir * 20.0;
	if (s_alt > 3000) {
		s_alt_dir = -1;
	}
	else if (s_alt < 900) {
		s_alt_dir = 1;
	}
	pos.alt = s_alt;


	s_framecount++;

	Geo_Normalize_Lonlat(&pos);

	_mapIn.longitude = pos.lon;
	_mapIn.latitude = pos.lat;
	_mapIn.altitude = pos.alt;
	_mapIn.heading = s_heading;
}

void MapIn::_getDisAndAngleByGeo(Geo_Wgs84_t* s, Geo_Wgs84_t* e, double* dis, double* alpha)
{
	Vec3d s3d = Geo_Wgs84_To_Ecef(s);
	Vec3d e3d = Geo_Wgs84_To_Ecef(e);
	*dis = sqrt((s3d.x - e3d.x) * (s3d.x - e3d.x) + (s3d.y - e3d.y) * (s3d.y - e3d.y) +
		(s3d.z - e3d.z) * (s3d.z - e3d.z));
	*alpha = Geo_Deg_With_North(s->lon, s->lat, e->lon, e->lat);
}


ControlPanel::ControlPanel()
{
}

bool ControlPanel::init(GLFWwindow* glfwWindow, uint32_t mapNum)
{
	//_winName = p_winName;
	for (int i = 0; i < mapNum; i++) {
		std::string mapName = "MAP" + std::to_string(i+1);
		MapIn* dmapIn = new MapIn(mapName);
		_dmapIns.push_back(dmapIn);
	}

	_imguiInit(glfwWindow);

	return true;
}

MAP_Param_From_Inputs_t* ControlPanel::getMapIn(uint32_t p_mapId)
{
	assert((p_mapId >=0) && (p_mapId < _dmapIns.size()));
	return (_dmapIns[p_mapId]->getMapIn());
}

void ControlPanel::preRender()
{
	// Start the Dear ImGui frame
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	// Create the docking environment
	ImGuiWindowFlags windowFlags = 
		ImGuiWindowFlags_NoDocking | 
		ImGuiWindowFlags_NoTitleBar |
		ImGuiWindowFlags_NoCollapse |
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoBringToFrontOnFocus |
		ImGuiWindowFlags_NoNavFocus |
		ImGuiWindowFlags_NoBackground;

	ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(viewport->Pos);
	ImGui::SetNextWindowSize(viewport->Size);
	ImGui::SetNextWindowViewport(viewport->ID);

	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	ImGui::Begin("InvisibleWindow", nullptr, windowFlags);
	ImGui::PopStyleVar(3);

	ImGuiID dockSpaceId = ImGui::GetID("InvisibleWindowDockSpace");

	ImGui::DockSpace(dockSpaceId, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_PassthruCentralNode);
	ImGui::End();
}

void ControlPanel::render(uint32_t p_mapId, uint32_t p_texId, float p_width, float p_height)
{
	assert((p_mapId >= 0) && (p_mapId < _dmapIns.size()));
	MapIn* dmapIn = _dmapIns[p_mapId];

	dmapIn->autoRunTest();

	ImGui::Begin(dmapIn->getmapName().c_str());
	ImGuiIO& io = ImGui::GetIO();

	//ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
	//std::cout << viewportPanelSize.x << viewportPanelSize.y << std::endl;
	ImGui::Image(reinterpret_cast<void*>(p_texId), ImVec2{ p_width, p_height }, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });

	//鼠标双击，设置漫游点
	if (ImGui::IsMouseDoubleClicked(0) && ImGui::IsWindowFocused()) {
		//std::cout << dmapIn->getmapName() << " test" << std::endl;
		ImVec2 pos = ImGui::GetCursorScreenPos();
		int32_t mousePosX = io.MousePos.x - pos.x;
		int32_t mousePosY = pos.y - io.MousePos.y;

		dmapIn->setWindowPos(true, mousePosX, mousePosY);
	}
	else {
		dmapIn->setWindowPos(false, dmapIn->getWindowPosX(), dmapIn->getWindowPosY());
	}

	//设置量程
	if (ImGui::IsWindowFocused()) {
		if (io.MouseWheel > 0.0f) {
			io.MouseWheel = 0.0f;
			dmapIn->setRange(true);
		}
		else if (io.MouseWheel < 0) {
			io.MouseWheel = 0.0f;
			dmapIn->setRange(false);
		}
	}
	

	ImGui::End();

	_renderControlPanel(dmapIn);
}

void ControlPanel::renderDemo()
{
	ImGui::ShowDemoWindow();
}

void ControlPanel::postRender()
{
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	ImGuiIO& io = ImGui::GetIO();

	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		GLFWwindow* backup_current_context = glfwGetCurrentContext();
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
		glfwMakeContextCurrent(backup_current_context);
	}
}

bool ControlPanel::_imguiInit(GLFWwindow* glfwWindow)
{
	static bool s_isFirst = true;

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       // Enable Multi-Viewport / Platform Windows

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsLight();

	// When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
	ImGuiStyle& style = ImGui::GetStyle();
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		style.WindowRounding = 0.0f;
		style.Colors[ImGuiCol_WindowBg].w = 1.0f;
	}
		
	// Setup Platform/Renderer backends
	ImGui_ImplGlfw_InitForOpenGL(glfwWindow, true);
	ImGui_ImplOpenGL3_Init("#version 130");

	return true;
}

void ControlPanel::_renderControlPanel(MapIn *p_dmapIn)
{
	ImGuiWindowFlags window_flags = 0;
	static bool no_move = false;
	if (no_move) window_flags |= ImGuiWindowFlags_NoMove;

	std::string winName = std::string("ControlPanel_") + p_dmapIn->getmapName();
	ImGui::Begin(winName.c_str(), NULL, window_flags);

	bool isRoam = p_dmapIn->getMapMode();
	if (ImGui::Checkbox("Roam", &isRoam)) {
		p_dmapIn->setMapMode(isRoam);
	}

	bool upPress = ImGui::Button("Up"); ImGui::SameLine();
	p_dmapIn->setRoamKeyUp(upPress);
	bool downPress = ImGui::Button("Down"); ImGui::SameLine();
	p_dmapIn->setRoamKeyDown(downPress);
	bool leftPress = ImGui::Button("Left"); ImGui::SameLine();
	p_dmapIn->setRoamKeyLeft(leftPress);
	bool rightPress = ImGui::Button("Right"); ImGui::SameLine();
	p_dmapIn->setRoamKeyRight(rightPress);

	ImGui::End();
}

