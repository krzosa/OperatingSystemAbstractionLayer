#define OS_INTERFACE_IMPLEMENTATION
#include "win32_platform_executable.c"
#include "math_library.h"
#include "memory_storage.c"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "opengl_renderer.cpp"

struct game_state
{
    
};

// Called on the start of the app
void Initialize(OperatingSystemInterface *os)
{
    memory_storage *storage = (memory_storage *)os->memory;
    storage->memory = (u8 *)(storage + 1);
    storage->maxSize = os->memorySize;

    StorageReset(storage);
    opengl_renderer *gl = StoragePushStruct(storage, opengl_renderer);
    
    OpenGLRendererInitialize(gl);
    OpenGLRendererAttach(gl);
}

// Called on every frame
void Update(OperatingSystemInterface *os)
{
    memory_storage *storage = (memory_storage *)os->memory;
    storage->maxSize = os->memorySize;

    StorageReset(storage);
    opengl_renderer *gl = StoragePushStruct(storage, 
                                            opengl_renderer);
    
    
    if(KeyTap(KEY_F1))
    {
        os->WindowSetTransparency(40);
        os->WindowAlwaysOnTop();
    }
    if(KeyTap(KEY_F2))
    {
        os->WindowSetTransparency(255);
        os->WindowNotAlwaysOnTop();
    }
    
    m4x4 projectionMatrix = OrtographicProjectionMatrix(0, 1280, 0, 720, 0, 500);
    ShaderUniform(gl->basicShader, "viewProjectionMatrix", projectionMatrix);
    
    DrawBegin();
    {
        DrawRectangle({200, 200, 300, 300}, {0,0.7f,0.5f,1.f});
    }
    DrawEnd();
}

// Called when you recomplile while the app is running
void HotReload(OperatingSystemInterface *os)
{
    // NOTE: we need to call those on every reload because dll loses all memory
    // when we reload so the global variables get invalidated
    OSAttach(os);
    
    memory_storage *storage = (memory_storage *)os->memory;
    storage->maxSize = os->memorySize;

    StorageReset(storage);
    opengl_renderer *gl = StoragePushStruct(storage, opengl_renderer);
    OpenGLRendererAttach(gl);
    OpenGLRendererInitialize(gl);
}

// Called when you recomplile while the app is running
void HotUnload(OperatingSystemInterface *os)
{
    LogInfo("HotUnload");
    OpenGLRendererDestroy();
}