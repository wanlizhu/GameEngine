cmake_minimum_required (VERSION 3.1.0)
# Accept variable's value if it's defined prior to the `option()` command with the same name
set(CMAKE_POLICY_DEFAULT_CMP0077 NEW) 

project(Playground)

# Define macro
macro(remove_ignored_files FileListName)
    foreach(FilePath IN LISTS ${FileListName})
        get_filename_component(FileName "${FilePath}" NAME)
        if ("${FileName}" STREQUAL ".DS_Store")
            list(REMOVE_ITEM ${FileListName} ${FilePath})
        endif()
    endforeach()
endmacro()

macro(remove_files_with_suffix FileListName Suffix)
	foreach(FilePath IN LISTS ${FileListName})
        get_filename_component(BaseName "${FilePath}" NAME_WE)
		string(FIND "${BaseName}" "${Suffix}" Index) 
        if (NOT "${Index}" STREQUAL "-1")
            list(REMOVE_ITEM ${FileListName} ${FilePath})
        endif()
    endforeach()
endmacro()

# Initialize global/default properties
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(BUILD_SHARED_LIBS OFF)
set(CMAKE_DEBUG_POSTFIX "") 
set(CMAKE_RELEASE_POSTFIX "") 
set(PROJECT_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
set(BUILD_DIR "${CMAKE_CURRENT_BINARY_DIR}")
set(OUT_DIR "${BUILD_DIR}")

# Find platform-independent source files
if (CMAKE_SYSTEM_NAME MATCHES "Darwin")
    file(GLOB_RECURSE SourceFiles_Engine
        "${PROJECT_DIR}/Engine/OpenWorld/*.h"
        "${PROJECT_DIR}/Engine/OpenWorld/*.cpp"
        "${PROJECT_DIR}/Engine/OpenWorld/*.mm")
	remove_files_with_suffix(SourceFiles_Engine "_Vulkan")

    file(GLOB_RECURSE SourceFiles_Engine_Shaders
        "${PROJECT_DIR}/Assets/Shaders/*.metal")
    set_source_files_properties(${SourceFiles_Engine_Shaders} 
        PROPERTIES LANGUAGE METAL)
else()
    file(GLOB_RECURSE SourceFiles_Engine
        "${PROJECT_DIR}/Engine/OpenWorld/*.h"
        "${PROJECT_DIR}/Engine/OpenWorld/*.cpp")
	remove_files_with_suffix(SourceFiles_Engine "_Metal")
endif()

remove_ignored_files(SourceFiles_Engine)
remove_ignored_files(SourceFiles_Engine_Shaders)
source_group(TREE "${PROJECT_DIR}" PREFIX "" FILES ${SourceFiles_Engine})
source_group(TREE "${PROJECT_DIR}" PREFIX "" FILES ${SourceFiles_Engine_Shaders})

file(GLOB_RECURSE ResourceFiles "${PROJECT_DIR}/Assets/*.*")
remove_ignored_files(ResourceFiles)
source_group(TREE "${PROJECT_DIR}" PREFIX "" FILES ${ResourceFiles})

add_library(Engine STATIC ${SourceFiles_Engine})

target_include_directories(Engine PUBLIC "${PROJECT_DIR}/Engine")
target_include_directories(Engine PUBLIC "${PROJECT_DIR}/Assets/Shaders")
target_include_directories(Engine PUBLIC "${PROJECT_DIR}/ThirdParty")
target_include_directories(Engine PUBLIC "${PROJECT_DIR}/ThirdParty/glm/include")

target_compile_definitions(Engine PUBLIC $<$<CONFIG:DEBUG>:DEBUG>)
target_compile_definitions(Engine PUBLIC $<$<BOOL:${BUILD_SHARED_LIBS}>:BUILD_SHARED_LIBS>)

target_compile_options(Engine PUBLIC $<$<BOOL:${WIN32}>:/wd4244>)
target_compile_options(Engine PUBLIC $<$<BOOL:${WIN32}>:/wd4251>)
target_compile_options(Engine PUBLIC $<$<BOOL:${WIN32}>:/wd4267>)
target_compile_options(Engine PUBLIC $<$<BOOL:${WIN32}>:/wd4996>)

# Windows 32/64
if (WIN32)
    file(GLOB_RECURSE SourceFiles_Windows "${PROJECT_DIR}/Playground/Windows/*.*")
    remove_ignored_files(SourceFiles_Windows)
    source_group(TREE "${PROJECT_DIR}/" PREFIX "" FILES ${SourceFiles_Windows})
    
    add_executable(Playground ${SourceFiles_Windows} ${ResourceFiles})
	
	add_custom_command(TARGET Playground POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_directory "${PROJECT_DIR}/Assets" "${OUT_DIR}/$(Configuration)/Assets")

    if (MSVC)	
	    set_property(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}" PROPERTY 
            VS_STARTUP_PROJECT Playground)
    endif()
endif()

# macOS
if (CMAKE_SYSTEM_NAME MATCHES "Darwin")
    list(APPEND ResourceFiles "${PROJECT_DIR}/Playground/macOS/Main.storyboard")

    file(GLOB_RECURSE SourceFiles_macOS "${PROJECT_DIR}/Playground/macOS/*.*")
    remove_ignored_files(SourceFiles_macOS)
    source_group(TREE "${PROJECT_DIR}/Playground" PREFIX "" FILES ${SourceFiles_macOS})

    add_executable(Playground MACOSX_BUNDLE ${SourceFiles_macOS} ${ResourceFiles})

    # Available properties https://help.apple.com/xcode/mac/current/#/itcaec37c2a6
    set_target_properties(Playground PROPERTIES
	    MACOSX_BUNDLE TRUE
        MACOSX_BUNDLE_INFO_PLIST "${PROJECT_DIR}/Playground/macOS/Info.plist.in"
        MACOSX_BUNDLE_BUNDLE_NAME "Playground"
        MACOSX_BUNDLE_GUI_IDENTIFIER "com.wanlizhu.Playground_macOS"
        MACOSX_BUNDLE_ICON_FILE ""
        XCODE_ATTRIBUTE_GCC_WARN_64_TO_32_BIT_CONVERSION "NO"
        XCODE_ATTRIBUTE_CLANG_ENABLE_OBJC_ARC "YES"
        RESOURCE "${ResourceFiles}")

    set_target_properties(Engine PROPERTIES 
        XCODE_ATTRIBUTE_GCC_WARN_64_TO_32_BIT_CONVERSION "NO"
        XCODE_ATTRIBUTE_CLANG_ENABLE_OBJC_ARC "YES")

    target_link_libraries(Playground PRIVATE 
        "-framework Cocoa"
        "-framework Metal"
        "-framework MetalKit")
endif()

target_link_libraries(Playground PRIVATE Engine)