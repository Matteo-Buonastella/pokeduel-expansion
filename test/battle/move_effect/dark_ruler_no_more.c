#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(GetMoveEffect(MOVE_DARK_RULER_NO_MORE) == EFFECT_DARK_RULER_NO_MORE);
}

SINGLE_BATTLE_TEST("Dark Ruler No More suppresses the target's Ability and lowers its Speed by 1 stage")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_DARK_RULER_NO_MORE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_DARK_RULER_NO_MORE, player);
        MESSAGE("The opposing Wobbuffet's Ability was suppressed!");
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_STATS_CHANGE, opponent);
        MESSAGE("The opposing Wobbuffet's Speed fell!");
    } THEN {
        EXPECT(opponent->volatiles.gastroAcid);
        EXPECT_EQ(opponent->statStages[STAT_SPEED], DEFAULT_STAT_STAGE - 1);
    }
}

SINGLE_BATTLE_TEST("Dark Ruler No More lowers the Speed of Dark types by 2 stages")
{
    GIVEN {
        ASSUME(GetSpeciesType(SPECIES_UMBREON, 0) == TYPE_DARK);
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_UMBREON);
    } WHEN {
        TURN { MOVE(player, MOVE_DARK_RULER_NO_MORE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_DARK_RULER_NO_MORE, player);
        MESSAGE("The opposing Umbreon's Ability was suppressed!");
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_STATS_CHANGE, opponent);
        MESSAGE("The opposing Umbreon's Speed harshly fell!");
    } THEN {
        EXPECT(opponent->volatiles.gastroAcid);
        EXPECT_EQ(opponent->statStages[STAT_SPEED], DEFAULT_STAT_STAGE - 2);
    }
}

SINGLE_BATTLE_TEST("Dark Ruler No More suppresses the Ability before lowering Speed")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_METAGROSS) { Ability(ABILITY_CLEAR_BODY); }
    } WHEN {
        TURN { MOVE(player, MOVE_DARK_RULER_NO_MORE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_DARK_RULER_NO_MORE, player);
        MESSAGE("The opposing Metagross's Ability was suppressed!");
        NOT ABILITY_POPUP(opponent, ABILITY_CLEAR_BODY);
        MESSAGE("The opposing Metagross's Speed fell!");
    } THEN {
        EXPECT_EQ(opponent->statStages[STAT_SPEED], DEFAULT_STAT_STAGE - 1);
    }
}

SINGLE_BATTLE_TEST("Dark Ruler No More still lowers Speed if the target's Ability can't be suppressed")
{
    GIVEN {
        ASSUME(gAbilitiesInfo[ABILITY_MULTITYPE].cantBeSuppressed);
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_ARCEUS) { Ability(ABILITY_MULTITYPE); }
    } WHEN {
        TURN { MOVE(player, MOVE_DARK_RULER_NO_MORE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_DARK_RULER_NO_MORE, player);
        NOT MESSAGE("The opposing Arceus's Ability was suppressed!");
        MESSAGE("The opposing Arceus's Speed fell!");
    } THEN {
        EXPECT(!opponent->volatiles.gastroAcid);
        EXPECT_EQ(opponent->statStages[STAT_SPEED], DEFAULT_STAT_STAGE - 1);
    }
}

SINGLE_BATTLE_TEST("Dark Ruler No More still suppresses the Ability if Speed can't go any lower")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_SCARY_FACE); }
        TURN { MOVE(player, MOVE_SCARY_FACE); }
        TURN { MOVE(player, MOVE_SCARY_FACE); }
        TURN { MOVE(player, MOVE_DARK_RULER_NO_MORE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_DARK_RULER_NO_MORE, player);
        MESSAGE("The opposing Wobbuffet's Ability was suppressed!");
        NOT MESSAGE("The opposing Wobbuffet's Speed won't go any lower!");
    } THEN {
        EXPECT(opponent->volatiles.gastroAcid);
        EXPECT_EQ(opponent->statStages[STAT_SPEED], MIN_STAT_STAGE);
    }
}

SINGLE_BATTLE_TEST("Dark Ruler No More fails if the Ability can't be suppressed and Speed can't go any lower")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_SCARY_FACE); }
        TURN { MOVE(player, MOVE_SCARY_FACE); }
        TURN { MOVE(player, MOVE_SCARY_FACE); }
        TURN { MOVE(player, MOVE_GASTRO_ACID); }
        TURN { MOVE(player, MOVE_DARK_RULER_NO_MORE); }
    } SCENE {
        NOT ANIMATION(ANIM_TYPE_MOVE, MOVE_DARK_RULER_NO_MORE, player);
        MESSAGE("But it failed!");
    }
}
