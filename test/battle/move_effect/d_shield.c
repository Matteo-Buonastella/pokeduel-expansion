#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(GetMoveEffect(MOVE_D_SHIELD) == EFFECT_PROTECT);
    ASSUME(GetMoveProtectMethod(MOVE_D_SHIELD) == PROTECT_D_SHIELD);
    ASSUME(MoveMakesContact(MOVE_SCRATCH));
    ASSUME(!MoveMakesContact(MOVE_WATER_GUN));
}

SINGLE_BATTLE_TEST("D - Shield protects the user and raises its Defense if the move makes contact")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_D_SHIELD); MOVE(opponent, MOVE_SCRATCH); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_D_SHIELD, player);
        MESSAGE("Wobbuffet protected itself!");
        NOT HP_BAR(player);
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_STATS_CHANGE, player);
        MESSAGE("Wobbuffet's Defense rose!");
    } THEN {
        EXPECT_EQ(player->statStages[STAT_DEF], DEFAULT_STAT_STAGE + 1);
    }
}

SINGLE_BATTLE_TEST("D - Shield doesn't raise Defense if the move doesn't make contact")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_D_SHIELD); MOVE(opponent, MOVE_WATER_GUN); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_D_SHIELD, player);
        MESSAGE("Wobbuffet protected itself!");
        NONE_OF {
            HP_BAR(player);
            MESSAGE("Wobbuffet's Defense rose!");
        }
    } THEN {
        EXPECT_EQ(player->statStages[STAT_DEF], DEFAULT_STAT_STAGE);
    }
}

SINGLE_BATTLE_TEST("D - Shield protects from status moves")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_D_SHIELD); MOVE(opponent, MOVE_GROWL); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_D_SHIELD, player);
        MESSAGE("Wobbuffet protected itself!");
        NOT ANIMATION(ANIM_TYPE_MOVE, MOVE_GROWL, opponent);
    } THEN {
        EXPECT_EQ(player->statStages[STAT_ATK], DEFAULT_STAT_STAGE);
    }
}

SINGLE_BATTLE_TEST("D - Shield doesn't raise Defense if the attacker holds Protective Pads")
{
    GIVEN {
        ASSUME(gItemsInfo[ITEM_PROTECTIVE_PADS].holdEffect == HOLD_EFFECT_PROTECTIVE_PADS);
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET) { Item(ITEM_PROTECTIVE_PADS); }
    } WHEN {
        TURN { MOVE(player, MOVE_D_SHIELD); MOVE(opponent, MOVE_SCRATCH); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_D_SHIELD, player);
        MESSAGE("Wobbuffet protected itself!");
        NOT MESSAGE("Wobbuffet's Defense rose!");
    } THEN {
        EXPECT_EQ(player->statStages[STAT_DEF], DEFAULT_STAT_STAGE);
    }
}
