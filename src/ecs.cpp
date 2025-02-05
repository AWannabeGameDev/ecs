#include <ecs/ecs.hpp>

ECS::_ComponentDatabase::_ComponentDatabase(size_t componentByteSize, size_t maxComponents) :
    componentPool {componentByteSize, maxComponents}
{}

void ECS::_idExistsOrThrow(EntityId id)
{
    if(not entityExists(id))
    {
        throw std::invalid_argument {"This entity does not exist."};
    }
}

ECS::ECS(unsigned long long maxComponentsPerType) :
    _MAX_COMPONENTS {maxComponentsPerType}
{}

ECS::~ECS()
{
    for(auto& [_, database] : _componentDatabases)
    {
        for(auto& [_, componentBasePtr] : database.entityComponentPairs)
        {
            componentBasePtr->~ComponentBase();
            database.componentPool.deallocate((byte*)componentBasePtr);
        }
    }
}

ECS::EntityId ECS::newEntity()
{
    // Reuse a deleted ID if available
    if(deletedIds.size() != 0)
    {
        auto beginIter {deletedIds.begin()};
        EntityId newId {*beginIter};
        deletedIds.erase(beginIter);
        return newId;
    }
    else
    {
        return {_nextEntityId++};
    }
}

bool ECS::entityExists(EntityId id)
{
    return (id < _nextEntityId) && (not deletedIds.contains(id));
}

bool ECS::removeEntity(EntityId id)
{
    if(not entityExists(id))
    {
        return false;
    }

    // Iterate through all component databases and check if the component is possessed by the entity;
    // remove that component if it is
    for(auto& [_, database] : _componentDatabases)
    {
        if(database.entityComponentPairs.contains(id))
        {
            ComponentBase* componentBasePtr {database.entityComponentPairs[id]};

            componentBasePtr->~ComponentBase();

            database.componentPool.deallocate((byte*)componentBasePtr);
            database.entityComponentPairs.erase(id);
        }
    }

    deletedIds.emplace(id);
}