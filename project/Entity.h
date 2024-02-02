#pragma once
#include <string>
#include <memory>
#include "Components.h"
class EntityCls
{
private:
	std::string Tag;
	bool IsActive;
	size_t Id;

	EntityCls(const std::string& tag, const size_t id);

public:
	std::shared_ptr<CTransform>    cTransform		 = nullptr;
	std::shared_ptr<CShape>        cShape			 = nullptr;
	std::shared_ptr<CScore>        cScore			 = nullptr;
	std::shared_ptr<CCollision>    cCollision		 = nullptr;
	std::shared_ptr<CLifeSpan>     cLifeSpan	     = nullptr;
	std::shared_ptr<CInput>        cInput            = nullptr;
	std::shared_ptr<CSpecialShoot> cSpecialShoot     = nullptr;

	void Destroy();
	const std::string & GetTag() const;
	bool GetIsActive() const;
	const size_t GetId() const;

	friend class EntityManagerCls;
};

