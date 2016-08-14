#ifndef CONTROL_H_INCLUDED
#define CONTROL_H_INCLUDED

#include <iostream>
#include <allegro5/allegro.h>
#include "gamesystem.h"

enum keyInputKeys
{
    KEY_PAD_1, KEY_PAD_2, KEY_PAD_3,            //movement
    KEY_PAD_4, KEY_PAD_5, KEY_PAD_6,
    KEY_PAD_7, KEY_PAD_8, KEY_PAD_9,
    KEY_PAD_0,
    KEY_UP, KEY_LEFT, KEY_DOWN, KEY_RIGHT,      //movement
    KEY_A, KEY_B, KEY_C,
    KEY_D, KEY_E, KEY_F,
    KEY_G, KEY_H, KEY_I,
    KEY_J, KEY_K, KEY_L,
    KEY_M, KEY_N, KEY_O,
    KEY_P, KEY_Q, KEY_R,
    KEY_S, KEY_T, KEY_U,
    KEY_V, KEY_W, KEY_X,
    KEY_Y, KEY_Z


};
extern bool keyInput[];

void Control();


#endif // CONTROL_H_INCLUDED
