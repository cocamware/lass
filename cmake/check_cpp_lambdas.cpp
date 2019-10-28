int main()
{
    int multiplier = 2;
    return ([multiplier](int x) { return multiplier * x; })(3) == 6 ? 1 : 0;
}
