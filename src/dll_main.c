#include "shared_custom.h"
#include "shared_operating_system_interface.h"
#include "operating_system_interface.c"

#define ConsoleLog(text, ...) os->log(text, __VA_ARGS__)
#define ConsoleLogExtra(prepend, text, ...) os->logExtra(prepend, text, __VA_ARGS__)

#include "opengl.h"
#include "opengl.c"


void Initialize(operating_system_interface *operatingSystemInterface)
{
    os = operatingSystemInterface;
    OpenGLFunctionsLoad(os->OpenGLFunctionLoad);

    // char *vertexShaderSource = 
    //     "#version 330 core\n"
    //     "layout (location = 0) in vec3 aPos;"
    //     "void main()"
    //     "{"
    //     "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);"
    //     "}\0";

    // char *fragmentShaderSource =
    //     "#version 330 core\n"
    //     "out vec4 FragColor;"
    //     "void main(){"
    //         "FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);}\0";

    // u32 shaders[2];
    // u32 shaderCount = 0;

    // shaders[shaderCount++] = ShaderCreate(GL_VERTEX_SHADER, &vertexShaderSource);
    // shaders[shaderCount++] = ShaderCreate(GL_FRAGMENT_SHADER, &fragmentShaderSource);

    // u32 shaderProgram = ProgramCreate(shaders, shaderCount);

    // u32 vertexBufferObject, vertexArrayObject;
    // glGenVertexArrays(1, &vertexArrayObject);
    // glGenBuffers(1, &vertexBufferObject); 

    // f32 vertices[] = {
    //     -0.5f, -0.5f, 0.0f,
    //     0.5f, -0.5f, 0.0f,
    //     0.0f,  0.5f, 0.0f
    // }; 

    // glBindVertexArray(vertexArrayObject);
    // glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);
    // glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    // glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    // glEnableVertexAttribArray(0);
    // glUseProgram(shaderProgram);
}
bool32 Update(operating_system_interface *operatingSystemInterface)
{
    glClearColor(0, 0.5, 0.5, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);

    if(IsKeyDown(KEY_W)) Log("W\n");
    if(IsKeyPressedOnce(KEY_ESC)) return 0;
    if(IsKeyUnpressedOnce(KEY_A)) Log("A\n");
    if(IsButtonDown(BUTTON_UP)) Log("A\n");
    if(IsButtonPressedOnce(BUTTON_DOWN)) Log("FF\n");
    if(IsButtonUnpressedOnce(BUTTON_LEFT)) Log("A\n");

    // glDrawArrays(GL_TRIANGLES, 0, 3);
    return 1;
}
void HotReload(operating_system_interface *operatingSystemInterface)
{
    os = operatingSystemInterface;
}