#include "regexp.hpp"

int main()
{
    RegExp reg("(aa)+|(bb)*");
    reg.setRule("(((((a)))))");
    reg.setRule("(((((a)))))bc");
    return 0;
}