#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(GetMoveEffect(MOVE_YAMI_BLOW) == EFFECT_TERRAIN_BOOST);
    ASSUME(GetMoveTerrainType(MOVE_YAMI_TERRAIN) == B_TERRAIN_YAMI);
    ASSUME(GetMoveTerrainType(MOVE_WASTELAND_TERRAIN) == B_TERRAIN_WASTELAND);
}

// Doubled base power on top of Yami Terrain's own 30% Dark boost (2.6x).
// The modifiers apply to base power, so rounding later in the formula
// (+2 constant, STAB) can push the final damage ratio slightly off
SINGLE_BATTLE_TEST("Yami Blow doubles in power in Yami Terrain", s16 damage)
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
        TURN { MOVE(player, MOVE_YAMI_BLOW); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_YAMI_BLOW, player);
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_GE(results[1].damage * 100, results[0].damage * 250);
        EXPECT_LE(results[1].damage * 100, results[0].damage * 262);
    }
}

SINGLE_BATTLE_TEST("Yami Blow doesn't double in power in other terrains", s16 damage)
{
    bool32 terrain;
    PARAMETRIZE { terrain = FALSE; }
    PARAMETRIZE { terrain = TRUE; }
    GIVEN {
        PLAYER(SPECIES_KING_OF_YAMIMAKAI);
        OPPONENT(SPECIES_FROG_THE_JAM) { Ability(ABILITY_DAMP); MaxHP(999); HP(999); }
    } WHEN {
        if (terrain)
            TURN { MOVE(player, MOVE_WASTELAND_TERRAIN); }
        TURN { MOVE(player, MOVE_YAMI_BLOW); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_YAMI_BLOW, player);
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_EQ(results[0].damage, results[1].damage);
    }
}
