#ifndef KEY_H_INCLUDED
#define KEY_H_INCLUDED

#include "item.h"

class Key : public Item
{
public:

    Key(); // Produces a useless template weapon.
    Key(int keyTemplateID);
    ~Key();

};

#endif // KEY_H_INCLUDED
