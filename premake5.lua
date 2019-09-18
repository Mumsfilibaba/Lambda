workspace "Lambda"
	architecture "x64"
	startproject "Sandbox"
	warnings "Extra"
	configurations 
	{ 
		"Debug", 
		"Release" 
	}	

	outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"
	
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

	filter "action:vs*"
		defines
		{
			"LAMBDA_VISUAL_STUDIO",
			"_SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING"
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

	filter "system:windows"
		links
		{
			"d3d12",
			"dxgi",
			"d3dcompiler",
			"vulkan-1"
		}
		libdirs
		{
			"C:/VulkanSDK/1.1.114.0/Lib"
		}
		sysincludedirs
		{
			"C:/VulkanSDK/1.1.114.0/Include"
		}
		defines
		{
			"LAMBDA_PLAT_WINDOWS" 
		}

	filter "system:macosx"
		links
		{
			"glfw.3.3",
			"vulkan.1",
			"vulkan.1.1.114",
			"assimp"
		}
		libdirs
		{
			"/usr/local/lib",
			"../vulkansdk-macos-1.1.114.0/macOS/lib"
		}
		sysincludedirs
		{
			"/usr/local/include",
			"../vulkansdk-macos-1.1.114.0/macOS/include"
		}
		defines
		{
			"LAMBDA_PLAT_MACOS" 
		}

project "Lambda"
	kind "SharedLib"
	language "C++"
	cppdialect "C++17"
	systemversion "latest"
	location "Lambda"
	targetdir ("Build/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("Build/bin-int/" .. outputdir .. "/%{prj.name}")
	
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
	
	filter "system:windows"
		pchheader "LambdaPch.h"
		pchsource "Lambda/Source/LambdaPch.cpp"
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
		"Dependencies/glm"
	}
	
	dependson 
	{ 
		"Lambda" 
	}
	
	links 
	{ 
		"Lambda"
	}