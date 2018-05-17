#ifndef CONTROL_H_INCLUDED
#define CONTROL_H_INCLUDED

#include <iostream>
#include <allegro5/allegro.h>
#include "gamesystem.h"

enum keyInputKeys
{
    KEY_A = 0,  KEY_B = 1,  KEY_C = 2,
    KEY_D = 3,  KEY_E = 4,  KEY_F = 5,
    KEY_G = 6,  KEY_H = 7,  KEY_I = 8,
    KEY_J = 9,  KEY_K = 10, KEY_L = 11,
    KEY_M = 12, KEY_N = 13, KEY_O = 14,
    KEY_P = 15, KEY_Q = 16, KEY_R = 17,
    KEY_S = 18, KEY_T = 19, KEY_U = 20,
    KEY_V = 21, KEY_W = 22, KEY_X = 23,
    KEY_Y = 24, KEY_Z = 25,
    KEY_ENTER,
    KEY_PAD_1, KEY_PAD_2, KEY_PAD_3,            //movement
    KEY_PAD_4, KEY_PAD_5, KEY_PAD_6,
    KEY_PAD_7, KEY_PAD_8, KEY_PAD_9,
    KEY_PAD_0,
    KEY_UP, KEY_LEFT, KEY_DOWN, KEY_RIGHT,      //movement
    KEY_SHIFT

    // No KEY_ESC because escape currently sets exitgame = true
};
extern bool keyInput[];

void Control();


#endif // CONTROL_H_INCLUDED
