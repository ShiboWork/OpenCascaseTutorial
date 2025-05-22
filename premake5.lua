workspace "OcctImgui"
    configurations {"Debug", "Release"}
    system "Windows"
    platforms {"Win64"}
    architecture "X64"
    language "C++"
    
project "OcctImgui"
    kind "ConsoleApp"
    language "C++"
    targetdir "build/bin/%{cfg.buildcfg}"
    objdir "build/obj/%{cfg.buildcfg}"
    
    files { "**.h",  "**.cpp"}
    
    -- Header files.
    includedirs
    {
        "C:/OOCT7.8/OCCT-7_8_0-build/inc", 
        "C:/glfw-3.4/include"
    }

    -- Library files.
    links
    {
        "TKernel", "TKMath", "TKG2d", "TKG3d", "TKGeomBase", "TKGeomAlgo", "TKBRep", "TKTopAlgo", "TKPrim", "TKMesh", "TKService", "TKOpenGl", "TKV3d", "glfw3"
    }
    
    filter "configurations:Debug"
      defines { "DEBUG" }
      symbols "On"
      
      libdirs
      {
          "C:/OOCT7.8/OCCT-7_8_0-build/win64/vc14/libd", 
          "C:/glfw-3.4/lib-vc2022"
      }
      
      debugenvs
      {
          "path=%path%;C:/OOCT7.8/OCCT-7_8_0-build/win64/vc14/bind;$(ProjectDir)3rdParty"
      }

   filter "configurations:Release"
      defines { "NDEBUG" }
      symbols "Off"
      optimize "On"
      libdirs
      {
          "C:/OOCT7.8/OCCT-7_8_0-build/win64/vc14/lib", 
          "C:/glfw-3.4/lib-vc2022"
      }
      
      debugenvs
      {
          "path=%path%;C:/OOCT7.8/OCCT-7_8_0-build/win64/vc14/bin;3rdParty"
      }

-- 包含 Scene 文件夹中的 premake5.lua 文件
include "Scene/premake5.lua"