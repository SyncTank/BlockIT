project "App"
   kind "ConsoleApp"
   language "C++"
   cppdialect "C++20"
   targetdir "Binaries/%{cfg.buildcfg}"
   staticruntime "off"

   files { "Source/**.h", "Source/**.cpp", "Source/**.c", "imgui/backends/*.cpp", "imgui/backends/*.h", "imgui/*.h", "imgui/*.cpp" }

   includedirs
   {
      "Source",
	  "libs",
	  "imgui",
	  "imgui/backends",
	  "libs/glfw/include",
	  -- Include Core
	  "../Core/Source"
   }
   
   libdirs { "libs/glfw/lib-vc2010-32" }

   links
   {
	  "glfw3",
      "opengl32",
      "gdi32",
      "shell32",
      "Core"
   }

   targetdir ("../Binaries/" .. OutputDir .. "/%{prj.name}")
   objdir ("../Binaries/Intermediates/" .. OutputDir .. "/%{prj.name}")

   filter "system:windows"
       systemversion "latest"
       defines { "WINDOWS" }

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