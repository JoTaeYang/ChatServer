#pragma once

#include <string>
#include <vector>

class IRepository
{
public:
	virtual ~IRepository() = default;
	virtual void Get(const std::string& key, std::string& Out) = 0;

	virtual void MakeKey(std::string& Out, std::vector<std::string> params) = 0;
};
