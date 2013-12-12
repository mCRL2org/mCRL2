#include <algorithm>

int main(void)
{
    int v[3];
    for(int i = 0; i < 3; ++i)
    {
      v[i] = i;
    }
    return !std::is_sorted(v, v+3);
}
