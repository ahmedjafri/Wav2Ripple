//
//  Globals.h
//  Racer
//
//  Created by Ahmed Jafri on 11/20/12.
//  Copyright (c) 2012 Ahmed Jafri. All rights reserved.
//

#ifndef Racer_Globals_h
#define Racer_Globals_h
#define ENABLE_DEBUG 1

#if ENABLE_DEBUG
#define LOG_MSG printf
#else
#define LOG_MSG(...)
#endif

#define DRAW_CALLS_PER_ITERATE 2

#define SHOW_KAMCORD_VIEW 0

#define BUFFER_OFFSET(i) ((char *)NULL + (i))

#include <stdio.h>
#include <math.h>
#include <assert.h>

#if __APPLE__
    #if TARGET_IPHONE_SIMULATOR
        #define IOS_DEVICE
        #include <GLKit/GLKMathTypes.h>
        #include <OpenGLES/ES2/gl.h>
    #elif TARGET_OS_IPHONE
        #define IOS_DEVICE
        #include <GLKit/GLKMathTypes.h>
        #include <OpenGLES/ES2/gl.h>
    #else
        #define MAC_DEVICE
        #include <OpenGL/gl3.h>
        #include <GLKit/GLKMathTypes.h>
        #include <OpenGL/gltypes.h>
        #define UITableViewDataSource NSTableViewDataSource
    #endif
#elif WIN32
    #include <GL/freeglut.h>
    #include <GL/gl.h>
#else
// Unsupported platform
#endif

struct Vertex3D
{
    float x;
    float y;
    float z;
};

#if defined(IOS_DEVICE) || defined(MAC_DEVICE)
struct OpenGLRenderMatrixArray
{
    GLKMatrix4 modelViewProjection;
    GLKMatrix3 normalMatrix;
};
#endif

enum MODEL_MATRIX
{
    STARS = 0,
    SPACESHIP = 1,
    FLOOR = 2,
    CUBES = 3,
    PLANE = 4,
    SKYBOX = 5,
    PROGRESSCUBE = 6
};

enum kBUFFER_OFFSET {
    INTENSITY = 0,
    FUNDAMENTAL_FREQUENCY = 1,
    FREQUENCY_OCTAVE = 2
};

#define SCALE 1

#if defined(IOS_DEVICE) || defined(MAC_DEVICE)
#define UNUSED_IDENTIFIER __unused
#else
#define UNUSED_IDENTIFIER 
#endif

static int RIPPLE_PROTOCOL UNUSED_IDENTIFIER = 3;
static int FIELDS_PER_VERTEX UNUSED_IDENTIFIER = 8; //position-3,normal-3,texture-2
//static char RIPPLE_FILE[12] __unused =  "song.ripple";
static float TIME_SCALE UNUSED_IDENTIFIER = SCALE;
static int SAMPLE_LENGTH UNUSED_IDENTIFIER = 2048 * SCALE;
long SAMPLE_RATE = 44100*SCALE;
#define TIME_INTERVAL (8192.0f / (float)SAMPLE_RATE)
static int BUFFER_OFFSETS UNUSED_IDENTIFIER = 4;

#endif
