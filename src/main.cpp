#include <stdio.h>
#include <utility>
#include <ecs/ecs.hpp>

struct Subject
{
    int dummy;

    Subject() : dummy{0}
    {
        printf("Subject was default constructed.\n");
    }

    Subject(int x) : dummy{x}
    {
        printf("Subject was constructed with arg %d\n", x);
    }

    Subject(const Subject& other) : dummy{other.dummy}
    {
        printf("Subject was copied.\n");
    }

    Subject(Subject&& other) noexcept : dummy{other.dummy}
    {
        other.dummy = 0;
        printf("Subject was moved.\n");
    }

    ~Subject()
    {
        printf("Subject %d was destroyed.\n", dummy);
    }
};

int main()
{
    using EntityId = unsigned long long;

    ECS ecs {10};
    EntityId id {ecs.newEntity()};
    printf("Created entity %d.\n", id);

    ecs.addComponent<Subject>(id, 1);
    
    Subject& subj {ecs.getComponent<Subject>(id)};
    subj.dummy = 10;

    ecs.removeComponent<Subject>(id);

    printf("End of main.\n");
    return 0;
}