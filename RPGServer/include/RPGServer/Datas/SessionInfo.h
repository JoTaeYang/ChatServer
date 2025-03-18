#pragma once

#include <string>

#include <nlohmann/json.hpp>

using json = nlohmann::json;


class SessionInfo
{
public:
	std::string Uid;
	int ShardIdx;

	NLOHMANN_DEFINE_TYPE_INTRUSIVE(SessionInfo, Uid, ShardIdx)
};

class SessionInfoRepository
{
public:
	void GetUser(class IRepository* Repository, std::string& SessionKey, SessionInfo& Out);

};
