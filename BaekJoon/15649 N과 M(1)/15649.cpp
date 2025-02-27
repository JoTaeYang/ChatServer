#include <iostream>
#include <vector>
#include <algorithm>


void recur(int n, int m, std::vector<int> in, std::vector<bool> checks)
{
	if (in.size() == m)
	{
		// output
		for (const int& value : in)
		{
			std::cout << value << " ";
		}
		std::cout << "\n";
		return;
	}

	for (int i = 1; i <= n; ++i)
	{
		if (!checks[i])
		{
			in.push_back(i);
			checks[i] = true;
			recur(n, m, in, checks);
			in.pop_back();
			checks[i] = false;
		}
	}

}


int main() {
	std::ios::sync_with_stdio(false); std::cin.tie(NULL); std::cout.tie(NULL);

	int n, m;

	std::cin >> n >> m;

	std::vector<int> in;
	std::vector<bool> checks(n + 1);

	recur(n, m, in, checks);

	return 0;
}
