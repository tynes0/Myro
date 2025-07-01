include "./vendor/premake/premake_customization/solution_items.lua"
include "dependencies.lua"

workspace "Myro"
	architecture "x86_64"
	startproject "Myro-Examples"

	configurations
	{
		"Debug",
		"Release",
		"Dist"
	}

	solution_items
	{
		".editorconfig"	
	}

	flags
	{
		"MultiProcessorCompile"
	}
	
outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{}"

group "Additions"
	include "vendor/premake"
	include "myro/vendor/libogg"
	include "myro/vendor/OpenAL-Soft"
	include "myro/vendor/Vorbis"
group ""

group "Core"
	include "Myro"
group ""

group "Examples"
	include "Myro-Examples"
group ""
