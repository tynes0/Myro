project "myro"
	kind "StaticLib"
	staticruntime "off"
	language "C++"
	cppdialect "C++latest"

	targetdir ("%{wks.location}/bin/" ..outputdir.. "/%{prj.name}")
	objdir ("%{wks.location}/bin-int/" ..outputdir.. "/%{prj.name}")

	files
	{
		"src/**.h",
		"src/**.cpp",
		"vendor/coco/**.h",
		"vendor/miniaudio/**.h",
		"vendor/miniaudio/**.cpp",
		"vendor/minimp3/**.h",
		"vendor/minimp3/**.cpp",
		"vendor/dtlog/**.h",
		"vendor/dtlog/**.cpp",
	}

	defines
	{
		"_CRT_SECURE_NO_WARNINGS",
		"AL_LIBTYPE_STATIC"
	}

	includedirs
	{
		"src",
		"%{IncludeDir.coco}",
		"%{IncludeDir.dtlog}",
		"%{IncludeDir.miniaudio}",
		"%{IncludeDir.libogg}",
		"%{IncludeDir.Vorbis}",
		"%{IncludeDir.minimp3}",
		"%{IncludeDir.OpenAL_Soft_include}",
		"%{IncludeDir.OpenAL_Soft_src}",
		"%{IncludeDir.OpenAL_Soft_src_common}",
	}

	links
	{
		"OpenAL-Soft",
		"Vorbis",
	}

	filter "system:windows"
		systemversion "latest"

		defines
		{
			
		}

		links
		{
		}

	filter "configurations:Debug"
		defines "MYRO_DEBUG"
		runtime "Debug"
		symbols "on"
		links
		{
		}
		
	filter "configurations:Release"
		defines "MYRO_RELEASE"
		runtime "Release"
		optimize "on"
		links
		{
		}
		
	filter "configurations:Dist"
		defines "MYRO_DIST"
		runtime "Release"
		optimize "on"
		links
		{
		}
