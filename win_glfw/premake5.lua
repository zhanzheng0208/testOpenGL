project "DMapWindows_glfw"
	kind "ConsoleApp"
	language "C"
	staticruntime "off"

	targetdir ("%{wks.location}/../bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/obj/" .. outputdir .. "/%{prj.name}")
	
	files
	{	
		"%{wks.location}/../win_glfw/**.c",
		"%{wks.location}/../../AMDBEngine/**.h",
		"%{wks.location}/../../AMDBEngine/**.c",
		"%{wks.location}/../3rdParty/deps/glad/gl.h",
		"%{wks.location}/../3rdParty/deps/glad_gl.c"
	}

	includedirs
	{
		"%{wks.location}/../../AMDBEngine/extern/libdeflate/include",
		"%{wks.location}/../../AMDBEngine/include",
		"%{wks.location}/..",
		"%{wks.location}/../3rdParty/glfw/include",
		"%{wks.location}/../3rdParty/deps"
	}

	links
	{
		"%{wks.location}/../3rdParty/glfw/lib/glfw3.lib"
	}

	filter "system:windows"
		systemversion "latest"
		
		defines
		{
			"USE_GLFW=1",
			"MAP_ON_IDU=0",
			"MAP_WIN_CONSOLE=1",
			"MAP_TIME_MEASURING=1",
			"HAS_RING_MODE=0",
			"MAP_DEBUG=0",
			"MAP_FONT_DEBUG=0",
			"MAP_GL_DEBUG=1",
			"GPU_RAST_DEBUG=0",
			"GPU_VEC_DEBUG=0",
			"GPU_VEC_SCHEDULE_DEBUG=0",
			"GPU_VEC_DRAW_DEBUG=0",
			"USE_DEBUG_ELEMENT=1",
			"USE_DEBUG_INPUT=1",
			"IO_PERF_TEST=0",
			"FLOW_PERF_TEST=0",
			"BOUNDARY_LINE_USE_TEXTURE=0",
			"WINDOW_NUM=2"
		}

	filter "configurations:Debug"
		--defines "DMAP_DEBUG"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		--defines "DMAP_RELEASE"
		runtime "Release"
		optimize "on"
