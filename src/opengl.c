// TODO: move maybe to dynamically allocated string, but probably I need a custom allocator first
#define ERROR_BUFFER_SIZE 1024

const char *vertexShaderSource = 
    "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;"
    "void main()"
    "{"
    "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);"
    "}\0";

const char *fragmentShaderSource =
    "#version 330 core\n"
    "out vec4 FragColor;"
    "void main(){"
        "FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);}\0";

internal u32 
ShaderCreate(GLenum shaderType, const char *nullTerminatedShaderFile)
{
    u32 shader = glCreateShader(shaderType);
    glShaderSource(shader, 1, &nullTerminatedShaderFile, NULL);
    glCompileShader(shader);

    char log[ERROR_BUFFER_SIZE];
    glGetShaderInfoLog(shader, ERROR_BUFFER_SIZE - 1, NULL, log);

    char *strShaderType = NULL;
    switch(shaderType)
    {
        case GL_VERTEX_SHADER: strShaderType = "Vertex"; break;
        case GL_GEOMETRY_SHADER: strShaderType = "Geometry"; break;
        case GL_FRAGMENT_SHADER: strShaderType = "Fragment"; break;
    }

    i32 status;
    glGetShaderiv(shaderType, GL_COMPILE_STATUS, &status);
    if (!status)
    {
        LogError("%s shader compilation %s", strShaderType, log);
    }
    else
    {
        LogSuccess("%s shader compiled", strShaderType);
    }

    return shader;
}

internal u32
ProgramCreate(u32 shaders[], u32 shaderCount)
{
    u32 shaderProgram = glCreateProgram();

    for(u32 i = 0; i != shaderCount; i++)
        glAttachShader(shaderProgram, shaders[i]);

    glLinkProgram(shaderProgram);

    i32 status;
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &status);
    if (!status)
    {
        char log[ERROR_BUFFER_SIZE];
        glGetProgramInfoLog(shaderProgram, ERROR_BUFFER_SIZE - 1, NULL, log);

        LogError("Create program %s", log);
    }

    for(u32 i = 0; i != shaderCount; i++)
        glDeleteShader(shaders[i]); 

    return shaderProgram;
}

internal void
OpenGLTriangleSetup()
{

    u32 shaders[2];
    u32 shaderCount = 0;

    shaders[shaderCount++] = ShaderCreate(GL_VERTEX_SHADER, vertexShaderSource);
    shaders[shaderCount++] = ShaderCreate(GL_FRAGMENT_SHADER, fragmentShaderSource);

    u32 shaderProgram = ProgramCreate(shaders, shaderCount);

    u32 vertexBufferObject, vertexArrayObject;
    glGenVertexArrays(1, &vertexArrayObject);
    glGenBuffers(1, &vertexBufferObject); 

    f32 vertices[] = {
        -0.5f, -0.5f, 0.0f,
        0.5f, -0.5f, 0.0f,
        0.0f,  0.5f, 0.0f
    }; 

    glBindVertexArray(vertexArrayObject);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);
    glUseProgram(shaderProgram);
}