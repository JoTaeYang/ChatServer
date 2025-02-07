#pragma once

class CSetting
{
public:
	CSetting(const std::string InFileName);

	int GetPort() const;

private:
	int port;
};

