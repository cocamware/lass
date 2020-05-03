#include <algorithm>
#include <vector>
#include <memory>

typedef std::allocator_traits< std::allocator<int> > test;

int main()
{
    std::unique_ptr<size_t> unique(new size_t(2));
    std::shared_ptr<size_t> shared_multiplier(std::move(unique));

    std::vector<size_t> xs;
    for (size_t k = 0; k < 5; ++k)
    {
        xs.push_back(k);
    }
    std::transform(xs.begin(), xs.end(), xs.begin(), [shared_multiplier](size_t x) { return (*shared_multiplier) * x; });

    return xs.back() == 8 ? 1 : 0;
}
