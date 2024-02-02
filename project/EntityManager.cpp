#include "EntityManager.h"

EntityManagerCls::EntityManagerCls()
	:TotalEntities(0){}

void EntityManagerCls::RemoveDeadEntities(EntityVec& entityVec)
{
	auto it = remove_if(entityVec.begin(), entityVec.end(), [](const EntityPtr& e) {return e->GetIsActive() == false; });
	
	entityVec.erase(it, entityVec.end());
}

void EntityManagerCls::Update()
{
	for (auto& e : ToAddEntity) {
		Entities.push_back(e);

		EntitiesMap[e->GetTag()].push_back(e);
	}

	ToAddEntity.clear();

	RemoveDeadEntities(Entities);

	for (auto& e : EntitiesMap) {
		RemoveDeadEntities(e.second);
	}

}

EntityPtr EntityManagerCls::AddEntity(const std::string& tag)
{
	std::shared_ptr<EntityCls> entity(new EntityCls(tag, TotalEntities));
	TotalEntities++;
	ToAddEntity.push_back(entity);
	return entity;
}

EntityVec& EntityManagerCls::GetEntities()
{
	return Entities;
}

EntityVec& EntityManagerCls::GetEntities(const std::string& tag)
{
	return EntitiesMap[tag];
}

EntityMap& EntityManagerCls::GetEntityMap()
{
	return EntitiesMap;
}
