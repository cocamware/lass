#include <memory>

typedef std::allocator_traits< std::allocator<int> > test;

int main()
{
    return 0;
}
