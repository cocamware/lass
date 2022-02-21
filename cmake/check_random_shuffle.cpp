#include <algorithm>
#include <vector>

int main()
{
    std::vector<int> numbers;
    for (int i = 0; i < 10; ++i)
        numbers.push_back(i);
    std::random_shuffle(numbers.begin(), numbers.end());
}