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
		"%{prj.name}/**.cpp",
		"%{prj.name}/**.c",
		"%{prj.name}/**.hlsl"
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
	
	pchheader "LambdaPch.h"
	pchsource "Lambda/Source/LambdaPch.cpp"
	
	location "Lambda"
	targetdir ("Build/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("Build/bin-int/" .. outputdir .. "/%{prj.name}")
	
	includedirs 
	{ 
		"%{prj.name}/Include",
		"Dependencies"
	}

	defines 
	{ 
		"LAMBDA_EXPORT" 
	}
	
	filter "system:windows"
		cppdialect "C++17"
		systemversion "latest"
		
		postbuildcommands
		{
			("{COPY} %{cfg.buildtarget.relpath} \"../Build/bin/" .. outputdir .. "/Sandbox/\"")
		}

project "Sandbox"
	kind "ConsoleApp"
	language "C++"
	
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
	
	filter "system:windows"
		cppdialect "C++17"
		systemversion "latest"