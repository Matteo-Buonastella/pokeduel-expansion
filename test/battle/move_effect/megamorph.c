#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(GetMoveEffect(MOVE_MEGAMORPH) == EFFECT_MEGAMORPH);
}

SINGLE_BATTLE_TEST("Megamorph sharply raises Attack/Sp.Atk if HP less than opponent")
{
    GIVEN {
        PLAYER(SPECIES_ROCKET_WARRIOR) HP(50); 
        OPPONENT(SPECIES_NEOS) HP(150);
    } WHEN {
        TURN { MOVE(player, MOVE_MEGAMORPH);}
    } SCENE {
        MESSAGE("RK Warrior used Megamorph!");
        ANIMATION(ANIM_TYPE_MOVE, MOVE_MEGAMORPH, player);
        MESSAGE("RK Warrior's Attack rose sharply!");
        MESSAGE("RK Warrior's Sp. Atk rose sharply!");
    }
}

SINGLE_BATTLE_TEST("Megamorph sharply lowers Attack/Sp.Atk if HP more than opponent")
{
    GIVEN {
        PLAYER(SPECIES_ROCKET_WARRIOR) HP(50); 
        OPPONENT(SPECIES_NEOS) HP(1);
    } WHEN {
        TURN { MOVE(player, MOVE_MEGAMORPH);}
    } SCENE {
        MESSAGE("RK Warrior used Megamorph!");
        ANIMATION(ANIM_TYPE_MOVE, MOVE_MEGAMORPH, player);
        MESSAGE("RK Warrior's Attack harshly fell!");
        MESSAGE("RK Warrior's Sp. Atk harshly fell!");
    }
}

DOUBLE_BATTLE_TEST("Megamorph sharply lowers Attack/Sp.Atk if HP more than both opponents in double battle")
{
    GIVEN {
        PLAYER(SPECIES_ROCKET_WARRIOR) HP(50); 
        PLAYER(SPECIES_ROCKET_WARRIOR) HP(50); 
        OPPONENT(SPECIES_NEOS) HP(1);
        OPPONENT(SPECIES_NEOS) HP(1);
    } WHEN {
        TURN { MOVE(playerLeft, MOVE_MEGAMORPH);}
    } SCENE {
        MESSAGE("RK Warrior used Megamorph!");
        ANIMATION(ANIM_TYPE_MOVE, MOVE_MEGAMORPH, playerLeft);
        MESSAGE("RK Warrior's Attack harshly fell!");
        MESSAGE("RK Warrior's Sp. Atk harshly fell!");
    }
}

DOUBLE_BATTLE_TEST("Megamorph sharply raises Attack/Sp.Atk if HP more than at least one opponent in double battle")
{
    GIVEN {
        PLAYER(SPECIES_ROCKET_WARRIOR) HP(50); 
        PLAYER(SPECIES_ROCKET_WARRIOR) HP(50); 
        OPPONENT(SPECIES_NEOS) HP(1);
        OPPONENT(SPECIES_NEOS) HP(100);
    } WHEN {
        TURN { MOVE(playerLeft, MOVE_MEGAMORPH);}
    } SCENE {
        MESSAGE("RK Warrior used Megamorph!");
        ANIMATION(ANIM_TYPE_MOVE, MOVE_MEGAMORPH, playerLeft);
        MESSAGE("RK Warrior's Attack rose sharply!");
        MESSAGE("RK Warrior's Sp. Atk rose sharply!");
    }
}
