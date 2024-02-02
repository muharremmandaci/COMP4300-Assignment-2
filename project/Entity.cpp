#include "Entity.h"

EntityCls::EntityCls(const std::string& tag, const size_t id)
	: Tag(tag), IsActive(true), Id(id) {}

void EntityCls::Destroy()
{
	IsActive = false;
}

const std::string & EntityCls::GetTag() const
{
	return Tag;
}

bool EntityCls::GetIsActive() const
{
	return IsActive;
}

const size_t EntityCls::GetId() const
{
	return Id;
}
