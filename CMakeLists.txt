cmake_minimum_required(VERSION 3.10)
project(QUASI88 VERSION 0.7.3 LANGUAGES C CXX)

if(APPLE)
    set(CMAKE_OSX_DEPLOYMENT_TARGET "13.0" CACHE STRING "Minimum macOS deployment target" FORCE)
endif()

# Options
option(USE_SOUND "Enable sound engine" ON)
option(USE_FMGEN "Enable fmgen (FM Sound Generator)" ON)
option(SUPPORT_DOUBLE "Support double size display" ON)
option(USE_MONITOR "Enable monitor mode (debugger)" ON)

# Find Dependencies
find_package(SDL2 REQUIRED)

# Include Directories
set(SRCDIR src)
set(INCDIRS
    ${SRCDIR}
    ${SRCDIR}/pc88
    ${SRCDIR}/screen
    ${SRCDIR}/screen/func
    ${SRCDIR}/screen/func/macro
    ${SRCDIR}/tk
    ${SRCDIR}/tk/q8tk
    ${SRCDIR}/ui
    ${SRCDIR}/ui/menu
    ${SRCDIR}/snddrv
    ${SRCDIR}/sysdepend
    ${SRCDIR}/osdepend
)

# Core Source Files
set(SOURCES
    ${SRCDIR}/alarm.c
    ${SRCDIR}/control.c
    ${SRCDIR}/debug.c
    ${SRCDIR}/emu.c
    ${SRCDIR}/fname.c
    ${SRCDIR}/getconf.c
    ${SRCDIR}/icon.c
    ${SRCDIR}/menu.c
    ${SRCDIR}/monitor.c
    ${SRCDIR}/pause.c
    ${SRCDIR}/quasi88.c
    ${SRCDIR}/utility.c
    ${SRCDIR}/pc88/basic.c
    ${SRCDIR}/pc88/crtcdmac.c
    ${SRCDIR}/pc88/fdc.c
    ${SRCDIR}/pc88/image.c
    ${SRCDIR}/pc88/intr.c
    ${SRCDIR}/pc88/keyboard.c
    ${SRCDIR}/pc88/memory.c
    ${SRCDIR}/pc88/pc88main.c
    ${SRCDIR}/pc88/pc88sub.c
    ${SRCDIR}/pc88/pio.c
    ${SRCDIR}/pc88/romaji.c
    ${SRCDIR}/pc88/soundbd.c
    ${SRCDIR}/pc88/suspend.c
    ${SRCDIR}/pc88/z80.c
    ${SRCDIR}/pc88/z80-debug.c
    ${SRCDIR}/screen/color.c
    ${SRCDIR}/screen/menu-screen.c
    ${SRCDIR}/screen/putimage.c
    ${SRCDIR}/screen/screen.c
    ${SRCDIR}/screen/screen-8bpp.c
    ${SRCDIR}/screen/screen-16bpp.c
    ${SRCDIR}/screen/screen-32bpp.c
    ${SRCDIR}/screen/screen-snapshot.c
    ${SRCDIR}/screen/snapshot.c
    ${SRCDIR}/tk/q8tk-draw.c
    ${SRCDIR}/tk/q8tk-event.c
    ${SRCDIR}/tk/q8tk-glib.c
    ${SRCDIR}/tk/q8tk.c
    ${SRCDIR}/tk/q8tk/q8tk-accel.c
    ${SRCDIR}/tk/q8tk/q8tk-adjustment.c
    ${SRCDIR}/tk/q8tk/q8tk-button.c
    ${SRCDIR}/tk/q8tk/q8tk-combo.c
    ${SRCDIR}/tk/q8tk/q8tk-common.c
    ${SRCDIR}/tk/q8tk/q8tk-container.c
    ${SRCDIR}/tk/q8tk/q8tk-entry.c
    ${SRCDIR}/tk/q8tk/q8tk-frame.c
    ${SRCDIR}/tk/q8tk/q8tk-fselection.c
    ${SRCDIR}/tk/q8tk/q8tk-fviewer.c
    ${SRCDIR}/tk/q8tk/q8tk-grab.c
    ${SRCDIR}/tk/q8tk/q8tk-item.c
    ${SRCDIR}/tk/q8tk/q8tk-label.c
    ${SRCDIR}/tk/q8tk/q8tk-listbox.c
    ${SRCDIR}/tk/q8tk/q8tk-misc.c
    ${SRCDIR}/tk/q8tk/q8tk-notebook.c
    ${SRCDIR}/tk/q8tk/q8tk-signal.c
    ${SRCDIR}/tk/q8tk/q8tk-widget.c
    ${SRCDIR}/tk/q8tk/q8tk-window.c
    ${SRCDIR}/ui/diskchange-top.c
    ${SRCDIR}/ui/menu-top.c
    ${SRCDIR}/ui/pause-top.c
    ${SRCDIR}/ui/quit-top.c
    ${SRCDIR}/ui/reset-top.c
    ${SRCDIR}/ui/speedup-top.c
    ${SRCDIR}/ui/statusbar.c
    ${SRCDIR}/ui/toolbar.c
    ${SRCDIR}/ui/menu/menu-about.c
    ${SRCDIR}/ui/menu/menu-common.c
    ${SRCDIR}/ui/menu/menu-cpu.c
    ${SRCDIR}/ui/menu/menu-disk.c
    ${SRCDIR}/ui/menu/menu-graph.c
    ${SRCDIR}/ui/menu/menu-key.c
    ${SRCDIR}/ui/menu/menu-misc.c
    ${SRCDIR}/ui/menu/menu-mouse.c
    ${SRCDIR}/ui/menu/menu-reset.c
    ${SRCDIR}/ui/menu/menu-tape.c
    ${SRCDIR}/ui/menu/menu-volume.c
)

