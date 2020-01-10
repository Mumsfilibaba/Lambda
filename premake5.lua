workspace "Lambda"
	architecture "x64"
	startproject "Sandbox"
	warnings "Extra"

	-- Were to output files
	outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.platform}"
	-- Confingurations
	configurations 
	{
		"Debug", 
		"Release",
		"Production",
	}
	-- Platform
	platforms
	{
		"x64_SharedLib",
		"x64_StaticLib",
	}
	-- Architecture
	filter "architecture:x64"
		defines 
		{ 
			"LAMBDA_X64",
		}
	-- Debug builds
	filter "configurations:Debug"
		symbols "On"
		runtime "Debug"
		defines 
		{ 
			"LAMBDA_DEBUG",
			"_DEBUG",
		}
	-- Release builds
	filter "configurations:Release"
		symbols "On"
		runtime "Release"
		optimize "Full"
		defines 
		{ 
			"LAMBDA_RELEASE",
		}
	-- Production builds
	filter "configurations:Production"
		symbols "Off"
		runtime "Release"
		optimize "Full"
		defines 
		{ 
			"LAMBDA_PRODUCTION",
		}
	-- Setup dev-builds
	filter "configurations:Debug or Release"
		defines
		{
			"LAMBDA_DEVELOP",
		}
	-- Not debug builds
	filter "configurations:Release or Production"
		defines
		{
			"NDEBUG",
		}
	-- Visual Studio
	filter "action:vs*"
		defines
		{
			"LAMBDA_VISUAL_STUDIO",
			"_SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING",
			"_CRT_SECURE_NO_WARNINGS",
		}
	filter { "action:vs*", "configurations:Debug" }
		defines
		{
			"_CRTDBG_MAP_ALLOC",
		}
	-- macOS
	filter "system:macosx"
		defines
		{
			"LAMBDA_PLAT_MACOS",
		}
	-- Windows
	filter "system:windows"
		defines
		{
			"LAMBDA_PLAT_WINDOWS",
		}
	-- Desktop
	filter "system:macosx or windows"
		defines
		{
			"LAMBDA_DESKTOP"
		}
	-- Shared lib build
	filter "platforms:x64_SharedLib"
		defines
		{
			"LAMBDA_SHARED_LIB",
		}
	filter {}
	
	-- ENGINE PROJECT
	project "Lambda"
		language "C++"
		cppdialect "C++17"
		systemversion "latest"
		location "Lambda"

		-- SharedLib builds
		filter "platforms:x64_SharedLib"
			kind "SharedLib"
		filter {}
		-- StaticLib builds
		filter "platforms:x64_StaticLib"
			kind "StaticLib"
		filter {}

		-- Engine specific defines
		defines 
		{ 
			"LAMBDA_EXPORT",
		}

		-- Targets
		targetdir 	("Build/bin/" .. outputdir .. "/%{prj.name}")
		objdir 		("Build/bin-int/" .. outputdir .. "/%{prj.name}")	
		
		-- Files to include
		files 
		{ 
			"%{prj.name}/**.h",
			"%{prj.name}/**.hpp",
			"%{prj.name}/**.inl",
			"%{prj.name}/**.c",
			"%{prj.name}/**.cpp",
			"%{prj.name}/**.hlsl",
		}
		-- Remove files not available for windows builds
		filter "system:windows"
			removefiles
			{
				"%{prj.name}/Source/Platform/macOS/**",
				"%{prj.name}/Include/Platform/macOS/**",
			}
		filter {}
		-- Remove files not available for windows builds
		filter "system:macosx"
			files
			{
				"%{prj.name}/**.m",
				"%{prj.name}/**.mm",
			}
			removefiles
			{
				"%{prj.name}/Source/Platform/Windows/**",
				"%{prj.name}/Include/Platform/Windows/**",
			}
		filter {}
		-- We do not want to compile HLSL files
		excludes 
		{	
			"**.hlsl",
		}
		-- Includes
		includedirs
		{
			"%{prj.name}/Include",
		}
		sysincludedirs
		{
			"Dependencies/stb",
			"Dependencies/glm",
			"Dependencies/ImGui",
		}
		--	macOS
		filter "system:macosx"
			links
			{
				-- Libraries
				"vulkan.1",
				"vulkan.1.1.121",
				"assimp",
				"ImGui",
				-- Frameworks
				"Cocoa.framework"
			}
			libdirs
			{
				"/usr/local/lib",
			}
			sysincludedirs
			{
				"/usr/local/include",
				"../vulkansdk-macos-1.1.121.1/MoltenVK/include",
			}
		-- Windows
		filter "system:windows"
			pchheader "LambdaPch.h"
			pchsource "Lambda/Source/LambdaPch.cpp"
			links
			{
				"d3d12",
				"dxgi",
				"d3dcompiler",
				"vulkan-1",
				"ImGui",
			}
			libdirs
			{
				"C:/VulkanSDK/1.1.121.2/Lib",
			}
			sysincludedirs
			{
				"C:/VulkanSDK/1.1.121.2/Include",
			}
			sysincludedirs
			{
				"Dependencies/Assimp/include",
			}
		filter { "system:windows", "platforms:x64_SharedLib" }
			-- Copy DLL into correct folder
			postbuildcommands
			{
				("{COPY} %{cfg.buildtarget.relpath} \"../Build/bin/" .. outputdir .. "/Sandbox/\"")
			}
			-- Debug Libs
		filter { "system:windows", "configurations:Debug" }
			libdirs
			{
				"Dependencies/Assimp/build/code/Debug/",
				"Dependencies/Assimp/build/contrib/irrXML/Debug",
				"Dependencies/Assimp/build/contrib/zlib/Debug",
			}
			links
			{
				"assimp-vc141-mtd",
				"IrrXMLd",
				"zlibstaticd",
			}
		-- Release/Production Libs
		filter { "system:windows", "configurations:Release or Production" }
			libdirs
			{
				"Dependencies/Assimp/build/code/Release/",
				"Dependencies/Assimp/build/contrib/irrXML/Release",
				"Dependencies/Assimp/build/contrib/zlib/Release",
			}
			links
			{
				"assimp-vc141-mt",
				"IrrXML",
				"zlibstatic",
			}
	project "*"
	
	-- EXAMPLE PROJECTS
	project "Sandbox"
		kind "WindowedApp"
		language "C++"
		cppdialect "C++17"
		systemversion "latest"
		location "Sandbox"
		
		-- Targets
		targetdir ("Build/bin/" .. outputdir .. "/%{prj.name}")
		objdir ("Build/bin-int/" .. outputdir .. "/%{prj.name}")
		
		--Includes
		includedirs
		{
			"Lambda/Include",
		}
		sysincludedirs
		{
			"Dependencies/glm",
			"Dependencies/ImGui",
		}
		-- Files
		files 
		{ 
			"%{prj.name}/**.hpp",
			"%{prj.name}/**.h",
			"%{prj.name}/**.inl",
			"%{prj.name}/**.cpp",
			"%{prj.name}/**.c",
			"%{prj.name}/**.hlsl",
		}
		-- We do not want to compile HLSL files
		excludes 
		{	
			"**.hlsl",
		}
		-- Linking
		links 
		{ 
			"Lambda",
		}
	project "*"

	-- Dependencies
	group "Dependencies"
		include "Dependencies/ImGui"
	group ""