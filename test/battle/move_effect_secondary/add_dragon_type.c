#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(MoveHasAdditionalEffectSelf(MOVE_DRAGONIC_ATTACK, MOVE_EFFECT_ADD_DRAGON_TYPE));
    ASSUME(GetSpeciesType(SPECIES_WOBBUFFET, 0) == TYPE_PSYCHIC);
    ASSUME(GetSpeciesType(SPECIES_DRATINI, 0) == TYPE_DRAGON);
}

SINGLE_BATTLE_TEST("Dragonic Attack raises the user's Attack and Defense and adds the Dragon type to it")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_DRAGONIC_ATTACK); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_DRAGONIC_ATTACK, player);
        HP_BAR(opponent);
        MESSAGE("Wobbuffet's Attack rose!");
        MESSAGE("Wobbuffet's Defense rose!");
        MESSAGE("Dragon type was added to Wobbuffet!");
    } THEN {
        EXPECT_EQ(player->statStages[STAT_ATK], DEFAULT_STAT_STAGE + 1);
        EXPECT_EQ(player->statStages[STAT_DEF], DEFAULT_STAT_STAGE + 1);
        EXPECT_EQ(player->types[0], TYPE_PSYCHIC);
        EXPECT_EQ(player->types[2], TYPE_DRAGON);
    }
}

SINGLE_BATTLE_TEST("Dragonic Attack doesn't add the Dragon type if the user is already a Dragon type")
{
    GIVEN {
        PLAYER(SPECIES_DRATINI);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_DRAGONIC_ATTACK); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_DRAGONIC_ATTACK, player);
        NOT MESSAGE("Dragon type was added to Dratini!");
    } THEN {
        EXPECT_EQ(player->types[2], TYPE_MYSTERY);
    }
}

SINGLE_BATTLE_TEST("Dragonic Attack's Dragon type is removed when the user switches out")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        PLAYER(SPECIES_WYNAUT);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_DRAGONIC_ATTACK); }
        TURN { SWITCH(player, 1); }
        TURN { SWITCH(player, 0); }
    } THEN {
        EXPECT_EQ(player->species, SPECIES_WOBBUFFET);
        EXPECT_NE(player->types[2], TYPE_DRAGON);
    }
}