# Sysdepend (SDL2) Source Files
set(SYSDEP_DIR ${SRCDIR}/sysdepend/sdl2)
list(APPEND SOURCES
    ${SYSDEP_DIR}/graph.c
    ${SYSDEP_DIR}/touchkey.c
    ${SYSDEP_DIR}/wait.c
    ${SYSDEP_DIR}/event.c
    ${SYSDEP_DIR}/main.c
)
list(APPEND INCDIRS ${SYSDEP_DIR})

# Platform-specific configurations
if(WIN32)
    set(OSDEP_DIR ${SRCDIR}/osdepend/win)
    list(APPEND SOURCES ${OSDEP_DIR}/file-op.c)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
else()
    set(OSDEP_DIR ${SRCDIR}/osdepend/unix)
    list(APPEND SOURCES ${OSDEP_DIR}/file-op.c)
    # These will be defined via target_compile_definitions later
endif()
list(APPEND INCDIRS ${OSDEP_DIR})
list(APPEND INCDIRS ${SRCDIR}/tk/q8tk/q8tk)

# Global definitions
add_definitions(-DQUASI88_SDL2 -DHAVE_INTPTR_T -DINLINE=static\ __inline__ -DCLIB_DECL=)
if(SUPPORT_DOUBLE)
    add_definitions(-DSUPPORT_DOUBLE)
endif()
if(USE_MONITOR)
    add_definitions(-DUSE_MONITOR)
endif()

# Sound Engine
if(USE_SOUND)
    set(XMAME_DIR ${SRCDIR}/snddrv/xmame)
    set(SD_Q88_DIR ${XMAME_DIR}/quasi88)
    set(SD_SDL_DIR ${XMAME_DIR}/quasi88/SDL)
    set(XM_SRC_DIR ${XMAME_DIR}/src)
    set(XM_SOUND_DIR ${XM_SRC_DIR}/sound)

    list(APPEND SOURCES
        ${SD_Q88_DIR}/mame-quasi88.c
        ${SD_Q88_DIR}/beep.c
        ${SD_Q88_DIR}/beepintf.c
        ${SD_Q88_DIR}/pcg.c
        ${SD_Q88_DIR}/pcgintf.c
        ${XM_SRC_DIR}/driver.c
        ${XM_SRC_DIR}/restrack.c
        ${XM_SRC_DIR}/sound.c
        ${XM_SRC_DIR}/sndintrf.c
        ${XM_SRC_DIR}/streams.c
        ${XM_SOUND_DIR}/flt_vol.c
        ${XM_SOUND_DIR}/flt_rc.c
        ${XM_SOUND_DIR}/wavwrite.c
        ${XM_SOUND_DIR}/2203intf.c
        ${XM_SOUND_DIR}/2608intf.c
        ${XM_SOUND_DIR}/ay8910.c
        ${XM_SOUND_DIR}/fm.c
        ${XM_SOUND_DIR}/ymdeltat.c
        ${XM_SOUND_DIR}/samples.c
        ${SD_SDL_DIR}/audio.c
        ${SD_SDL_DIR}/sdl.c
    )
    list(APPEND INCDIRS
        ${XMAME_DIR}
        ${SD_Q88_DIR}
        ${SD_SDL_DIR}
        ${XM_SRC_DIR}
        ${XM_SOUND_DIR}
    )
    add_definitions(-DUSE_SOUND -DPI=M_PI -DSYSDEP_DSP_SDL)

    if(USE_FMGEN)
        set(FMGEN_DIR ${SRCDIR}/snddrv/fmgen)
        list(APPEND SOURCES
            ${SD_Q88_DIR}/2203fmgen.cpp
            ${SD_Q88_DIR}/2608fmgen.cpp
            ${FMGEN_DIR}/fmgen.cpp
            ${FMGEN_DIR}/fmtimer.cpp
            ${FMGEN_DIR}/opna.cpp
            ${FMGEN_DIR}/psg.cpp
        )
        list(APPEND INCDIRS ${FMGEN_DIR})
        add_definitions(-DUSE_FMGEN)
    endif()
