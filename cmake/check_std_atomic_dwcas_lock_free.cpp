#include <stdint.h>
#include <atomic>

struct alignas(2 * sizeof(void*)) double_pointer_t
{
    void* a;
    void* b;
};

static_assert(sizeof(double_pointer_t) == 2 * sizeof(void*),
    "double_pointer_t must be as wide as two pointers");

int main()
{
#if defined(__cpp_lib_atomic_is_always_lock_free)
    return std::atomic<double_pointer_t>::is_always_lock_free
#else
    std::atomic<double_pointer_t> ptr;
    return ptr.is_lock_free()
#endif
        ? 0
        : 1;
}