
Internal i32
ShaderGetUniformLocation(ShaderProgram shader, char *uniformName)
{
    i32 uniformLocation = glGetUniformLocation(shader.id, uniformName);
    if(uniformLocation != -1)
    {
        return uniformLocation;
    }
    else
    {
        LogError("UniformLocation invalid uniform name");
        dbg;
        return -1;
    }
}

Internal void 
ShaderUniform(ShaderProgram shader, char *uniformName, i32 value)
{
    i32 uniformLocation = ShaderGetUniformLocation(shader, uniformName);
    glUniform1i(uniformLocation, value);
}

Internal void 
ShaderUniform(ShaderProgram shader, char *uniformName, M4x4 matrix)
{
    i32 uniformLocation = ShaderGetUniformLocation(shader, uniformName);
    glUniformMatrix4fv(uniformLocation, 1, GL_FALSE, &matrix.e[0][0]);
}

Internal void 
ShaderUniform(ShaderProgram shader, char *uniformName, i32 value[], i32 count)
{
    i32 uniformLocation = ShaderGetUniformLocation(shader, uniformName);
    glUniform1iv(uniformLocation, count, value);
}

Internal void 
ShaderUniform(ShaderProgram shader, char *uniformName, V4 vector)
{
    i32 uniformLocation = ShaderGetUniformLocation(shader, uniformName);
    glUniform4f(uniformLocation, vector.x, vector.y, vector.z, vector.w);
}

Internal void
ShaderUse(ShaderProgram shader)
{
    glUseProgram(shader.id);
}

Internal void
ShaderDelete(ShaderProgram shader)
{
    glDeleteProgram(shader.id);
}

Internal void
TextureDelete(Texture2D texture)
{
    glDeleteTextures(1, &texture.id);
}

Internal u32 
ShaderCreate(GLenum shaderType, const char *nullTerminatedShader)
{
    u32 shader = glCreateShader(shaderType);
    glShaderSource(shader, 1, &nullTerminatedShader, NULL);
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
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
    if (!status)
    {
        LogError("%s shader compilation %s", strShaderType, log);
    }
    else
    {
        LogSuccess("%s shader compiled", strShaderType);
    }
    GLPrintErrors();
    
    return shader;
}

Internal u32
ProgramCreate(u32 shaders[], u32 shaderCount)
{
    u32 shaderProgram;
    shaderProgram = glCreateProgram();
    
    for(u32 i = 0; i != shaderCount; i++)
    {
        glAttachShader(shaderProgram, shaders[i]);
    }
    
    glLinkProgram(shaderProgram);
    
    i32 status;
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &status);
    if (!status)
    {
        char log[ERROR_BUFFER_SIZE];
        glGetProgramInfoLog(shaderProgram, ERROR_BUFFER_SIZE - 1, NULL, log);
        
        LogError("%s", log);
    }
    
    for(u32 i = 0; i != shaderCount; i++)
        glDeleteShader(shaders[i]); 
    
    GLPrintErrors();
    
    return shaderProgram;
}


Internal ShaderProgram
ShaderCreate(const char *vertexShader, const char *fragmentShader)
{
    u32 vertexShaderId = ShaderCreate(GL_VERTEX_SHADER, vertexShader);       
    u32 fragmentShaderId = ShaderCreate(GL_FRAGMENT_SHADER, fragmentShader);       
    u32 shaders[2] = {vertexShaderId, fragmentShaderId};
    
    ShaderProgram result;
    result.id = ProgramCreate(shaders, 2);
    
    return result;
}

Internal Texture2D
TextureWhiteCreate()
{
    Texture2D result = {};
    u32 whiteColor = 0xffffffff;
    
    glGenTextures(1, &result.id);
    glBindTexture(GL_TEXTURE_2D, result.id);
    {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, &whiteColor);
    }
    
    
    return result;
}

Internal VertexRectangle
VertexForSymbol(Font *font, i32 symbol, V4 rectangle, V4 color, f32 textureIndex)
{
    VertexRectangle result = {};
    
    
    
    // TODO(KKrzosa): Idea 1 atlas should be built in such a way to make it as easy as possible to
    
    f32 x = rectangle.x;
    f32 y = rectangle.y;
    f32 w = rectangle.width + x;
    f32 h = rectangle.height + y;
    V4 box = font->symbols[symbol].atlasBoundingBox;
    
    result.vertices[0].position = {x, y, 0};
    result.vertices[0].color = color;
    result.vertices[0].textureCoordinate = {box.x1, box.y1};
    result.vertices[0].textureIndex = textureIndex;
    
    result.vertices[1].position = {x, h, 0};
    result.vertices[1].color = color;
    result.vertices[1].textureCoordinate = {box.x1, box.y2};
    result.vertices[1].textureIndex = textureIndex;
    
    result.vertices[2].position = {w, h, 0};
    result.vertices[2].color = color;
    result.vertices[2].textureCoordinate = {box.x2, box.y2};
    result.vertices[2].textureIndex = textureIndex;
    
    result.vertices[3].position = {w, y, 0};
    result.vertices[3].color = color;
    result.vertices[3].textureCoordinate = {box.x2, box.y1};
    result.vertices[3].textureIndex = textureIndex;
    
    return result;
}

Internal VertexRectangle
VertexForRectangle(f32 x, f32 y, f32 z, 
                   f32 width, f32 height, 
                   f32 textureIndex, V4 color)
{
    VertexRectangle result;
    
    f32 w = width + x;
    f32 h = height + y;
    
    result.vertices[0].position = {x, y, 0};
    result.vertices[0].color = color;
    result.vertices[0].textureCoordinate = {0.0f, 0.0f};
    result.vertices[0].textureIndex = textureIndex;
    
    result.vertices[1].position = {x, h, 0};
    result.vertices[1].color = color;
    result.vertices[1].textureCoordinate = {0.0f, 1.0f};
    result.vertices[1].textureIndex = textureIndex;
    
    result.vertices[2].position = {w, h, 0};
    result.vertices[2].color = color;
    result.vertices[2].textureCoordinate = {1.0f, 1.0f};
    result.vertices[2].textureIndex = textureIndex;
    
    result.vertices[3].position = {w, y, 0};
    result.vertices[3].color = color;
    result.vertices[3].textureCoordinate = {1.0f, 0.0f};
    result.vertices[3].textureIndex = textureIndex;
    
    // flipped
    // result.vertices[0].textureCoordinate = {1.0f, 1.0f};
    // result.vertices[1].textureCoordinate = {1.0f, 0.0f};
    // result.vertices[2].textureCoordinate = {0.0f, 0.0f};
    // result.vertices[3].textureCoordinate = {0.0f, 1.0f};
    
    return result;
}

Internal VertexRectangle
RectangleTexture(V4 rectangle, i32 textureIndex)
{
    VertexRectangle result = VertexForRectangle(rectangle.x, rectangle.y, 0, 
                                                rectangle.width, rectangle.height, 
                                                (f32)textureIndex, {1,1,1,1});
    
    return result;
}

Internal VertexRectangle
RectangleColor(V4 rectangle, V4 color)
{
    VertexRectangle result = VertexForRectangle(rectangle.x, rectangle.y, 0, 
                                                rectangle.width, rectangle.height, 
                                                0, color);
    
    return result;
}
