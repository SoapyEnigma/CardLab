function copyFolderRecursive(sourceDir, targetDir)
    local files = os.matchfiles(sourceDir .. "/**")
    for _, file in ipairs(files) do
        local relative = path.getrelative(sourceDir, file)
        local targetPath = path.join(targetDir, relative)
        os.mkdir(path.getdirectory(targetPath))
        os.copyfile(file, targetPath)
    end
end

workspace "CardLab"
    architecture "x64"
    configurations { "Debug", "Release" }
    startproject "Card Lab"
	location "Build"

project "CardLab"
    location "Build"
    language "C++"
    cppdialect "C++20"
    systemversion "latest"
	targetname "Enigma's Card Lab"
	
	copyFolderRecursive("Data/Custom", "Build/Custom")
	os.mkdir("Build/Custom/Flairs")
	os.copyfile("Data/Data.mpq", "Build/Data.mpq")

    targetdir ("Build/bin/%{cfg.architecture}/%{cfg.buildcfg}")
    objdir ("Build/obj/%{cfg.architecture}/%{cfg.buildcfg}")

    files
	{
	    "Source/**.cpp",
        "Source/**.h",
		"Vendor/imgui/**.cpp",
		"Vendor/imgui/**.h",
		"Vendor/imgui/misc/cpp/**.cpp",
		"Vendor/imgui/misc/cpp/**.h",
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
        "Vendor/SFML/include",
		"Vendor/StormLib/src"
    }

    libdirs
	{
        "Vendor/SFML/lib",
		"Vendor/StormLib/lib"
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
		"StormLibRUS",
		"StormLibDUD",
		"StormLibRUD"
    }

    filter "system:windows"
        defines { "PLATFORM_WINDOWS", "SFML_STATIC" }

    filter "configurations:Debug"
        defines { "DEBUG" }
        symbols "On"
		kind "ConsoleApp"
		
		links
		{
		    "sfml-graphics-s-d",
			"sfml-window-s-d",
			"sfml-system-s-d",
		}
		
    filter "configurations:Release"
        defines { "NDEBUG" }
        optimize "On"
		kind "WindowedApp"
        entrypoint "mainCRTStartup"

		links
		{
		    "sfml-graphics-s",
			"sfml-window-s",
			"sfml-system-s",
		}
		
	filter "configurations:*"
    postbuildcommands 
    {
		'xcopy /E /I /Y "Data\\Custom" "%{cfg.targetdir}\\Custom\\"',
        "{MKDIR} %{cfg.targetdir}/Custom/Flairs",
		"{MKDIR} %{cfg.targetdir}/Custom/Evolves_From",
		"{MKDIR} %{cfg.targetdir}/Custom/Illustrations",
		
		
		"{COPYFILE} %[Data/Custom/Evolves_From/pre_evolution_template.png] %[%{!cfg.targetdir}/Custom/Evolves_From/]",
		"{COPYFILE} %[Data/Custom/Illustrations/fa_illustration_template.png] %[%{!cfg.targetdir}/Custom/Illustrations/]",
		"{COPYFILE} %[Data/Custom/Illustrations/normal_illustration_template.png] %[%{!cfg.targetdir}/Custom/Illustrations/]",
		"{COPYFILE} %[Data/Notes.txt] %[%{!cfg.targetdir}]",
		"{COPYFILE} %[Data/Data.mpq] %[%{!cfg.targetdir}]"
    }