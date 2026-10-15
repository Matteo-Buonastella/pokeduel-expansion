#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(GetMoveTerrainType(MOVE_MOUNTAIN_TERRAIN) == B_TERRAIN_MOUNTAIN);
    ASSUME(GetMoveType(MOVE_DRAGON_CLAW) == TYPE_DRAGON);
    ASSUME(GetMoveType(MOVE_GUST) == TYPE_FLYING);
    ASSUME(GetMoveType(MOVE_THUNDER) == TYPE_ELECTRIC);
    ASSUME(GetMoveAccuracy(MOVE_THUNDER) == 70);
}

// The boost applies to base power, so rounding later in the formula
// (+2 constant, STAB) can push the final damage ratio slightly off 1.3x
SINGLE_BATTLE_TEST("Mountain Terrain increases power of Dragon- and Flying-type moves by 30 percent", s16 damage)
{
    bool32 terrain;
    enum Move move;
    PARAMETRIZE { terrain = FALSE; move = MOVE_DRAGON_CLAW; }
    PARAMETRIZE { terrain = TRUE;  move = MOVE_DRAGON_CLAW; }
    PARAMETRIZE { terrain = FALSE; move = MOVE_GUST; }
    PARAMETRIZE { terrain = TRUE;  move = MOVE_GUST; }
    GIVEN {
        PLAYER(SPECIES_DRAGON_MASTER_KNIGHT);
        OPPONENT(SPECIES_SANGAN) { MaxHP(999); HP(999); } // Enough HP to survive so the full damage is captured
    } WHEN {
        if (terrain)
            TURN { MOVE(player, MOVE_MOUNTAIN_TERRAIN); }
        TURN { MOVE(player, move); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, move, player);
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_GE(results[1].damage * 100, results[0].damage * 125);
        EXPECT_LE(results[1].damage * 100, results[0].damage * 131);
        EXPECT_GE(results[3].damage * 100, results[2].damage * 125);
        EXPECT_LE(results[3].damage * 100, results[2].damage * 131);
    }
}

SINGLE_BATTLE_TEST("Mountain Terrain makes Electric-type moves bypass accuracy checks")
{
    PASSES_RANDOMLY(100, 100, RNG_ACCURACY);
    GIVEN {
        PLAYER(SPECIES_SUMMONED_SKULL);
        OPPONENT(SPECIES_DRAGON_MASTER_KNIGHT);
    } WHEN {
        TURN { MOVE(opponent, MOVE_MOUNTAIN_TERRAIN); }
        TURN { MOVE(player, MOVE_THUNDER); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_MOUNTAIN_TERRAIN, opponent);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_THUNDER, player);
        NONE_OF { MESSAGE("The opposing D Master K avoided the attack!"); }
    }
}

SINGLE_BATTLE_TEST("Electric-type moves can still miss without Mountain Terrain")
{
    PASSES_RANDOMLY(70, 100, RNG_ACCURACY);
    GIVEN {
        PLAYER(SPECIES_SUMMONED_SKULL);
        OPPONENT(SPECIES_DRAGON_MASTER_KNIGHT);
    } WHEN {
        TURN { MOVE(player, MOVE_THUNDER); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_THUNDER, player);
    }
}

SINGLE_BATTLE_TEST("Mountain Terrain lasts for 5 turns")
{
    GIVEN {
        PLAYER(SPECIES_DRAGON_MASTER_KNIGHT);
        OPPONENT(SPECIES_SANGAN);
    } WHEN {
        TURN { MOVE(opponent, MOVE_CELEBRATE); MOVE(player, MOVE_MOUNTAIN_TERRAIN); }
        TURN {}
        TURN {}
        TURN {}
        TURN {}
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_MOUNTAIN_TERRAIN, player);
        MESSAGE("Mountains rose up around the battlefield!");
        MESSAGE("The mountains faded from the battlefield.");
    }
}
