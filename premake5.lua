workspace("qor")
    targetdir("bin")
    debugdir("bin")
    
    configurations {"Debug", "Release"}
    --platforms { "x86", "x86_64" }
    
        --filter "platforms:x86"
        --    arch = 32
        --    architecture "x32"
        --filter "platforms:x86_64"
        --    arch = 64
        --    architecture "x64"
        --filter {}

        defines {
            "META_SHARED",
            "GLM_FORCE_CTOR_INIT",
            "GLM_ENABLE_EXPERIMENTAL",
            "GLM_FORCE_RADIANS",
            "NOMINMAX"
        }
        
        -- Debug Config
        configuration "Debug"
            defines { "DEBUG" }
            symbols "On"
            linkoptions { }

            configuration "linux"
                links {
                    "z",
                    "bfd",
                    "iberty"
                }
        
        -- Release Config
        configuration "Release"
            defines { "NDEBUG" }
            optimize "Speed"
            floatingpoint "Fast"
            targetname("qor_dist")

        -- gmake Config
        configuration "gmake"
            buildoptions { "-std=c++11" }
            -- buildoptions { "-std=c++11", "-pedantic", "-Wall", "-Wextra", '-v', '-fsyntax-only'}
            links {
                "pthread",
                "SDL2_ttf",
                "SDL2",
                "GLEW",
                "assimp",
                "freeimage",
                "openal",
                "alut",
                "ogg",
                "vorbis",
                "vorbisfile",
                "boost_system",
                "boost_filesystem",
                "boost_coroutine",
                "boost_python",
                "boost_regex",
                "jsoncpp",
                "raknet",
                --"BulletSoftBody",
                "BulletDynamics",
                "BulletCollision",
                "LinearMath",
            }
            includedirs {
                "lib/qor/",
                "lib/qor/lib/kit",
                "lib/qor/lib/kit/lib/local_shared_ptr",
                "/usr/local/include/",
                "/usr/include/bullet/",
                "/usr/include/rapidxml/",
                "/usr/include/raknet/DependentExtensions"
            }

            libdirs {
                "/usr/local/lib"
            }
            
            buildoptions {
                "`python2-config --includes`",
                "`pkg-config --cflags cairomm-1.0 pangomm-1.4`"
            }

            linkoptions {
                "`python2-config --libs`",
                "`pkg-config --libs cairomm-1.0 pangomm-1.4`"
            }
            
        configuration "macosx"
            links {
                "boost_thread-mt",
            }
            buildoptions { "-framework OpenGL" }
            linkoptions { "-framework OpenGL" }
            --buildoptions { "-U__STRICT_ANSI__", "-stdlib=libc++" }
            --linkoptions { "-stdlib=libc++" }

        configuration "linux"
            links {
                "GL",
                "boost_thread",
            }

        configuration "windows"
            toolset "v141"
            flags { "MultiProcessorCompile", "NoMinimalRebuild" }
            linkoptions { "/ignore:4099" } -- ignore pdb warnings

            links {
                "ws2_32",
                "glibmm.dll.lib",
                "cairomm.dll.lib",
                "pangomm.dll.lib",
                "SDL2main",
                "OpenGL32",
                "GLU32",
                "SDL2",
                "SDL2_ttf",
                "GLEW32",
                "assimp",
                "freeimage",
                "OpenAL32",
                "alut",
                "libogg",
                "libvorbis",
                "libvorbisfile",
                --"boost_system-vc141-mt-1_64",
                --"boost_filesystem-vc141-mt-1_64",
                --"boost_thread-vc141-mt-1_64",
                "python27",
                --"boost_python-vc141-mt-1_64",
                --"boost_coroutine-vc141-mt-1_64",
                --"boost_regex-vc141-mt-1_64",
                "lib_json",
            }

            includedirs {
                "c:/Python27/include",
                "c:/gtkmm/lib/pangomm/include",
                "c:/gtkmm/lib/sigc++/include",
                "c:/gtkmm/lib/cairomm/include",
                "c:/gtkmm/include/pango",
                "c:/gtkmm/include/pangomm",
                "c:/gtkmm/include/sigc++",
                "c:/gtkmm/include",
                "c:/gtkmm/include/cairo",
                "c:/gtkmm/lib/glib/include",
                "c:/gtkmm/include/glib",
                "c:/gtkmm/lib/glibmm/include",
                "c:/gtkmm/include/glibmm",
                "c:/gtkmm/include/cairomm",
                "c:/gtkmm/include",
                "c:/local/boost_1_64_0",
                "c:/Program Files (x86)/OpenAL 1.1 SDK/include",
                "c:/msvc/include",
            }
            configuration { "windows", "Debug" }
                libdirs {
                    "c:/msvc/lib32/debug"
                }
            configuration "windows"
            includedirs {
                "C:/Python27/include",
            }
            libdirs {
                "c:/Program Files (x86)/OpenAL 1.1 SDK/libs/Win32",
                "c:/msvc/lib32",
                "c:/gtkmm/lib",
                "C:/local/boost_1_64_0/lib32-msvc-14.1",
                "C:/Python27/libs",
            }
            -- buildoptions {
                -- "/MP",
                -- "/Gm-",
            -- }
            
            configuration { "windows", "Debug*" }
                links {
                    "RakNet_VS2008_LibStatic_Debug_Win32",
                    --"BulletSoftBody_vs2010",
                    "BulletDynamics_vs2010_debug",
                    "BulletCollision_vs2010_debug",
                    "LinearMath_vs2010_debug",
                }
            configuration {}
            configuration { "windows", "Release*" }
                links {
                    "RakNet_VS2008_LibStatic_Release_Win32",
                    --"BulletSoftBody_vs2010",
                    "BulletDynamics_vs2010",
                    "BulletCollision_vs2010",
                    "LinearMath_vs2010",
                }

    project "qor"
        kind "WindowedApp"
        language "C++"

        -- Project Files
        files {
            "Qor/**.cpp",
            "lib/kit/kit/**.cpp",
        }

        -- Exluding Files
        excludes {
            "Qor/tests/**",
            "Qor/scripts/**",
            "Qor/addons/**",
            "lib/kit/tests/**",
            "lib/kit/toys/**",
            "lib/kit/lib/tests/**",
        }
        
        includedirs {
            "lib/kit",
            "lib/kit/lib/local_shared_ptr",
            "/usr/local/include/",
            "/usr/include/bullet/",
            "/usr/include/raknet/DependentExtensions"
        }

