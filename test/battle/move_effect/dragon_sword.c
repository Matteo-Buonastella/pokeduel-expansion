#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Dragon Sword raises user's Attack by 1 stage if it KO's a DRAGON")
{
    GIVEN {
        PLAYER(SPECIES_DARK_PALADIN) { Moves(MOVE_DRAGON_SWORD); }
        OPPONENT(SPECIES_RAYQUAZA) { HP(1); }
        OPPONENT(SPECIES_RAYQUAZA);
    } WHEN {
        TURN { MOVE(player, MOVE_DRAGON_SWORD); SEND_OUT(opponent, 1); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_DRAGON_SWORD, player);
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_STATS_CHANGE, player);
    } THEN {
        EXPECT_EQ(player->statStages[STAT_ATK], DEFAULT_STAT_STAGE + 1);
    }
}

SINGLE_BATTLE_TEST("Dragon Sword does not raise Attack if it KO's non Dragon")
{
    GIVEN {
        PLAYER(SPECIES_DARK_PALADIN) { Moves(MOVE_DRAGON_SWORD); }
        OPPONENT(SPECIES_KURIBOH) { HP(1); }
        OPPONENT(SPECIES_KURIBOH);
    } WHEN {
        TURN { MOVE(player, MOVE_DRAGON_SWORD); SEND_OUT(opponent, 1); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_DRAGON_SWORD, player);
        NOT ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_STATS_CHANGE, player);
    } THEN {
        EXPECT_EQ(player->statStages[STAT_ATK], DEFAULT_STAT_STAGE);
    }
}

SINGLE_BATTLE_TEST("Dragon Sword doesn't raise user's Attack if it doesn't faint target")
{
    GIVEN {
        PLAYER(SPECIES_DARK_PALADIN) { Moves(MOVE_DRAGON_SWORD); }
        OPPONENT(SPECIES_YUBEL_ULTIMATE_KNIGHTMARE);
    } WHEN {
        TURN { MOVE(player, MOVE_DRAGON_SWORD); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_DRAGON_SWORD, player);
        NOT ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_STATS_CHANGE, player);
    } THEN {
        EXPECT_EQ(player->statStages[STAT_ATK], DEFAULT_STAT_STAGE);
    }
}
