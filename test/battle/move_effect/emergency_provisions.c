#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(GetMoveEffect(MOVE_EMERGENCY_PROVISIONS) == EFFECT_EMERGENCY_PROVISIONS);
    ASSUME(GetMoveEffect(MOVE_SWORDS_DANCE) == EFFECT_STAT_CHANGE);
}

SINGLE_BATTLE_TEST("Emergency Provisions restores 25% of max HP per positive stat stage")
{
    s16 healed;

    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { HP(1); MaxHP(400); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_SWORDS_DANCE); }
        TURN { MOVE(player, MOVE_EMERGENCY_PROVISIONS); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_EMERGENCY_PROVISIONS, player);
        HP_BAR(player, captureDamage: &healed);
        MESSAGE("Wobbuffet had its HP restored.");
        MESSAGE("Wobbuffet's stat changes were removed!");
    } THEN {
        EXPECT_EQ(healed, -200); // +2 Attack = 50%
        EXPECT_EQ(player->statStages[STAT_ATK], DEFAULT_STAT_STAGE);
    }
}

SINGLE_BATTLE_TEST("Emergency Provisions counts accuracy and evasion boosts and ignores stat drops")
{
    s16 healed;

    GIVEN {
        ASSUME(GetMoveEffect(MOVE_DOUBLE_TEAM) == EFFECT_STAT_CHANGE);
        ASSUME(GetMoveEffect(MOVE_GROWL) == EFFECT_STAT_CHANGE);
        PLAYER(SPECIES_WOBBUFFET) { HP(1); MaxHP(400); Speed(1); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(2); }
    } WHEN {
        TURN { MOVE(player, MOVE_DOUBLE_TEAM); MOVE(opponent, MOVE_GROWL); }
        TURN { MOVE(player, MOVE_EMERGENCY_PROVISIONS); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_EMERGENCY_PROVISIONS, player);
        HP_BAR(player, captureDamage: &healed);
    } THEN {
        EXPECT_EQ(healed, -100); // +1 evasion = 25%, the -1 Attack doesn't count
        EXPECT_EQ(player->statStages[STAT_EVASION], DEFAULT_STAT_STAGE);
        EXPECT_EQ(player->statStages[STAT_ATK], DEFAULT_STAT_STAGE); // Drops are reset too
    }
}

SINGLE_BATTLE_TEST("Emergency Provisions heals at most 100% of max HP")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { HP(1); MaxHP(400); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_SWORDS_DANCE); }
        TURN { MOVE(player, MOVE_SWORDS_DANCE); }
        TURN { MOVE(player, MOVE_SWORDS_DANCE); } // +6 Attack = 150%
        TURN { MOVE(player, MOVE_EMERGENCY_PROVISIONS); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_EMERGENCY_PROVISIONS, player);
        HP_BAR(player);
    } THEN {
        EXPECT_EQ(player->hp, 400);
    }
}

SINGLE_BATTLE_TEST("Emergency Provisions fails if the user has no stat boosts")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { HP(1); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_EMERGENCY_PROVISIONS); }
    } SCENE {
        NOT ANIMATION(ANIM_TYPE_MOVE, MOVE_EMERGENCY_PROVISIONS, player);
        MESSAGE("But it failed!");
    } THEN {
        EXPECT_EQ(player->hp, 1);
    }
}

SINGLE_BATTLE_TEST("Emergency Provisions doesn't reset stats if the user is at full HP")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_SWORDS_DANCE); }
        TURN { MOVE(player, MOVE_EMERGENCY_PROVISIONS); }
    } SCENE {
        NOT ANIMATION(ANIM_TYPE_MOVE, MOVE_EMERGENCY_PROVISIONS, player);
        MESSAGE("Wobbuffet's HP is full!");
    } THEN {
        EXPECT_EQ(player->statStages[STAT_ATK], DEFAULT_STAT_STAGE + 2);
    }
}

AI_SINGLE_BATTLE_TEST("AI doesn't throw away important boosts with Emergency Provisions if it doesn't need the HP")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_TRY_TO_FAINT);
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(250); MaxHP(400); Moves(MOVE_EMERGENCY_PROVISIONS, MOVE_SWORDS_DANCE, MOVE_SCRATCH); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_SWORDS_DANCE); }
        TURN { MOVE(player, MOVE_CELEBRATE); NOT_EXPECT_MOVE(opponent, MOVE_EMERGENCY_PROVISIONS); }
    }
}

AI_SINGLE_BATTLE_TEST("AI uses Emergency Provisions to trade its boosts for HP when it's low")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_TRY_TO_FAINT);
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_SCRATCH); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(100); MaxHP(400); Moves(MOVE_EMERGENCY_PROVISIONS, MOVE_SWORDS_DANCE, MOVE_SPLASH); }
    } WHEN {
        TURN { MOVE(player, MOVE_SCRATCH); MOVE(opponent, MOVE_SWORDS_DANCE); }
        TURN { MOVE(player, MOVE_SCRATCH); EXPECT_MOVE(opponent, MOVE_EMERGENCY_PROVISIONS); }
    }
}

AI_SINGLE_BATTLE_TEST("AI doesn't use Emergency Provisions without stat boosts")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_TRY_TO_FAINT);
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(50); MaxHP(400); Moves(MOVE_EMERGENCY_PROVISIONS, MOVE_SCRATCH); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); EXPECT_MOVE(opponent, MOVE_SCRATCH); }
    }
}
