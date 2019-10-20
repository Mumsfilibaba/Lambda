workspace "Lambda"
	architecture "x64"
	startproject "Sandbox"
	warnings "Extra"

	outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

	configurations 
	{ 
		"Debug", 
		"Release" 
	}
	filter "configurations:Debug"
		symbols "On"
		runtime "Debug"
		defines 
		{ 
			"LAMBDA_DEBUG"
		}	
	filter "configurations:Release"
		symbols "On"
		runtime "Release"
		optimize "Full"
		defines 
		{ 
			"LAMBDA_RELEASE" 
		}

	-- Visual Studio SPECIFIC
	filter "action:vs*"
		defines
		{
			"LAMBDA_VISUAL_STUDIO",
			"_SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING",
			"_CRT_SECURE_NO_WARNINGS"
		}
	filter { "action:vs*", "configurations:Debug" }
		defines
		{
			"_DEBUG",
			"_CRTDBG_MAP_ALLOC"
		}
	filter { "action:vs*", "configurations:Release" }
		defines
		{
			"NDEBUG"
		}
	-- macOS SPECIFIC
	filter "system:macosx"
		defines
		{
			"LAMBDA_PLAT_MACOS" 
		}
	-- WINDOWS SPECIFIC
	filter "system:windows"
		defines
		{
			"LAMBDA_PLAT_WINDOWS" 
		}

	-- Dependencies
	group "Dependencies"
		include "Dependencies/ImGui"
	group ""

	-- ENGINE PROJECT
	project "Lambda"
		kind "SharedLib"
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
		--	macOS SPECIFIC
		filter "system:macosx"
			links
			{
				"glfw.3.3",
				"vulkan.1",
				"vulkan.1.1.121",
				"assimp",
				"ImGui"
			}
			libdirs
			{
				"/usr/local/lib",
				"../vulkansdk-macos-1.1.121.1/macOS/lib"
			}
			sysincludedirs
			{
				"/usr/local/include",
				"../vulkansdk-macos-1.1.121.1/macOS/include"
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
				"C:/VulkanSDK/1.1.114.0/Lib"
			}
			sysincludedirs
			{
				"C:/VulkanSDK/1.1.114.0/Include"
			}
			sysincludedirs
			{
				"Dependencies/Assimp/include"
			}
			defines 
			{ 
				"LAMBDA_EXPORT" 
			}
			postbuildcommands
			{
				("{COPY} %{cfg.buildtarget.relpath} \"../Build/bin/" .. outputdir .. "/Sandbox/\"")
			}
		filter { "system:windows", "configurations:Debug" }
			links
			{
				"assimp-vc141-mtd",
				"IrrXMLd",
				"zlibstaticd",
			}
			libdirs
			{
				"Dependencies/Assimp/build/code/Debug/",
				"Dependencies/Assimp/build/contrib/irrXML/Debug",
				"Dependencies/Assimp/build/contrib/zlib/Debug"
			}
		filter { "system:windows", "configurations:Release" }
			links
			{
				"assimp-vc141-mt",
				"IrrXML",
				"zlibstatic",
			}
			libdirs
			{
				"Dependencies/Assimp/build/code/Release/",
				"Dependencies/Assimp/build/contrib/irrXML/Release",
				"Dependencies/Assimp/build/contrib/zlib/Release"
			}

	-- EXAMPLE PROJECTS
	project "Sandbox"
		kind "ConsoleApp"
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