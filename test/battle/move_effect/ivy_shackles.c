#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(GetMoveEffect(MOVE_IVY_SHACKLES) == EFFECT_IVY_SHACKLES);
    ASSUME(GetMoveTerrainType(MOVE_WASTELAND_TERRAIN) == B_TERRAIN_WASTELAND);
}

SINGLE_BATTLE_TEST("Ivy Shackles ingrains the target and harshly lowers its Speed")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_IVY_SHACKLES); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_IVY_SHACKLES, player);
        MESSAGE("The opposing Wobbuffet was ingrained by the ivy shackles!");
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_STATS_CHANGE, opponent);
        MESSAGE("The opposing Wobbuffet's Speed harshly fell!");
    } THEN {
        EXPECT(opponent->volatiles.root);
        EXPECT_EQ(opponent->statStages[STAT_SPEED], DEFAULT_STAT_STAGE - 2);
    }
}

SINGLE_BATTLE_TEST("Ivy Shackles still lowers Speed if the target is already ingrained")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(opponent, MOVE_INGRAIN); }
        TURN { MOVE(player, MOVE_IVY_SHACKLES); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_IVY_SHACKLES, player);
        NOT MESSAGE("The opposing Wobbuffet was ingrained by the ivy shackles!");
        MESSAGE("The opposing Wobbuffet's Speed harshly fell!");
    } THEN {
        EXPECT_EQ(opponent->statStages[STAT_SPEED], DEFAULT_STAT_STAGE - 2);
    }
}

SINGLE_BATTLE_TEST("Ivy Shackles still ingrains the target if its Speed can't go any lower")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_IVY_SHACKLES); }
        TURN { MOVE(player, MOVE_IVY_SHACKLES); }
        TURN { MOVE(player, MOVE_IVY_SHACKLES); }
    } SCENE {
        MESSAGE("The opposing Wobbuffet was ingrained by the ivy shackles!");
    } THEN {
        EXPECT_EQ(opponent->statStages[STAT_SPEED], MIN_STAT_STAGE);
    }
}

SINGLE_BATTLE_TEST("Ivy Shackles fails if the target is already ingrained and its Speed can't go any lower")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_IVY_SHACKLES); }
        TURN { MOVE(player, MOVE_IVY_SHACKLES); }
        TURN { MOVE(player, MOVE_IVY_SHACKLES); }
        TURN { MOVE(player, MOVE_IVY_SHACKLES); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_IVY_SHACKLES, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_IVY_SHACKLES, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_IVY_SHACKLES, player);
        MESSAGE("But it failed!");
    }
}

SINGLE_BATTLE_TEST("Ivy Shackles fails in Wasteland Terrain")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(opponent, MOVE_WASTELAND_TERRAIN); }
        TURN { MOVE(player, MOVE_IVY_SHACKLES); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_WASTELAND_TERRAIN, opponent);
        MESSAGE("Wobbuffet can't use Ivy Shackles in the wasteland!");
        NOT ANIMATION(ANIM_TYPE_MOVE, MOVE_IVY_SHACKLES, player);
    } THEN {
        EXPECT(!opponent->volatiles.root);
        EXPECT_EQ(opponent->statStages[STAT_SPEED], DEFAULT_STAT_STAGE);
    }
}

AI_SINGLE_BATTLE_TEST("AI doesn't use Ivy Shackles in Wasteland Terrain")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_TRY_TO_FAINT);
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_WASTELAND_TERRAIN, MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_IVY_SHACKLES, MOVE_SCRATCH); }
    } WHEN {
        TURN { MOVE(player, MOVE_WASTELAND_TERRAIN); }
        TURN { MOVE(player, MOVE_CELEBRATE); EXPECT_MOVE(opponent, MOVE_SCRATCH); }
    }
}
