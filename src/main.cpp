#include <stdio.h>
#include <utility>
#include "../include/pool_allocator.hpp"

struct Subject
{
    int dummy;

    Subject() : dummy{0}
    {
        printf("Subject was default constructed.\n");
    }

    Subject(int x) : dummy{x}
    {
        printf("Subject was constructed with args.\n");
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
        printf("Subject was destroyed.\n");
    }
};

int main()
{
    using std::byte;

    PoolAllocator alloc {sizeof(Subject), 10};

    Subject* newSubj = (Subject*)alloc.allocate();
    new (newSubj) Subject {0};

    for(int _ = 1; _ < 10; _++)
    {
        Subject* newSubj_ = (Subject*)alloc.allocate();
        new (newSubj_) Subject {_};
    }

    for(Subject* subj = newSubj; subj < (newSubj + 10); subj++)
    {
        printf("%d\n", subj->dummy);
    }

    alloc.deallocate((byte*)(newSubj + 4));
    alloc.deallocate((byte*)(newSubj + 6));
    new ((Subject*)alloc.allocate()) Subject {100};
    new ((Subject*)alloc.allocate()) Subject {200};

    for(Subject* subj = newSubj; subj < (newSubj + 10); subj++)
    {
        printf("%d\n", subj->dummy);
    }
}