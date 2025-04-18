workspace "CardLab"
    architecture "x64"
    configurations { "Debug", "Release" }
    startproject "Card Lab"
	location "Build"

project "CardLab"
    location "Build"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++20"
    systemversion "latest"
	targetname "Enigma's Card Lab"

    targetdir ("Build/bin/%{cfg.architecture}/%{cfg.buildcfg}")
    objdir ("Build/obj/%{cfg.architecture}/%{cfg.buildcfg}")

    files
	{
	    "Source/**.cpp",
        "Source/**.h",
		"Vendor/imgui/**.cpp",
		"Vendor/imgui/**.h",
		"Vendor/imgui/misc/cpp/**.cpp",
		"Vendor/imgui/misc/cpp/**.h"
    }
	
	excludes
	{
		"Vendor/imgui/backends/**.cpp",
		"Vendor/imgui/backends/**.h",
		"Vendor/imgui/examples/**.cpp",
		"Vendor/imgui/examples/**.h",
		"Vendor/imgui/misc/debuggers/**.cpp",
		"Vendor/imgui/misc/debuggers/**.h",
		"Vendor/imgui/misc/fonts/**.cpp",
		"Vendor/imgui/misc/fonts/**.h",
		"Vendor/imgui/misc/freetype/**.cpp",
		"Vendor/imgui/misc/freetype/**.h",
		"Vendor/imgui/misc/single_file/**.cpp",
		"Vendor/imgui/misc/single_file/**.h",
        "Vendor/SFML/examples/**.cpp",
        "Vendor/SFML/examples/**.h"
    }

    includedirs
	{
        "Vendor/imgui",
		"Vendor/imgui/misc/cpp",
        "Vendor/SFML/include"
    }

    libdirs
	{
        "Vendor/SFML/lib"
    }

    links
	{
        "opengl32",
        "freetype",
        "winmm",
        "gdi32",
        "flac",
        "vorbisenc",
        "vorbisfile",
        "vorbis",
        "ogg",
        "ws2_32",
    }

    filter "system:windows"
        defines { "PLATFORM_WINDOWS", "SFML_STATIC" }

    filter "configurations:Debug"
        defines { "DEBUG" }
        symbols "On"
		
		links
		{
		    "sfml-graphics-s-d",
			"sfml-window-s-d",
			"sfml-system-s-d",
		}

    filter "configurations:Release"
        defines { "NDEBUG" }
        optimize "On"

		links
		{
		    "sfml-graphics-s",
			"sfml-window-s",
			"sfml-system-s",
		}