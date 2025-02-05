#ifndef ECS_HPP
#define ECS_HPP

#include <unordered_map>
#include <unordered_set>
#include <cstddef>
#include <typeindex>

#include "pool_allocator.hpp"
#include "component.hpp"

class ECS
{
private :
    using EntityId = unsigned long long;
    using byte = std::byte;
    using size_t = std::size_t;
    using type_index = std::type_index;

    // Stores components and which entities possess them
    // Note : I thought of having a separate container to associate entity IDs with components, with
    // std::unordered_map<EntityId, std::unordered_map<ComponentType, ComponentBase*>>. Like this,
    // we can remove an entity in constant time (see current implementation of entity removal below). 
    // But this would lead to tons of unordered maps since we can have tons of entities. I thought that'd be inefficient
    // compared to having one entity-to-component map per component type and having linear time entity removal
    // (linear in number of component types, which is not too bad). Do comment on this! Also, PoolAllocator 
    // isn't templated to store a particular type because if it was, _ComponentDatabase would also have to be 
    // templated and then I wouldn't be able to store it in a container.
    // Another note : I chose to use a pool allocator for storing components so that a) they can be stored contiguously,
    // b) pointer invalidation isn't an issue (as would be in a vector), c) removing an element doesn't shift all
    // subsequent elements (as in a vector), which is inefficient. Empty space is instead reused for future allocations.
    class _ComponentDatabase
    {
    public :
        PoolAllocator componentPool;
        std::unordered_map<EntityId, ComponentBase*> entityComponentPairs{};

        _ComponentDatabase(size_t componentByteSize, size_t maxComponents);
    };

    const unsigned long long _MAX_COMPONENTS;

    // components databases are stored in a container and not as a templated variable so that I can iterate through them.
    std::unordered_map<type_index, _ComponentDatabase> _componentDatabases{};
    
    // For handling creation and deletion of entities
    EntityId _nextEntityId {0};
    std::unordered_set<EntityId> deletedIds{};

    template <typename ComponentType>
    _ComponentDatabase& _getOrCreateDatabase()
    {
        return _componentDatabases.try_emplace(type_index {typeid(ComponentType)}, 
                                               sizeof(ComponentWrapper<ComponentType>), _MAX_COMPONENTS).first->second;
    }

    void _idExistsOrThrow(EntityId id);

public :
    ECS(unsigned long long maxComponentsPerType);

    ~ECS();

    ECS(const ECS&) = delete;
    ECS& operator=(const ECS&) = delete;

    EntityId newEntity();

    bool entityExists(EntityId id);

    template <typename ComponentType>
    bool entityHasComponents(EntityId id)
    {
        _ComponentDatabase& database {_getOrCreateDatabase<ComponentType>()};
        return database.entityComponentPairs.contains(id);
    }

    // Uses recursion through variadic template arguments
    template <typename ComponentType1, typename... ComponentType2s>
    bool entityHasComponents(EntityId id)
    {
        return entityHasComponents<ComponentType1>(id) ? entityHasComponents<ComponentType2s...>(id) : false;
    }

    template <typename ComponentType, typename... CtorArgTypes>
    bool addComponent(EntityId id, CtorArgTypes&&... ctorArgs)
    {
        _idExistsOrThrow(id);

        if(entityHasComponents<ComponentType>(id))
        {
            return false;
        }

        ComponentBase* newComponentBasePtr {(ComponentBase*)database.componentPool.allocate()};
        new (newComponentBasePtr) ComponentWrapper<ComponentType> {std::forward<CtorArgTypes>(ctorArgs)...};

        database.entityComponentPairs.try_emplace(id, newComponentBasePtr);

        return true;
    }

    template <typename ComponentType>
    ComponentType& getComponent(EntityId id)
    {
        _idExistsOrThrow(id);

        if(not entityHasComponents<ComponentType>(id))
        {
            throw std::invalid_argument {"This entity does not have the requested component."};
        }

        return ((ComponentWrapper<ComponentType>*)database.entityComponentPairs[id])->component;
    }

    template <typename ComponentType>
    bool removeComponent(EntityId id)
    {
        _idExistsOrThrow(id);

        if(not entityHasComponents<ComponentType>(id))
        {
            return false;
        }

        _ComponentDatabase& database {_getOrCreateDatabase<ComponentType>()};
        ComponentBase* componentBasePtr {database.entityComponentPairs[id]};

        componentBasePtr->~ComponentBase();
        database.componentPool.deallocate((byte*)componentBasePtr);

        database.entityComponentPairs.erase(id);

        return true;
    }

    bool removeEntity(EntityId id);
};

#endif