endif()

# Executable
add_executable(QUASI88 ${SOURCES})

if(APPLE)
    set(APP_ICON "${CMAKE_CURRENT_SOURCE_DIR}/AppIcon.png")

    set_target_properties(QUASI88 PROPERTIES
        MACOSX_BUNDLE TRUE
        MACOSX_BUNDLE_GUI_IDENTIFIER "com.bubio.quasi88"
        MACOSX_BUNDLE_BUNDLE_NAME "QUASI88"
        MACOSX_BUNDLE_DISPLAY_NAME "QUASI88"
        MACOSX_BUNDLE_SHORT_VERSION_STRING "${PROJECT_VERSION}"
        MACOSX_BUNDLE_BUNDLE_VERSION "1"
        MACOSX_BUNDLE_ICON_FILE "AppIcon.png"
        MACOSX_BUNDLE_INFO_PLIST "${CMAKE_CURRENT_SOURCE_DIR}/Info.plist.in"
    )

    set_source_files_properties(${APP_ICON} PROPERTIES
        MACOSX_PACKAGE_LOCATION "Resources"
    )

    target_sources(QUASI88 PRIVATE ${APP_ICON})
endif()

# Target Include Directories
target_include_directories(QUASI88 PRIVATE ${INCDIRS})

# Target Compilation Definitions
target_compile_definitions(QUASI88 PRIVATE
    QUASI88_SDL2
    HAVE_INTPTR_T
    "INLINE=static __inline__"
    "CLIB_DECL="
)

if(APPLE OR UNIX)
    target_compile_definitions(QUASI88 PRIVATE
        "ROM_DIR=\"~/Library/Application Support/quasi88/rom/\""
        "DISK_DIR=\"~/\""
        "TAPE_DIR=\"~/\""
        "SNAP_DIR=\"~/Pictures/quasi88/\""
        "STATE_DIR=\"~/Library/Application Support/quasi88/state/\""
        "L_CFG_DIR=\"~/Library/Application Support/quasi88/rc/\""
        "G_CFG_DIR=\"~/Library/Application Support/quasi88/\""
    )
endif()

if(SUPPORT_DOUBLE)
    target_compile_definitions(QUASI88 PRIVATE SUPPORT_DOUBLE)
endif()
if(USE_MONITOR)
    target_compile_definitions(QUASI88 PRIVATE USE_MONITOR)
endif()
if(USE_SOUND)
    target_compile_definitions(QUASI88 PRIVATE USE_SOUND "PI=M_PI" SYSDEP_DSP_SDL)
    if(USE_FMGEN)
        target_compile_definitions(QUASI88 PRIVATE USE_FMGEN)
    endif()
endif()

# Link Libraries
if(APPLE)
    target_link_libraries(QUASI88 PRIVATE SDL2::SDL2 "-framework Cocoa")
elseif(WIN32)
    target_link_libraries(QUASI88 PRIVATE SDL2::SDL2 SDL2::SDL2main)
else()
    target_link_libraries(QUASI88 PRIVATE SDL2::SDL2)
endif()

if(UNIX)
    target_link_libraries(QUASI88 PRIVATE m)
endif()

# Compiler-specific options
if(MSVC)
    target_compile_options(QUASI88 PRIVATE /utf-8)
endif()
# Optimization flags are handled by CMAKE_BUILD_TYPE (e.g. -O2 for Release, -g for Debug)

# Installation Rules
include(GNUInstallDirs)
install(TARGETS QUASI88
    # macOS は .app バンドルとしてルートに配置
    BUNDLE DESTINATION "$<$<BOOL:${APPLE}>:.>"
    # Windows は exe をインストールルートの直下に配置
    # Linux は標準の bin ディレクトリに配置
    RUNTIME DESTINATION "$<$<AND:$<BOOL:${WIN32}>,$<NOT:$<BOOL:${APPLE}>>>:.>$<$<NOT:$<BOOL:${WIN32}>>:$<IF:$<BOOL:${APPLE}>,.,${CMAKE_INSTALL_BINDIR}>>"
)


# CPack
set(CPACK_PACKAGE_NAME "QUASI88")
set(CPACK_PACKAGE_VENDOR "bubio")
set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "PC-8801 Emulator")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")

if(APPLE)
    set(CPACK_GENERATOR "DragNDrop")
elseif(WIN32)
    set(CPACK_GENERATOR "NSIS;ZIP")
else()
    set(CPACK_GENERATOR "DEB;RPM;TGZ")
    set(CPACK_DEBIAN_PACKAGE_MAINTAINER "bubio")
    set(CPACK_DEBIAN_PACKAGE_DEPENDS "libsdl2-2.0-0")
endif()

include(CPack)
