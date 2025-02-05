#include <stdio.h>

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

    ~Subject()
    {
        printf("Subject was destroyed.\n");
    }
};

 func(int n)
{
    printf("End of func.\n");
    return n;
}

int main()
{
    const Subject& subj2s{func(2)};

    printf("End of main.\n");
    return 0;
}