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

        baseName = fullName = "Maple Leaf";
        spriteID = MASI_MAPLE_LEAF;

        description[0] = "Red leaf fallen from the branch of an";
        description[1] = "autumn maple.";
        description[2] = "";
        description[3] = "Their lively colour belies their curse;";
        description[4] = "unable to wither away, such leaves are";
        description[5] = "forever locked in false splendour.";
        break;

    case MATERIAL_TEMPLATE_MATERIA:

        baseName = fullName = "Materia";
        spriteID = MASI_MATERIA;

        description[0] = "Form without essence; essence without";
        description[1] = "form. Materia is everything, with the";
        description[2] = "potential to be nothing.";
        break;

    case MATERIAL_TEMPLATE_WOOD:

        baseName = fullName = "Wood";
        spriteID = MASI_WOOD;

        description[0] = "Hard flesh of a tree. Its ringed pattern";
        description[1] = "tells of long seasons weathered.";
        description[2] = "";
        description[3] = "Do you record in order to suffer?";
        description[4] = "Or do you suffer in order to record?";
        break;
    }
}

Material::~Material()
{

}
