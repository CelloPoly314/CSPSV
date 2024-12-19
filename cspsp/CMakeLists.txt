cmake_minimum_required(VERSION 3.2)

if(NOT DEFINED CMAKE_TOOLCHAIN_FILE)
  if(DEFINED ENV{VITASDK})
    set(CMAKE_TOOLCHAIN_FILE "$ENV{VITASDK}/share/vita.toolchain.cmake" CACHE PATH "toolchain file")
  else()
    message(FATAL_ERROR "Please define VITASDK to point to your SDK path!")
  endif()
endif()

project(CSPSP)
include("${VITASDK}/share/vita.cmake" REQUIRED)

find_package(SDL2 REQUIRED)
find_package(SDL2_mixer REQUIRED)
find_package(PNG REQUIRED)
find_package(ZLIB REQUIRED)

find_library(SDL2_MIXER_LIB NAMES SDL2_mixer PATHS ${VITASDK}/lib)

set(VITA_APP_NAME "CSPSP")
set(VITA_TITLEID  "CSPSP0001")
set(VITA_VERSION  "01.00")

SET(DATA_PATH "ux0:data/CSPSP" CACHE STRING "Path to data files")

# Set C and C++ standard
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=gnu11 -mthumb")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")


set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS}")

add_definitions(-DGLM_ENABLE_EXPERIMENTAL)

include_directories(
  ../JGE/include
  ../JGE/include/hge
  ${VITASDK}/include
  ${VITASDK}/include/libpng16
  ${SDL2_INCLUDE_DIRS}
  ${CMAKE_SOURCE_DIR}/src
)

file(GLOB SOURCE_FILES
  ${CMAKE_SOURCE_DIR}/src/*.cpp
  ${CMAKE_SOURCE_DIR}/src/*.c
  ${CMAKE_SOURCE_DIR}/src/*.h
)


add_executable(${PROJECT_NAME} ${SOURCE_FILES}

)

target_link_libraries(${PROJECT_NAME}
  c
  ${CMAKE_SOURCE_DIR}/../JGE/build/lib/libjge.a
  sndfile
  SDL2::SDL2
  openal
  mpg123
  pthread
  png16
  ZLIB::ZLIB
  vitaGL
  modplug
  opusfile
  opus
  vorbisfile
  vorbis
  ogg
  mikmod
  FLAC
  xmp
  vitashark
  SceShaccCgExt
  SceShaccCg_stub
  SceSysmodule_stub
  SceCommonDialog_stub
  SceGxm_stub
  SceDisplay_stub
  SceAppMgr_stub
  SceNet_stub 
  mathneon
  taihen_stub
  SceKernelDmacMgr_stub
  SceAvPlayer_stub
  SceAudio_stub
  SceCtrl_stub
  z
  m
)

vita_create_self(${PROJECT_NAME}.self ${PROJECT_NAME})
vita_create_vpk(${PROJECT_NAME}.vpk ${VITA_TITLEID} ${PROJECT_NAME}.self
  VERSION ${VITA_VERSION}
  NAME ${VITA_APP_NAME}
  FILE sce_sys/icon0.png sce_sys/icon0.png
  FILE sce_sys/livearea/contents/bg.png sce_sys/livearea/contents/bg.png
  FILE sce_sys/livearea/contents/startup.png sce_sys/livearea/contents/startup.png
  FILE sce_sys/livearea/contents/template.xml sce_sys/livearea/contents/template.xml
  FILE simple.frag simple.frag
  FILE simple.vert simple.vert
  FILE sprite.frag sprite.frag
  FILE sprite.vert sprite.vert
)
