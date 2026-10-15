#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(GetMoveTerrainType(MOVE_YAMI_TERRAIN) == B_TERRAIN_YAMI);
    ASSUME(GetMoveType(MOVE_CRUNCH) == TYPE_DARK);
    ASSUME(GetMoveType(MOVE_DAZZLING_GLEAM) == TYPE_FAIRY);
}

// The modifiers apply to base power, so rounding later in the formula
// (+2 constant, STAB) can push the final damage ratio slightly off
SINGLE_BATTLE_TEST("Yami Terrain increases power of Dark-type moves by 30 percent", s16 damage)
{
    bool32 terrain;
    PARAMETRIZE { terrain = FALSE; }
    PARAMETRIZE { terrain = TRUE; }
    GIVEN {
        PLAYER(SPECIES_KING_OF_YAMIMAKAI);
        OPPONENT(SPECIES_FROG_THE_JAM) { Ability(ABILITY_DAMP); MaxHP(999); HP(999); } // Water takes neutral damage from Dark
    } WHEN {
        if (terrain)
            TURN { MOVE(player, MOVE_YAMI_TERRAIN); }
        TURN { MOVE(player, MOVE_CRUNCH); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CRUNCH, player);
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_GE(results[1].damage * 100, results[0].damage * 125);
        EXPECT_LE(results[1].damage * 100, results[0].damage * 131);
    }
}

SINGLE_BATTLE_TEST("Yami Terrain halves the power of Fairy-type moves", s16 damage)
{
    bool32 terrain;
    PARAMETRIZE { terrain = FALSE; }
    PARAMETRIZE { terrain = TRUE; }
    GIVEN {
        PLAYER(SPECIES_SPORE);
        OPPONENT(SPECIES_DARK_FLARE_KNIGHT) { MaxHP(999); HP(999); } // Fire/Dark takes neutral damage from Fairy
    } WHEN {
        if (terrain)
            TURN { MOVE(opponent, MOVE_YAMI_TERRAIN); }
        TURN { MOVE(player, MOVE_DAZZLING_GLEAM); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_DAZZLING_GLEAM, player);
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_GE(results[1].damage * 100, results[0].damage * 48);
        EXPECT_LE(results[1].damage * 100, results[0].damage * 52);
    }
}

SINGLE_BATTLE_TEST("Yami Terrain lasts for 5 turns")
{
    GIVEN {
        PLAYER(SPECIES_DARK_SAGE);
        OPPONENT(SPECIES_SANGAN);
    } WHEN {
        TURN { MOVE(opponent, MOVE_CELEBRATE); MOVE(player, MOVE_YAMI_TERRAIN); }
        TURN {}
        TURN {}
        TURN {}
        TURN {}
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_YAMI_TERRAIN, player);
        MESSAGE("Darkness shrouded the battlefield!");
        MESSAGE("The darkness lifted from the battlefield.");
    }
}
