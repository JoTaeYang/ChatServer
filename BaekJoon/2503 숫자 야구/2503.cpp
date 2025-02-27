#include <iostream>
#include <vector>
#include <algorithm>
#include <string>

struct BaseBall
{
	std::string num;
	int strike;
	int ball;
};

bool recur(std::vector<BaseBall> infos, int num)
{
	std::string tmpNumStr = std::to_string(num);

	for (const BaseBall& info : infos)
	{
		int strike = 0;
		int ball = 0;
		for (int i = 0; i < info.num.size(); ++i)
		{
			if (info.num[i] == tmpNumStr[i])
			{
				++strike;
			}
			else if (info.num.find(tmpNumStr[i]) != std::string::npos)
			{
				++ball;
			}
		}

		if (info.strike != strike || info.ball != ball)
			return false;
	}
	return true;
}


int main() {
	std::ios::sync_with_stdio(false); std::cin.tie(NULL); std::cout.tie(NULL);

	int n;

	std::cin >> n;

	std::vector<BaseBall> infos(n);

	for (int i = 0; i < n; i++)
	{
		std::cin >> infos[i].num >> infos[i].strike >> infos[i].ball;
	}

	int count = 0;
	for (int a = 0; a <= 9; ++a)
		for (int b = 0; b <= 9; ++b)
			for (int c = 0; c <= 9; ++c)
			{
				if (a > 0 && b > 0 && c > 0)
					if (a != b && b != c && a != c)
					{
						if (recur(infos, (a * 100) + (b * 10) + c))
							++count;
					}
			}


	std::cout << count;

	return 0;
}
