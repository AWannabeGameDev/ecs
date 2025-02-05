#include <vector>
#include <stdio.h>

//#define AFTER_11

struct Subject
{
    int dummy;

    Subject()
    {
        printf("Subject was default constructed.\n");
    }

    Subject(int)
    {
        printf("Subject was constructed with args.\n");
    }

    Subject(const Subject& other)
    {
        printf("Subject was copied.\n");
    }

    #ifdef AFTER_11
    Subject(Subject&& other) noexcept
    {
        printf("Subject was moved.\n");
    }
    #endif

    ~Subject()
    {
        printf("Subject was destroyed.\n");
    }
};

struct SubjectContainer
{
    Subject subj {};
};

int main()
{
    SubjectContainer lol {1};

    printf("End of main.\n");
    return 0;
}