#include "regexp.hpp"

int main()
{
    RegExp reg("(aa)+|(bb)*");

    if (reg.match("aaaa"))
    {
        printf("match aaaa\n");
    }

    if (reg.match("aaa"))
    {
        printf("match aaa\n");
    }
    else
    {
        printf("not match aaa\n");
    }
    

    return 0;
}