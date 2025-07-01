project "Myro-Examples"
	kind "ConsoleApp"
	staticruntime "off"
	language "C++"
	cppdialect "C++20"

	targetdir ("%{wks.location}/bin/" ..outputdir.. "/%{prj.name}")
	objdir ("%{wks.location}/bin-int/" ..outputdir.. "/%{prj.name}")

	files
	{
		"src/**.h",
		"src/**.cpp"
	}

	defines
	{
		"_CRT_SECURE_NO_WARNINGS"
	}

	includedirs
	{
		"%{wks.location}/Myro/vendor",
		"%{wks.location}/Myro/src",
		"%{IncludeDir.coco}",
		"%{IncludeDir.dtlog}"
	}

	links
	{
		"Myro"
	}

	filter "system:windows"
		systemversion "latest"

	filter "configurations:Debug"
		defines "MYRO_DEBUG"
		runtime "Debug"
		symbols "on"
		
	filter "configurations:Release"
		defines "MYRO_RELEASE"
		runtime "Release"
		optimize "on"
		
	filter "configurations:Dist"
		defines "MYRO_DIST"
		runtime "Release"
		optimize "on"
