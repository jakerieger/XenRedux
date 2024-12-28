// Author: Jake Rieger
// Created: 12/27/2024.
//

#pragma once

#include "Types.hpp"

namespace x::Input {
    namespace KeyCodes {
        static constexpr u16 Space        = 32;
        static constexpr u16 Apostrophe   = 39;
        static constexpr u16 Comma        = 44;
        static constexpr u16 Minus        = 45;
        static constexpr u16 Period       = 46;
        static constexpr u16 Slash        = 47;
        static constexpr u16 Zero         = 48;
        static constexpr u16 One          = 49;
        static constexpr u16 Two          = 50;
        static constexpr u16 Three        = 51;
        static constexpr u16 Four         = 52;
        static constexpr u16 Five         = 53;
        static constexpr u16 Six          = 54;
        static constexpr u16 Seven        = 55;
        static constexpr u16 Eight        = 56;
        static constexpr u16 Nine         = 57;
        static constexpr u16 Semicolon    = 59;
        static constexpr u16 Equal        = 61;
        static constexpr u16 A            = 65;
        static constexpr u16 B            = 66;
        static constexpr u16 C            = 67;
        static constexpr u16 D            = 68;
        static constexpr u16 E            = 69;
        static constexpr u16 F            = 70;
        static constexpr u16 G            = 71;
        static constexpr u16 H            = 72;
        static constexpr u16 I            = 73;
        static constexpr u16 J            = 74;
        static constexpr u16 K            = 75;
        static constexpr u16 L            = 76;
        static constexpr u16 M            = 77;
        static constexpr u16 N            = 78;
        static constexpr u16 O            = 79;
        static constexpr u16 P            = 80;
        static constexpr u16 Q            = 81;
        static constexpr u16 R            = 82;
        static constexpr u16 S            = 83;
        static constexpr u16 T            = 84;
        static constexpr u16 U            = 85;
        static constexpr u16 V            = 86;
        static constexpr u16 W            = 87;
        static constexpr u16 X            = 88;
        static constexpr u16 Y            = 89;
        static constexpr u16 Z            = 90;
        static constexpr u16 LeftBracket  = 91;
        static constexpr u16 Backslash    = 92;
        static constexpr u16 RightBracket = 93;
        static constexpr u16 GraveAccent  = 96;
        static constexpr u16 World1       = 161;
        static constexpr u16 World2       = 162;
        static constexpr u16 Escape       = 256;
        static constexpr u16 Enter        = 257;
        static constexpr u16 Tab          = 258;
        static constexpr u16 Backspace    = 259;
        static constexpr u16 Insert       = 260;
        static constexpr u16 Delete       = 261;
        static constexpr u16 Right        = 262;
        static constexpr u16 Left         = 263;
        static constexpr u16 Down         = 264;
        static constexpr u16 Up           = 265;
        static constexpr u16 PageUp       = 266;
        static constexpr u16 PageDown     = 267;
        static constexpr u16 Home         = 268;
        static constexpr u16 End          = 269;
        static constexpr u16 CapsLock     = 280;
        static constexpr u16 ScrollLock   = 281;
        static constexpr u16 NumLock      = 282;
        static constexpr u16 PrintScreen  = 283;
        static constexpr u16 Pause        = 284;
        static constexpr u16 F1           = 290;
        static constexpr u16 F2           = 291;
        static constexpr u16 F3           = 292;
        static constexpr u16 F4           = 293;
        static constexpr u16 F5           = 294;
        static constexpr u16 F6           = 295;
        static constexpr u16 F7           = 296;
        static constexpr u16 F8           = 297;
        static constexpr u16 F9           = 298;
        static constexpr u16 F10          = 299;
        static constexpr u16 F11          = 300;
        static constexpr u16 F12          = 301;
        static constexpr u16 F13          = 302;
        static constexpr u16 F14          = 303;
        static constexpr u16 F15          = 304;
        static constexpr u16 F16          = 305;
        static constexpr u16 F17          = 306;
        static constexpr u16 F18          = 307;
        static constexpr u16 F19          = 308;
        static constexpr u16 F20          = 309;
        static constexpr u16 F21          = 310;
        static constexpr u16 F22          = 311;
        static constexpr u16 F23          = 312;
        static constexpr u16 F24          = 313;
        static constexpr u16 F25          = 314;
        static constexpr u16 KP_0         = 320;
        static constexpr u16 KP_1         = 321;
        static constexpr u16 KP_2         = 322;
        static constexpr u16 KP_3         = 323;
        static constexpr u16 KP_4         = 324;
        static constexpr u16 KP_5         = 325;
        static constexpr u16 KP_6         = 326;
        static constexpr u16 KP_7         = 327;
        static constexpr u16 KP_8         = 328;
        static constexpr u16 KP_9         = 329;
        static constexpr u16 KP_DECIMAL   = 330;
        static constexpr u16 KP_DIVIDE    = 331;
        static constexpr u16 KP_MULTIPLY  = 332;
        static constexpr u16 KP_SUBTRACT  = 333;
        static constexpr u16 KP_ADD       = 334;
        static constexpr u16 KP_ENTER     = 335;
        static constexpr u16 KP_EQUAL     = 336;
        static constexpr u16 LeftShift    = 340;
        static constexpr u16 LeftControl  = 341;
        static constexpr u16 LeftAlt      = 342;
        static constexpr u16 LeftSuper    = 343;
        static constexpr u16 RightShift   = 344;
        static constexpr u16 RightControl = 345;
        static constexpr u16 RightAlt     = 346;
        static constexpr u16 RightSuper   = 347;
        static constexpr u16 RightMenu    = 348;
    }  // namespace KeyCodes

    namespace MouseCodes {
        static constexpr u16 Left   = 0;
        static constexpr u16 Right  = 1;
        static constexpr u16 Middle = 2;
        static constexpr u16 Macro1 = 3;
        static constexpr u16 Macro2 = 4;
        static constexpr u16 Macro3 = 5;
        static constexpr u16 Macro4 = 6;
        static constexpr u16 Macro5 = 7;
    }  // namespace MouseCodes

    /* namespace GamepadCodes {} */
}  // namespace x::Input