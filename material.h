#ifndef MATERIAL_H_INCLUDED
#define MATERIAL_H_INCLUDED

#include "item.h"
#include "resource.h"

class Material : public Item
{
    bool stackable;

public:
    Material();
    Material(int materialTemplateID);
    ~Material();
};

enum enumMaterialTemplateIDs
{
    MATERIAL_TEMPLATE_WOOD = 0
};

#endif // MATERIAL_H_INCLUDED
