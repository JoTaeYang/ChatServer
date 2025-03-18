#include "Library/DB/CRedisDB.h"

#include <iostream>
#include <sstream>

#include <sw/redis++/redis.h>
#include <sw/redis++/redis_cluster.h>
#include <nlohmann/json.hpp>

#include "Library/Setting/CSetting.h"
#include "Library/DB/IRepository.h"

using json = nlohmann::json;

struct MyData {
	std::string Uid;
	int ShardIdx;
	
	// JSON 변환을 위한 매크로
	NLOHMANN_DEFINE_TYPE_INTRUSIVE(MyData, Uid, ShardIdx)
};

using namespace sw::redis;

CRedisDB::CRedisDB(CSetting* setting) : _setting(setting)
{

}

bool CRedisDB::Connect()
{
	ConnectionOptions connection_options;
	connection_options.host = _setting->redisIP;  // 필수.
	connection_options.port = _setting->redisPort; // 선택 사항. 기본 포트는 6379.

	ConnectionPoolOptions poolOptions;
	poolOptions.size = 10;
	
	try
	{
		this->cluster = std::make_shared<RedisCluster>(connection_options, poolOptions);
		std::cout << cluster.get()->redis("key", false).ping() << "\n";

		//sw::redis::OptionalString check = cluster->get("bfgames:a559e17d4e5a427a877e77a6faeaa998:session");
		//if (check.has_value())
		//{
		//	std::cout << check.value() << "\n";

		//	json j = json::parse(check.value());
		//	MyData data = j.get<MyData>();

		//	std::cout << "ID: " << data.Uid << ", Shard Idx :" << data.ShardIdx << "\n";
		//}		
		return true;
	}
	catch (const sw::redis::Error& e)
	{
		std::cerr << e.what() << std::endl;
		return false;
	}
		
	return true;
}

void CRedisDB::Get(const std::string& key, std::string& Out)
{
	sw::redis::OptionalString ret = cluster->get(key);	

	if (ret.has_value())
		Out = std::move(ret.value());
}

void CRedisDB::MakeKey(std::string& Out, std::vector<std::string> params)
{
	std::stringstream ss;

	ss << _setting->redisAppName;

	for (const std::string& param : params)
	{
		ss << ":" << param;
	}

	Out = ss.str();
}

