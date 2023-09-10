#if defined(_MSC_VER)
#   define DLLEXPORT __declspec( dllexport )
#else
#   define DLLEXPORT
#endif

// cppcheck-suppress unusedFunction

extern "C" DLLEXPORT float multiply(float a, float b)
{
    return a * b;
}
