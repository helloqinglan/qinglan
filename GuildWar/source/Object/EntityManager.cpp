
// *-----------------------------------------------------------------
//
// ��Ϸʵ����������
//
// *-----------------------------------------------------------------

#include "GuildWarPch.h"
#include "Object/entity.h"
#include "Object/EntityManager.h"

// ***TODO*** ������·������̫��ª, ��ȷʵ�ֲ��뱩¶��includeĿ¼
//            �ɿ��������������, ������ʵ����ComponentĿ¼
#include "../Component/PropertySet/PropertySetImpl.h"

Entity* EntityManager::createEntity(Entity::EntityType eType)
{
	Entity* entity = new Entity(eType);

	// ***TODO*** �������ļ򵥴���, entity�д��������е�Component
	entity->m_componentList.push_back(new PropertySetImpl(entity));

	return 0;
}
