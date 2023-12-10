
#if defined(__cplusplus)
extern "C" {
#endif

#if defined(_WINDOWS)
void dbg(const char *msg, ...);
#else
#define dbg(msg, ...) printf("\nDEBUG\tFile:%s Line:%d::" ,__FILE__, __LINE__); printf(msg, ##__VA_ARGS__)
#endif

#if defined(__cplusplus)
}
#endif