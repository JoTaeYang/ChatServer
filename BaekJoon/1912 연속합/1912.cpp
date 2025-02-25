#include <iostream>
#include <vector>

int main()
{
	int n;

	std::cin >> n;

	std::vector<int> array(n);
	std::vector<int> Total(n);
	for (int i = 0; i < n; i++)
		std::cin >> array[i];

	/*
	loop를 돌면서 수를 더하다가
	어떤 기준을 두고 이 수를 버려서
	다시 수열을 시작을 해야함

	기준을 어떻게 잡는가가? 관건인듯하다

	10 6
	6이 되는 순간 수열을 버려야함.
	왜 버려야 하나? 이전 값보다 작아짐.

	근데 수열을 다시 시작할 때, 거의 높은 확률로 이전 값보다 작을 거라 거기서부터 고민이 시작
	*/
	int num = 0;
	int total = 0;
	int maxTotal = array[0];
	for (int i = 0; i < n; i++)
	{
		if (total < 0)
			total = 0;

		maxTotal = std::max(maxTotal, total + array[i]);
		total += array[i];
	}

	std::cout << maxTotal;

	return 0;
}