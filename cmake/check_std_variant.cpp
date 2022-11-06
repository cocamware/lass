#include <variant>

int main()
{
    std::variant<std::monostate, int, float> s;
    return 0;
}
