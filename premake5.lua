workspace "Lambda"
	architecture "x64"
	startproject "Sandbox"
	warnings "Extra"

	outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

	configurations 
	{ 
		-- Build Engine as dynamic lib
		"Debug_Shared", 
		"Release_Shared",
		"Production_Shared",
		-- Build Engine as static lib
		"Debug", 
		"Release",
		"Production"
	}
	filter "configurations:Debug or Debug_Shared"
		symbols "On"
		runtime "Debug"
		defines 
		{ 
			"LAMBDA_DEBUG",
			"_DEBUG",
		}	
	filter "configurations:Release or Release_Shared"
		symbols "On"
		runtime "Release"
		optimize "Full"
		defines 
		{ 
			"LAMBDA_RELEASE" 
		}
	filter "configurations:Debug or Debug_Shared or Release or Release_Shared"
		defines
		{
			"LAMBDA_DEVELOP"
		}
	filter "configurations:Production or Production_Shared"
		symbols "Off"
		runtime "Release"
		optimize "Full"
		defines 
		{ 
			"LAMBDA_PRODUCTION" 
		}
	filter "configurations:Debug_Shared or Release_Shared or Production_Shared"
		defines 
		{ 
			"LAMBDA_SHARED_LIB" 
		}
	filter "configurations:Release or Release_Shared or Production or Production_Shared"
		defines
		{
			"NDEBUG"
		}

	-- Visual Studio SPECIFIC
	filter "action:vs*"
		defines
		{
			"LAMBDA_VISUAL_STUDIO",
			"_SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING",
			"_CRT_SECURE_NO_WARNINGS"
		}
	filter { "action:vs*", "configurations:Debug or Debug_Shared" }
		defines
		{
			"_CRTDBG_MAP_ALLOC"
		}
	-- macOS SPECIFIC
	filter "system:macosx"
		defines
		{
			"LAMBDA_PLAT_MACOS" 
		}
	filter {}
	-- WINDOWS SPECIFIC
	filter "system:windows"
		defines
		{
			"LAMBDA_PLAT_WINDOWS" 
		}
	filter {}

	-- Dependencies
	group "Dependencies"
		include "Dependencies/ImGui"
	group ""

	-- ENGINE PROJECT
	project "Lambda"
		filter "configurations:Debug or Release or Production"
			kind "StaticLib"
		filter {}

		filter "configurations:Debug_Shared or Release_Shared or Production_Shared"
			kind "SharedLib"
		filter {}

		language "C++"
		cppdialect "C++17"
		systemversion "latest"
		location "Lambda"

		targetdir 	("Build/bin/" .. outputdir .. "/%{prj.name}")
		objdir 		("Build/bin-int/" .. outputdir .. "/%{prj.name}")	
		
		files 
		{ 
			"%{prj.name}/**.hpp",
			"%{prj.name}/**.h",
			"%{prj.name}/**.inl",
			"%{prj.name}/**.cpp",
			"%{prj.name}/**.c",
			"%{prj.name}/**.hlsl"
		}
		excludes 
		{	
			"**.hlsl"
		}
		includedirs 
		{ 
			"%{prj.name}/Include"
		}
		sysincludedirs
		{
			"Dependencies/stb",
			"Dependencies/glm",
			"Dependencies/ImGui"
		}
		-- Define that we are exporting the engine and not a application
		defines
		{
			"LAMBDA_EXPORT"
		}
		--	macOS SPECIFIC
		filter "system:macosx"
			links
			{
				"glfw.3.4",
				"vulkan.1",
				"vulkan.1.1.121",
				"assimp",
				"ImGui",
			}
			libdirs
			{
				"/usr/local/lib",
			}
			sysincludedirs
			{
				"/usr/local/include",
				"../vulkansdk-macos-1.1.121.1/MoltenVK/include"
			}
		-- WINDOWS SPECIFIC
		filter "system:windows"
			pchheader "LambdaPch.h"
			pchsource "Lambda/Source/LambdaPch.cpp"
			links
			{
				"d3d12",
				"dxgi",
				"d3dcompiler",
				"vulkan-1",
				"ImGui"
			}
			libdirs
			{
				"C:/VulkanSDK/1.1.121.2/Lib"
			}
			sysincludedirs
			{
				"C:/VulkanSDK/1.1.121.2/Include"
			}
			sysincludedirs
			{
				"Dependencies/Assimp/include"
			}
			-- Copy DLL into correct folder
			postbuildcommands
			{
				("{COPY} %{cfg.buildtarget.relpath} \"../Build/bin/" .. outputdir .. "/Sandbox/\"")
			}
			-- Debug Libs
		filter { "system:windows", "configurations:Debug or Debug_Shared" }
			libdirs
			{
				"Dependencies/Assimp/build/code/Debug/",
				"Dependencies/Assimp/build/contrib/irrXML/Debug",
				"Dependencies/Assimp/build/contrib/zlib/Debug"
			}
			links
			{
				"assimp-vc141-mtd",
				"IrrXMLd",
				"zlibstaticd",
			}
		-- Release/Production Libs
		filter { "system:windows", "configurations:Release or Release_Shared or Production or Production_Shared" }
			libdirs
			{
				"Dependencies/Assimp/build/code/Release/",
				"Dependencies/Assimp/build/contrib/irrXML/Release",
				"Dependencies/Assimp/build/contrib/zlib/Release"
			}
			links
			{
				"assimp-vc141-mt",
				"IrrXML",
				"zlibstatic",
			}
	-- EXAMPLE PROJECTS
	project "Sandbox"
		kind "WindowedApp"
		language "C++"
		cppdialect "C++17"
		systemversion "latest"
		location "Sandbox"
		
		targetdir ("Build/bin/" .. outputdir .. "/%{prj.name}")
		objdir ("Build/bin-int/" .. outputdir .. "/%{prj.name}")
		
		includedirs
		{ 
			"Lambda/Include"
		}
		sysincludedirs
		{
			"Dependencies/glm",
			"Dependencies/ImGui"
		}
		files 
		{ 
			"%{prj.name}/**.hpp",
			"%{prj.name}/**.h",
			"%{prj.name}/**.inl",
			"%{prj.name}/**.cpp",
			"%{prj.name}/**.c",
			"%{prj.name}/**.hlsl"
		}
		excludes 
		{	
			"**.hlsl"
		}
		dependson 
		{ 
			"Lambda" 
		}
		links 
		{ 
			"Lambda"
		}