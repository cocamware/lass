#include <algorithm>
#include <vector>

int main()
{
    std::vector<size_t> xs;
    for (size_t k = 0; k < 5; ++k)
    {
        xs.push_back(k);
    }

    const size_t multiplier = 2;
    std::transform(xs.begin(), xs.end(), xs.begin(), [multiplier](size_t x) { return multiplier * x; });

    return xs.back() == 8 ? 1 : 0;
}
