#pragma once


#include <sw/redis++/redis_cluster.h>
#include <string>

#include "Library/DB/IRepository.h"

class CSetting;

class CRedisDB : public IRepository
{
public:
	CRedisDB() = default;
	CRedisDB(CSetting* setting);

	bool Connect();
	
public:
	virtual void Get(const std::string& key, std::string& Out) override;
	virtual void MakeKey(std::string& Out, std::vector<std::string> params) override;

private:
	CSetting* _setting;
	std::shared_ptr<sw::redis::RedisCluster> cluster;
};