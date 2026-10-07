#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(GetMoveEffect(MOVE_TRIPLE_SLICE) == EFFECT_TRIPLE_KICK);
    ASSUME(GetMoveStrikeCount(MOVE_TRIPLE_SLICE) == 3);
}

SINGLE_BATTLE_TEST("Triple Slice hits 3 times with each hit stronger than the last")
{
    s16 firstHit;
    s16 secondHit;
    s16 thirdHit;

    GIVEN {
        PLAYER(SPECIES_SANGAN);
        OPPONENT(SPECIES_SANGAN);
    } WHEN {
        TURN { MOVE(player, MOVE_TRIPLE_SLICE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TRIPLE_SLICE, player);
        HP_BAR(opponent, captureDamage: &firstHit);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TRIPLE_SLICE, player);
        HP_BAR(opponent, captureDamage: &secondHit);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TRIPLE_SLICE, player);
        HP_BAR(opponent, captureDamage: &thirdHit);
        MESSAGE("The Pokémon was hit 3 times!");
    } THEN {
        EXPECT_GT(secondHit, firstHit);
        EXPECT_GT(thirdHit, secondHit);
        // Power goes 20 -> 40 -> 60, so each hit adds the same amount of damage.
        // Damage isn't exactly 2x/3x because the damage formula adds a flat +2.
        // Allow 1 point of rounding difference.
        EXPECT_LE(abs((secondHit - firstHit) - (thirdHit - secondHit)), 1);
    }
}
