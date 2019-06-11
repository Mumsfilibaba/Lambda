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
			"d3dcompiler"
		}
		defines
		{
			"LAMBDA_PLAT_WINDOWS" 
		}

	filter "system:macosx"
		links
		{
			"glfw.3.3"
		}
		libdirs
		{
			"/usr/local/lib"
		}
		sysincludedirs
		{
			"/usr/local/include"
		}
		defines
		{
			"LAMBDA_PLAT_MACOS" 
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
		"%{prj.name}/Include",
		"Dependencies"
	}
	
	filter "system:windows"
		pchheader "LambdaPch.h"
		pchsource "Lambda/Source/LambdaPch.cpp"
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
	
	dependson { "Lambda" }
	
	links 
	{ 
		"Lambda"
	}