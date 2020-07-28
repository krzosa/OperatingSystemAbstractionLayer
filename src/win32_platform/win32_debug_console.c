#define TEXT_BUFFER_SIZE 1024
global_variable HANDLE GLOBALConsoleHandle;

internal void 
ConsoleLog(char *text, ...)
{
    static char GLOBALRandomAccessTextBuffer[1][TEXT_BUFFER_SIZE];

    va_list args;
    va_start(args, text);
    vsprintf_s(GLOBALRandomAccessTextBuffer[0], TEXT_BUFFER_SIZE, text, args);
    va_end(args);

    size_t textLength = CharLength(GLOBALRandomAccessTextBuffer[0]);
    WriteConsole(GLOBALConsoleHandle, GLOBALRandomAccessTextBuffer[0], (DWORD)textLength, 0, 0);
}

internal void
ConsoleLogExtra(char *prepend, char *text, ...)
{
    static char GLOBALRandomAccessTextBuffer[2][TEXT_BUFFER_SIZE];

    size_t textLength = CharLength(text);
    size_t prependLength = CharLength(prepend);
    if(textLength + prependLength > TEXT_BUFFER_SIZE)
    {
        MessageBoxA(0, "Text buffer overflow", "ERROR", MB_OK);
        OutputDebugStringA("Text buffer overflow!\n");
        SilentSetDebuggerBreakpoint();
    }

    memcpy(GLOBALRandomAccessTextBuffer[1], prepend, prependLength);
    memcpy(GLOBALRandomAccessTextBuffer[1] + prependLength, text, textLength);
    GLOBALRandomAccessTextBuffer[1][prependLength + textLength] = '\0';

    va_list args;
    va_start(args, text);
    vsprintf_s(GLOBALRandomAccessTextBuffer[0], TEXT_BUFFER_SIZE, 
                GLOBALRandomAccessTextBuffer[1], args);
    va_end(args);

    textLength = CharLength(GLOBALRandomAccessTextBuffer[0]);
    GLOBALRandomAccessTextBuffer[0][textLength] = '\n';

    WriteConsole(GLOBALConsoleHandle, GLOBALRandomAccessTextBuffer[0], (DWORD)(textLength + 1), 0, 0);
}


// NOTE: Attaches to the console that invoked the application
//       if that fails it allocates a new console
internal void
Win32ConsoleAttach(void)
{
    // NOTE: Console Setup
    if(!AttachConsole(ATTACH_PARENT_PROCESS)) 
    {
        OutputDebugStringA("Failed to attach to console\n");
        if(!AllocConsole())
        {
            OutputDebugStringA("Failed to create a console\n");
        }
    }

    GLOBALConsoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    if(!GLOBALConsoleHandle) OutputDebugStringA("Failed to get console handle\n");
}

internal void
Win32LastErrorMessageLog(char *text)
{
    DWORD dLastError = GetLastError();
    LPSTR strErrorMessage = NULL;
    
    FormatMessageA(
        FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS | 
        FORMAT_MESSAGE_ARGUMENT_ARRAY | FORMAT_MESSAGE_ALLOCATE_BUFFER,
        NULL,
        dLastError,
        0,
        strErrorMessage,
        0,
        NULL);

    Log("%s: %s\n", text, strErrorMessage);

    LocalFree(strErrorMessage);
}
