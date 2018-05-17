#include "control.h"
#include "allegrocustom.h"

bool keyInput[] =
{

    false, false, false,                        //ABC
    false, false, false,                        //DEF
    false, false, false,                        //GHI
    false, false, false,                        //JKL
    false, false, false,                        //MNO
    false, false, false,                        //PQR
    false, false, false,                        //STU
    false, false, false,                        //VWX
    false, false,                               //YZ
    false,                                      //Enter
    false, false, false,                        //123
    false, false, false,                        //456
    false, false, false,                        //789
    false,                                      //0
    false, false, false, false,                 //ULDR
    false                                       //Shift

};

void Control()
{

    if(ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
    {
        gameExit = true;
    }
    if(ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN)
    {
        al_get_mouse_state(&mouseState);
        if(mouseState.buttons & 1)
        {

        }
        if(mouseState.buttons & 2)
        {

        }
    }
    /*if(ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP)
    {
        al_get_mouse_state(&mouseState);
        if(!mouseState.buttons & 1)
        {

        }
    }
    if(ev.type == ALLEGRO_EVENT_MOUSE_AXES || ALLEGRO_EVENT_MOUSE_ENTER_DISPLAY)
    {
        mouseCurrentXPosition = al_get_mouse_state_axis(&mouseState,0); // mousestate,0 for x axis
        mouseCurrentYPosition = al_get_mouse_state_axis(&mouseState,1); // mousestate,1 for y axis

        al_get_mouse_state(&mouseState);
        if(mouseState.buttons & 2)
        {

        }

    }*/
    if(ev.type == ALLEGRO_EVENT_KEY_DOWN)
    {
        switch(ev.keyboard.keycode)
        {
        case ALLEGRO_KEY_A:
            keyInput[KEY_A] = true;
            break;
        case ALLEGRO_KEY_B:
            keyInput[KEY_B] = true;
            break;
        case ALLEGRO_KEY_C:
            keyInput[KEY_C] = true;
            break;
        case ALLEGRO_KEY_D:
            keyInput[KEY_D] = true;
            break;
        case ALLEGRO_KEY_E:
            keyInput[KEY_E] = true;
            break;
        case ALLEGRO_KEY_F:
            keyInput[KEY_F] = true;
            break;
        case ALLEGRO_KEY_G:
            keyInput[KEY_G] = true;
            break;
        case ALLEGRO_KEY_H:
            keyInput[KEY_H] = true;
            break;
        case ALLEGRO_KEY_I:
            keyInput[KEY_I] = true;
            break;
        case ALLEGRO_KEY_J:
            keyInput[KEY_J] = true;
            break;
        case ALLEGRO_KEY_K:
            keyInput[KEY_K] = true;
            break;
        case ALLEGRO_KEY_L:
            keyInput[KEY_L] = true;
            break;
        case ALLEGRO_KEY_M:
            keyInput[KEY_M] = true;
            break;
        case ALLEGRO_KEY_N:
            keyInput[KEY_N] = true;
            break;
        case ALLEGRO_KEY_O:
            keyInput[KEY_O] = true;
            break;
        case ALLEGRO_KEY_P:
            keyInput[KEY_P] = true;
            break;
        case ALLEGRO_KEY_Q:
            keyInput[KEY_Q] = true;
            break;
        case ALLEGRO_KEY_R:
            keyInput[KEY_R] = true;
            break;
        case ALLEGRO_KEY_S:
            keyInput[KEY_S] = true;
            break;
        case ALLEGRO_KEY_T:
            keyInput[KEY_T] = true;
            break;
        case ALLEGRO_KEY_U:
            keyInput[KEY_U] = true;
            break;
        case ALLEGRO_KEY_V:
            keyInput[KEY_V] = true;
            break;
        case ALLEGRO_KEY_W:
            keyInput[KEY_W] = true;
            break;
        case ALLEGRO_KEY_X:
            keyInput[KEY_X] = true;
            break;
        case ALLEGRO_KEY_Y:
            keyInput[KEY_Y] = true;
            break;
        case ALLEGRO_KEY_Z:
            keyInput[KEY_Z] = true;
            break;
        case ALLEGRO_KEY_ENTER:
            keyInput[KEY_ENTER] = true;
            break;
        case ALLEGRO_KEY_PAD_1:
            keyInput[KEY_PAD_1] = true;
            break;
        case ALLEGRO_KEY_PAD_2:
            keyInput[KEY_PAD_2] = true;
            break;
        case ALLEGRO_KEY_PAD_3:
            keyInput[KEY_PAD_3] = true;
            break;
        case ALLEGRO_KEY_PAD_4:
            keyInput[KEY_PAD_4] = true;
            break;
        case ALLEGRO_KEY_PAD_5:
            keyInput[KEY_PAD_5] = true;
            break;
        case ALLEGRO_KEY_PAD_6:
            keyInput[KEY_PAD_6] = true;
            break;
        case ALLEGRO_KEY_PAD_7:
            keyInput[KEY_PAD_7] = true;
            break;
        case ALLEGRO_KEY_PAD_8:
            keyInput[KEY_PAD_8] = true;
            break;
        case ALLEGRO_KEY_PAD_9:
            keyInput[KEY_PAD_9] = true;
            break;
        case ALLEGRO_KEY_PAD_0:
            keyInput[KEY_PAD_0] = true;
            break;
        case ALLEGRO_KEY_UP:
            keyInput[KEY_UP] = true;
            break;
        case ALLEGRO_KEY_DOWN:
            keyInput[KEY_DOWN] = true;
            break;
        case ALLEGRO_KEY_LEFT:
            keyInput[KEY_LEFT] = true;
            break;
        case ALLEGRO_KEY_RIGHT:
            keyInput[KEY_RIGHT] = true;
            break;
        case ALLEGRO_KEY_LSHIFT:
            keyInput[KEY_SHIFT] = true;
            break;
        case ALLEGRO_KEY_RSHIFT:
            keyInput[KEY_SHIFT] = true;
            break;
        }
    }
    if(ev.type == ALLEGRO_EVENT_KEY_UP)
    {
        switch(ev.keyboard.keycode)
        {
        case ALLEGRO_KEY_PAD_1:
            keyInput[KEY_PAD_1] = false;
            break;
        case ALLEGRO_KEY_PAD_2:
            keyInput[KEY_PAD_2] = false;
            break;
        case ALLEGRO_KEY_PAD_3:
            keyInput[KEY_PAD_3] = false;
            break;
        case ALLEGRO_KEY_PAD_4:
            keyInput[KEY_PAD_4] = false;
            break;
        case ALLEGRO_KEY_PAD_5:
            keyInput[KEY_PAD_5] = false;
            break;
        case ALLEGRO_KEY_PAD_6:
            keyInput[KEY_PAD_6] = false;
            break;
        case ALLEGRO_KEY_PAD_7:
            keyInput[KEY_PAD_7] = false;
            break;
        case ALLEGRO_KEY_PAD_8:
            keyInput[KEY_PAD_8] = false;
            break;
        case ALLEGRO_KEY_PAD_9:
            keyInput[KEY_PAD_9] = false;
            break;
        case ALLEGRO_KEY_PAD_0:
            keyInput[KEY_PAD_0] = false;
            break;
        case ALLEGRO_KEY_UP:
            keyInput[KEY_UP] = false;
            break;
        case ALLEGRO_KEY_DOWN:
            keyInput[KEY_DOWN] = false;
            break;
        case ALLEGRO_KEY_LEFT:
            keyInput[KEY_LEFT] = false;
            break;
        case ALLEGRO_KEY_RIGHT:
            keyInput[KEY_RIGHT] = false;
            break;
        case ALLEGRO_KEY_A:
            keyInput[KEY_A] = false;
            break;
        case ALLEGRO_KEY_B:
            keyInput[KEY_B] = false;
            break;
        case ALLEGRO_KEY_C:
            keyInput[KEY_C] = false;
            break;
        case ALLEGRO_KEY_D:
            keyInput[KEY_D] = false;
            break;
        case ALLEGRO_KEY_E:
            keyInput[KEY_E] = false;
            break;
        case ALLEGRO_KEY_F:
            keyInput[KEY_F] = false;
            break;
        case ALLEGRO_KEY_G:
            keyInput[KEY_G] = false;
            break;
        case ALLEGRO_KEY_H:
            keyInput[KEY_H] = false;
            break;
        case ALLEGRO_KEY_I:
            keyInput[KEY_I] = false;
            break;
        case ALLEGRO_KEY_J:
            keyInput[KEY_J] = false;
            break;
        case ALLEGRO_KEY_K:
            keyInput[KEY_K] = false;
            break;
        case ALLEGRO_KEY_L:
            keyInput[KEY_L] = false;
            break;
        case ALLEGRO_KEY_M:
            keyInput[KEY_M] = false;
            break;
        case ALLEGRO_KEY_N:
            keyInput[KEY_N] = false;
            break;
        case ALLEGRO_KEY_O:
            keyInput[KEY_O] = false;
            break;
        case ALLEGRO_KEY_P:
            keyInput[KEY_P] = false;
            break;
        case ALLEGRO_KEY_Q:
            keyInput[KEY_Q] = false;
            break;
        case ALLEGRO_KEY_R:
            keyInput[KEY_R] = false;
            break;
        case ALLEGRO_KEY_S:
            keyInput[KEY_S] = false;
            break;
        case ALLEGRO_KEY_T:
            keyInput[KEY_T] = false;
            break;
        case ALLEGRO_KEY_U:
            keyInput[KEY_U] = false;
            break;
        case ALLEGRO_KEY_V:
            keyInput[KEY_V] = false;
            break;
        case ALLEGRO_KEY_W:
            keyInput[KEY_W] = false;
            break;
        case ALLEGRO_KEY_X:
            keyInput[KEY_X] = false;
            break;
        case ALLEGRO_KEY_Y:
            keyInput[KEY_Y] = false;
            break;
        case ALLEGRO_KEY_Z:
            keyInput[KEY_Z] = false;
            break;
        case ALLEGRO_KEY_ENTER:
            keyInput[KEY_ENTER] = false;
            break;
        case ALLEGRO_KEY_LSHIFT:
            keyInput[KEY_SHIFT] = false;
            break;
        case ALLEGRO_KEY_RSHIFT:
            keyInput[KEY_SHIFT] = false;
            break;

        case ALLEGRO_KEY_ESCAPE:
            gameExit = true;
            break;

            /*
            case ALLEGRO_KEY_F1:
                debugMode = true;
                break;
            case ALLEGRO_KEY_F2:
                debugMode = false;
                break;
            case ALLEGRO_KEY_F3:
                break;
            */
        }
    }
}



