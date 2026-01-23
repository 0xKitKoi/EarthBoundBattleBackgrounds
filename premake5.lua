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

    includedirs {
      ".",
      "rom",
      "thirdparty/SDL2/include",
      "thirdparty/SDL2_image/include",
      "thirdparty/SDL2_ttf/include"
    }

    libdirs {
      "thirdparty/SDL2/lib/x64",
      "thirdparty/SDL2_image/lib/x64",
      "thirdparty/SDL2_ttf/lib/x64"
    }

    links {
      "SDL2",
      "SDL2main",
      "SDL2_image",
      "SDL2_ttf"
    }


    -- SDL2 configuration
    filter "system:windows"
        links { "SDL2", "SDL2main" }
        includedirs { "$(SDL2_INCLUDE_DIR)" }
        libdirs { "$(SDL2_LIB_DIR)" }
        postbuildcommands {
            "{COPY} %{wks.location}/thirdparty/SDL2/lib/x64/SDL2.dll %{cfg.targetdir}",
            "{COPY} %{wks.location}/thirdparty/SDL2_image/lib/x64/SDL2_image.dll %{cfg.targetdir}",
            "{COPY} %{wks.location}/thirdparty/SDL2_ttf/lib/x64/SDL2_ttf.dll %{cfg.targetdir}",
            "{COPY} %{wks.location}/truncated_backgrounds.dat %{cfg.targetdir}"
        }

    --filter "system:linux"
    --    links { "SDL2" }
    --    linkoptions { "`pkg-config --libs sdl2`" }
    --    buildoptions { "`pkg-config --cflags sdl2`" }
    filter "system:linux"
        links { "SDL2" }
        linkoptions { "`pkg-config --libs sdl2`" }
        includedirs { "/usr/include/SDL2" } -- this is so #include <SDL.h> works on linux, because of how SDL2 is setup for windows.
        postbuildcommands {
            "{COPY} %{wks.location}/truncated_backgrounds.dat %{cfg.targetdir}"
        }

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
