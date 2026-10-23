#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(GetMoveEffect(MOVE_STIM_PACK) == EFFECT_STIM_PACK);
}

SINGLE_BATTLE_TEST("Stim-Pack raises the user's Attack by 3 stages, then lowers it by 1 at the end of the turn")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_STIM_PACK); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_STIM_PACK, player);
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_STATS_CHANGE, player);
        MESSAGE("Wobbuffet's Attack drastically rose!");
        MESSAGE("Wobbuffet's Stim-Pack is wearing off!");
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_STATS_CHANGE, player);
        MESSAGE("Wobbuffet's Attack fell!");
    } THEN {
        EXPECT_EQ(player->statStages[STAT_ATK], DEFAULT_STAT_STAGE + 2);
    }
}

SINGLE_BATTLE_TEST("Stim-Pack keeps lowering the user's Attack every turn, even past its original stage")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_STIM_PACK); }
        TURN {}
        TURN {}
        TURN {}
    } THEN {
        EXPECT_EQ(player->statStages[STAT_ATK], DEFAULT_STAT_STAGE - 1);
    }
}

SINGLE_BATTLE_TEST("Stim-Pack's Attack drops aren't prevented by Clear Body")
{
    GIVEN {
        PLAYER(SPECIES_BELDUM) { Ability(ABILITY_CLEAR_BODY); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_STIM_PACK); }
    } SCENE {
        NOT ABILITY_POPUP(player, ABILITY_CLEAR_BODY);
    } THEN {
        EXPECT_EQ(player->statStages[STAT_ATK], DEFAULT_STAT_STAGE + 2);
    }
}

SINGLE_BATTLE_TEST("Stim-Pack's Attack drops stop when the user switches out")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        PLAYER(SPECIES_WYNAUT);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_STIM_PACK); }
        TURN { SWITCH(player, 1); }
    } SCENE {
        MESSAGE("Wobbuffet's Stim-Pack is wearing off!");
        NOT MESSAGE("Wynaut's Stim-Pack is wearing off!");
    } THEN {
        EXPECT_EQ(player->statStages[STAT_ATK], DEFAULT_STAT_STAGE);
    }
}

SINGLE_BATTLE_TEST("Stim-Pack fails if the user's Attack is already maxed")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_BELLY_DRUM); }
        TURN { MOVE(player, MOVE_STIM_PACK); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_BELLY_DRUM, player);
        NOT ANIMATION(ANIM_TYPE_MOVE, MOVE_STIM_PACK, player);
        MESSAGE("But it failed!");
        NOT MESSAGE("Wobbuffet's Stim-Pack is wearing off!");
    }
}

AI_SINGLE_BATTLE_TEST("AI doesn't use Stim-Pack without physical moves")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_TRY_TO_FAINT);
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_STIM_PACK, MOVE_EMBER); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); EXPECT_MOVE(opponent, MOVE_EMBER); }
    }
}
