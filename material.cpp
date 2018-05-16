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
    case MATERIAL_TEMPLATE_WOOD:
        spriteID = MASI_WOOD;
        break;
    }
}

Material::~Material()
{

}
