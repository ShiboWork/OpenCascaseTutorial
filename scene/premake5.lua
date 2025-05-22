project "scene"
    kind "StaticLib"  -- 或者 "ConsoleApp" 或其他类型，取决于你的需求
    language "C++"
    targetdir "build/bin/%{cfg.buildcfg}"
    objdir "build/obj/%{cfg.buildcfg}"
    
    files { "**.h", "**.cpp" }  -- 包含 scene 文件夹中的所有 .h 和 .cpp 文件
    
    includedirs
    {
        "C:/OOCT7.8/OCCT-7_8_0-build/inc", 
        "C:/glfw-3.4/include"
    }
    
    links
    {
        "TKernel", "TKMath", "TKG2d", "TKG3d", "TKGeomBase", "TKGeomAlgo", "TKBRep", "TKTopAlgo", "TKPrim", "TKMesh", "TKService", "TKOpenGl", "TKV3d", 
        "glfw3"
    }

    -- 开启预编译头文件
    pchheader "pch.h"  -- 指定预编译头文件
    pchsource "src/pch.cpp"  -- 指定生成.pch文件的源文件

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
          "path=%path%;C:/OOCT7.8/OCCT-7_8_0-build/win64/vc14/bind"
      }

   filter "configurations:Release"
      defines { "NDEBUG" }
      symbols "Off"
      optimize "On"
      libdirs
      {
          "D:/OpenCASCADE-7.7.0/opencascade-7.7.0/win64/vc14/lib", 
          "D:/glfw-3.3.8/lib"
      }
      
      debugenvs
      {
          "path=%path%;D:/OpenCASCADE-7.7.0/opencascade-7.7.0/win64/vc14/bin"
      }