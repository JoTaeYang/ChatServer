#pragma once

class CSetting
{
public:
	void LoadSettings(const std::string InFileName);

	int serverPort;

	std::string redisIP;
	int redisPort;
	std::string redisAppName;
};

