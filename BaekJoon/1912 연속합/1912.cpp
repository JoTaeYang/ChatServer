#include <iostream>
#include <vector>
#include <algorithm>

int main()
{
	int n, k;

	std::cin >> n >> k;

	std::vector<int> array(n);
	std::vector<int> Total(n);

	for (int i = 0; i < n; ++i)
		std::cin >> array[i];

	int checkIdx = k - 1;
	int j = 0;
	Total[0] = array[0];
	for (int i = 1; i < n; ++i)
	{
		if (i > checkIdx)
		{
			Total[i] = Total[i - 1] + array[i] - array[j];
			j++;
		}
		else
			Total[i] = Total[i - 1] + array[i];
	}

	std::sort(Total.begin() + checkIdx, Total.end());

	std::cout << *(Total.end() - 1);

	return 0;
}