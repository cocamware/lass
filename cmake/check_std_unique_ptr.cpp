#include <memory>

int foo(std::unique_ptr<int> p)
{
    return *p;
}

int main()
{
    std::unique_ptr<int> bar(new int(5));
    return foo(std::move(bar));
}
