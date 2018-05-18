#include "material.h"

Material::Material()
{

}

Material::Material(int materialTemplateID)
{
    derivedType = ITEM_TYPE_MATERIAL;

    stackable = true;

    switch(materialTemplateID)
    {
    case MATERIAL_TEMPLATE_MAPLE_LEAF:
        spriteID = MASI_MAPLE_LEAF;

        description = "Red leaf fallen from the branch of a maple tree in Autumn.";
        break;

    case MATERIAL_TEMPLATE_MATERIA:
        spriteID = MASI_MATERIA;

        description = "Form without essence, and essence without form. It is everything, with the potential to be nothing.";
        break;

    case MATERIAL_TEMPLATE_WOOD:
        spriteID = MASI_WOOD;

        description = "Hard flesh of a tree. Its ringed pattern tells of seasons weathered.";
        break;
    }
}

Material::~Material()
{

}
