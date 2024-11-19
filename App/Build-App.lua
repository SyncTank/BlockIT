project "App"
   kind "WindowedApp"
   language "C++"
   cppdialect "C++20"
   targetdir "Binaries/%{cfg.buildcfg}"
   staticruntime "off"

   files { "Source/*.h", "Source/*.cpp", "Source/*.c", 
   "include/imgui/**/*.h", "include/imgui/**/*.cpp", "include/imgui/**/*.hpp",
   "include/imgui/*.h", "include/imgui/*.cpp", 
	"include/GLFW/*.h",
   "include/stb/*.h", 
   "include/nfd/*.h", "include/nfd/*.hpp", "include/nfd/*.cpp"}

   includedirs{
		"include/fmt",
		"include/nfd",
		"include/stb",
		"include/imgui",
		"include/imgui/backends",
		"../Core/Source"
   }
   
   libdirs { "libs/glfw" }

   links {
    "glfw3dll",      -- Static GLFW library
    "opengl32",      -- OpenGL library (required by GLFW)
    "gdi32",         -- Windows Graphics Device Interface
    "user32",        -- Windows User API
    "shell32",       -- Windows Shell API
    "advapi32",      -- Advanced Windows API
    "ws2_32",        -- Winsock library (network support for Windows)
    "Core"           -- core library
	}

   targetdir ("../Binaries/" .. OutputDir .. "/%{prj.name}")
   objdir ("../Binaries/Intermediates/" .. OutputDir .. "/%{prj.name}")

   filter "system:windows"
       systemversion "latest"
       defines { "WINDOWS" }
	   linkoptions { "/SUBSYSTEM:WINDOWS", "/VERBOSE" }


   filter "configurations:Debug"
       defines { "DEBUG" }
       runtime "Debug"
       symbols "On"

   filter "configurations:Release"
       defines { "RELEASE" }
       runtime "Release"
       optimize "On"
       symbols "On"

   filter "configurations:Dist"
       defines { "DIST" }
       runtime "Release"
       optimize "On"
       symbols "Off"