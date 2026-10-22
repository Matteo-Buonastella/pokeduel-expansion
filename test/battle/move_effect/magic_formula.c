#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(GetMoveEffect(MOVE_MAGIC_FORMULA) == EFFECT_MAGIC_FORMULA);
}

SINGLE_BATTLE_TEST("Magic Formula raises the user's Sp. Atk by 1 stage and restores 1/4 of its max HP")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { HP(1); MaxHP(400); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_MAGIC_FORMULA); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_MAGIC_FORMULA, player);
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_STATS_CHANGE, player);
        MESSAGE("Wobbuffet's Sp. Atk rose!");
        HP_BAR(player, damage: -100);
        MESSAGE("Wobbuffet had its HP restored.");
    } THEN {
        EXPECT_EQ(player->statStages[STAT_SPATK], DEFAULT_STAT_STAGE + 1);
    }
}

SINGLE_BATTLE_TEST("Magic Formula still raises Sp. Atk at full HP")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_MAGIC_FORMULA); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_MAGIC_FORMULA, player);
        MESSAGE("Wobbuffet's Sp. Atk rose!");
        NONE_OF {
            HP_BAR(player);
            MESSAGE("Wobbuffet had its HP restored.");
        }
    } THEN {
        EXPECT_EQ(player->statStages[STAT_SPATK], DEFAULT_STAT_STAGE + 1);
    }
}

SINGLE_BATTLE_TEST("Magic Formula still restores HP if Sp. Atk can't go any higher")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { HP(100); MaxHP(400); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_NASTY_PLOT); }
        TURN { MOVE(player, MOVE_NASTY_PLOT); }
        TURN { MOVE(player, MOVE_NASTY_PLOT); }
        TURN { MOVE(player, MOVE_MAGIC_FORMULA); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_MAGIC_FORMULA, player);
        MESSAGE("Wobbuffet's Sp. Atk won't go any higher!");
        HP_BAR(player);
        MESSAGE("Wobbuffet had its HP restored.");
    } THEN {
        EXPECT_EQ(player->statStages[STAT_SPATK], MAX_STAT_STAGE);
    }
}

SINGLE_BATTLE_TEST("Magic Formula fails at full HP if Sp. Atk can't go any higher")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_NASTY_PLOT); }
        TURN { MOVE(player, MOVE_NASTY_PLOT); }
        TURN { MOVE(player, MOVE_NASTY_PLOT); }
        TURN { MOVE(player, MOVE_MAGIC_FORMULA); }
    } SCENE {
        MESSAGE("Wobbuffet used Magic Formula!");
        NOT ANIMATION(ANIM_TYPE_MOVE, MOVE_MAGIC_FORMULA, player);
        MESSAGE("But it failed!");
    }
}
