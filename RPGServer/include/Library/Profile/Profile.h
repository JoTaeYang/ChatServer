#pragma once

#include <iostream>
#include <Windows.h>
#include <vector>

constexpr int MAX_SAMPLE = 100;
constexpr int MAX_PROFILE = 10;
struct ProfileInfo
{
	long lFlag;  //���������� ��� ����. (�迭 �ÿ���)	
	std::string name;

	LARGE_INTEGER lStartTime; // �������� ���� ���� �ð�

	std::vector<double> allTime;
	double iTotalTime;  // ��ü ��� �ð� ī����
	double iMin[2];  //�ּ� ��� �ð� ī����
	double iMax[2];  //�ִ� ��� �ð� ī����

	_int64 iCall;  //���� ȣ��
};


class ThreadSample
{
public:
	ThreadSample()
	{
		id = 0;
		flag = 0;
	}

	int id;
	int flag;

	ProfileInfo samples[MAX_SAMPLE];	
};


class Profile
{
public:
	static Profile& GetInstance();
	void Begin(const std::string& name);
	void End(const std::string& name);
	void ProfileDataOutText(const std::string& fileName);
	
private:
	int IsCheckName(int index, const std::string& name);
	int IsCheckThread(int id);
	void AddProfile(int index, const std::string& name);
	void StartProfile(int id, int index);
	void CheckMinMax(int id, int index, double time);
private:
	ThreadSample profile[MAX_PROFILE];

	Profile() {}
	~Profile() {}

	Profile(const Profile&) = delete;
	Profile& operator=(const Profile&) = delete;
	Profile(Profile&&) = delete;
	Profile& operator=(Profile&&) = delete;
};