#include <stdint.h>

typedef int8_t   i8;
typedef int16_t  i16;
typedef int32_t  i32;
typedef int64_t  i64;
typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef float    f32;
typedef double   f64;
typedef int8_t   bool8;
typedef int16_t  bool16;
typedef int32_t  bool32;

#define global_variable static
#define local_scoped_global static // global variable with local scope
#define internal static // Function internal to the obj, file 

#define true 1
#define false 0

#define Kilobytes(value) ((value)*1024LL)
#define Megabytes(value) (Kilobytes(value)*1024LL)
#define Gigabytes(value) (Megabytes(value)*1024LL)
#define Terabytes(value) (Gigabytes(value)*1024LL)

#define Log(text, ...)         ConsoleLog(text, __VA_ARGS__)
#define LogInfo(text, ...)     ConsoleLogExtra("INFO:    ", text, __VA_ARGS__)
#define LogSuccess(text, ...)  ConsoleLogExtra("SUCCESS: ", text, __VA_ARGS__)
#define LogError(text, ...)    ConsoleLogExtra("ERROR %s %s %d: ", text, __FILE__, __FUNCTION__, __LINE__, __VA_ARGS__)
#define assert(expression)     if(!expression) PrivateSetDebuggerBreakpoint("ASSERT")
#define Assert(expression,text)if(!expression) PrivateSetDebuggerBreakpoint(text)
#define dbg()                  PrivateSetDebuggerBreakpoint("BREAKPOINT") 

typedef struct v2
{
    f32 x;
    f32 y;
} v2;

#if _MSC_VER
    #define PrivateSetDebuggerBreakpoint(text) {LogError(text); __debugbreak();}
    #define SilentSetDebuggerBreakpoint() {__debugbreak();}
#else
    #define PrivateSetDebuggerBreakpoint(text) {LogError(text); *(volatile int *)0 = 0;}
    #define SilentSetDebuggerBreakpoint() { *(volatile int *)0 = 0;}
#endif