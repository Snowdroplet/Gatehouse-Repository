#ifndef MISC_H_INCLUDED
#define MISC_H_INCLUDED

#include "item.h"

class Misc : public Item
{
public:

    Misc(); // Produces a useless template weapon.
    Misc(int miscTemplateID);
    ~Misc();

};

#endif // MISC_H_INCLUDED
