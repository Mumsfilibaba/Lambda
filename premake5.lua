workspace "Lambda"
	architecture "x64"
	startproject "Sandbox"
	configurations { "Debug", "Release" }	
	
	outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"
	
	files 
	{ 
		"%{prj.name}/**.hpp",
		"%{prj.name}/**.h",
		"%{prj.name}/**.cpp",
		"%{prj.name}/**.c"
	}
	
	filter "system:windows"
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
		"%{prj.name}/Include"
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