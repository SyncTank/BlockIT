project "App"
   kind "WindowedApp"
   language "C++"
   cppdialect "C++20"
   targetdir "Binaries/%{cfg.buildcfg}"
   staticruntime "on"

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
   
   libdirs { "libs/glfw/lib-vc2022-64" }

   links {
    "glfw3_mt",      -- Static GLFW library
    "opengl32",      -- OpenGL library (required by GLFW)
	"Core"           -- core library
	}

   targetdir ("../Binaries/" .. OutputDir .. "/%{prj.name}")
   objdir ("../Binaries/Intermediates/" .. OutputDir .. "/%{prj.name}")

   filter "system:windows"
       systemversion "latest"
       defines { "WINDOWS", "GLFW_STATIC" }
	   
       linkoptions { "/SUBSYSTEM:WINDOWS", "/VERBOSE", "/MANIFEST:embed" }
	   
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