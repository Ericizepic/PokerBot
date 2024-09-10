#include <algorithm>
#include <vector>
#include <numeric>
#include <map>
#include <string>
#include <iostream>

using namespace std;

int main()
{
    std::vector<std::vector<int>> t = std::vector<std::vector<int>>(2, std::vector<int>(1));
    cout << t.size() << endl;
    cout << t[1].size() << endl;
    cout << t[1][0] << endl;
}