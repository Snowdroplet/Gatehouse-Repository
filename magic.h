#ifndef MAGIC_H_INCLUDED
#define MAGIC_H_INCLUDED

#include "item.h"

class Magic : public Item
{
public:

    Magic(); // Produces a useless template weapon.
    Magic(int magicTemplateID);
    ~Magic();

};

#endif // MAGIC_H_INCLUDED
