#include "RPGServer/Datas/SessionInfo.h"

#include <iostream>
#include <sstream>
#include <nlohmann/json.hpp>

#include "Library/DB/CRedisDB.h"
#include "Library/DB/IRepository.h"

using json = nlohmann::json;

void SessionInfoRepository::GetUser(IRepository* Repository, std::string& SessionKey, SessionInfo& Out)
{
	std::string key = "";
	std::vector<std::string> params;
	params.reserve(2);
	params.push_back(SessionKey);
	params.push_back("session");

	Repository->MakeKey(key, params);

	std::string queryResult = "";

	Repository->Get(key, queryResult);

	if (!queryResult.empty())
	{
		json j = json::parse(queryResult);
		Out = j.get<SessionInfo>();
	}
}
