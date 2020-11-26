namespace Keyboard
{
    static const int SHFT_KEY = 0x10;
    static const int CTRL_KEY = 0x11;
    static const int  ALT_KEY = 0x12;
    static const int LOCK_KEY = 0x14;
    static const int LSTT_KEY = 0x5b;
    static const int RSTT_KEY = 0x5c;
    static const int GRID_KEY = 0x5d;

    static const int KEY_END = 35;
    static const int KEY_HOME = 36;


    static const int KEY_UP = 38;
    static const int KEY_DOWN = 40;
    static const int KEY_LEFT = 37;
    static const int KEY_RIGHT = 39;

    static const int KEY_CR = 13;
    static const int KEY_DEL = '.';
    static const int KEY_BS = 8;
    static const int KEY_ESC = 27;
                                
         	/* constants used by shiftset */
    static const int CTRL_ELE = 1;
    static const int SHFT_ELE = 2;
    static const int ALT_ELE = 4;

    Bitset shiftset(TShiftState state);
    int    tochar(WORD key, TShiftState state);

};
