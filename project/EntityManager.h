#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include "Entity.h"
#include <memory>
#include <algorithm>

typedef std::shared_ptr<EntityCls> EntityPtr;
typedef std::vector<EntityPtr> EntityVec;
typedef std::map<std::string, EntityVec> EntityMap;


class EntityManagerCls
{
private:
	EntityVec Entities;
	EntityMap EntitiesMap;
	EntityVec ToAddEntity;
	size_t TotalEntities = 0;

	void RemoveDeadEntities(EntityVec& entityVec);
public:
	EntityManagerCls();
	void Update();
	EntityPtr AddEntity(const std::string & tag);
	EntityVec& GetEntities();
	EntityVec& GetEntities(const std::string & tag);
	EntityMap& GetEntityMap();

};

