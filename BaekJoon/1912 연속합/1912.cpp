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
	loop�� ���鼭 ���� ���ϴٰ�
	� ������ �ΰ� �� ���� ������
	�ٽ� ������ ������ �ؾ���

	������ ��� ��°���? �����ε��ϴ�

	10 6
	6�� �Ǵ� ���� ������ ��������.
	�� ������ �ϳ�? ���� ������ �۾���.

	�ٵ� ������ �ٽ� ������ ��, ���� ���� Ȯ���� ���� ������ ���� �Ŷ� �ű⼭���� ����� ����
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