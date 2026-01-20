workspace "EarthBoundBackgrounds"
    configurations { "Debug", "Release" }
    platforms { "x86_64", "ARM64" }
    language "C++"
    cppdialect "C++20"

    filter "configurations:Debug"
        defines { "DEBUG" }
        symbols "On"
        optimize "Off"

    filter "configurations:Release"
        defines { "NDEBUG" }
        optimize "On"
        symbols "Off"

project "ebbg"
    kind "ConsoleApp"
    targetdir "bin/%{cfg.buildcfg}"
    objdir "obj/%{cfg.buildcfg}"

    -- Source files
    files {
        "main.cpp",
        "rom/**.cpp",
        "rom/**.h",
        "rom/**.hpp"
    }

    -- Include directories
    includedirs {
        ".",
        "rom"
    }

    -- SDL2 configuration
    filter "system:windows"
        links { "SDL2", "SDL2main" }
        includedirs { "$(SDL2_INCLUDE_DIR)" }
        libdirs { "$(SDL2_LIB_DIR)" }

    filter "system:linux"
        links { "SDL2" }
        linkoptions { "`pkg-config --libs sdl2`" }
        buildoptions { "`pkg-config --cflags sdl2`" }

    filter "system:macosx"
        links { "SDL2" }
        linkoptions { "`pkg-config --libs sdl2`" }
        buildoptions { "`pkg-config --cflags sdl2`" }

    -- Compiler flags
    filter "action:gmake* or action:ninja"
        buildoptions {
            "-Wall",
            "-Wextra",
            "-Wpedantic"
        }

    filter "toolset:msc"
        buildoptions {
            "/W4"
        }
