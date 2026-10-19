#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(GetMoveEffect(MOVE_GOAT_HEAL) == EFFECT_GOAT_HEAL);
    ASSUME(GetMoveEffect(MOVE_SUBSTITUTE) == EFFECT_SUBSTITUTE);
}

SINGLE_BATTLE_TEST("Goat Heal restores 1/2 of the user's max HP")
{
    s16 healed;

    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { HP(1); MaxHP(400); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_GOAT_HEAL); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_GOAT_HEAL, player);
        HP_BAR(player, captureDamage: &healed);
    } THEN {
        EXPECT_EQ(healed, -200);
    }
}

SINGLE_BATTLE_TEST("Goat Heal restores 3/4 of the user's max HP when behind a Substitute")
{
    s16 healed;

    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { HP(150); MaxHP(400); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_SUBSTITUTE); }
        TURN { MOVE(player, MOVE_GOAT_HEAL); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SUBSTITUTE, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_GOAT_HEAL, player);
        HP_BAR(player, captureDamage: &healed);
    } THEN {
        // Substitute costs 100 HP, leaving 50 HP, so the full 300 HP heal fits
        EXPECT_EQ(healed, -300);
        EXPECT(player->volatiles.substitute);
    }
}

SINGLE_BATTLE_TEST("Goat Heal fails if the user is at full HP")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_GOAT_HEAL); }
    } SCENE {
        NOT ANIMATION(ANIM_TYPE_MOVE, MOVE_GOAT_HEAL, player);
        MESSAGE("Wobbuffet's HP is full!");
    }
}

SINGLE_BATTLE_TEST("Goat Heal is prevented by Heal Block")
{
    GIVEN {
        ASSUME(GetMoveEffect(MOVE_HEAL_BLOCK) == EFFECT_HEAL_BLOCK);
        PLAYER(SPECIES_WOBBUFFET) { HP(1); Speed(1); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(2); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_HEAL_BLOCK); MOVE(player, MOVE_GOAT_HEAL); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_HEAL_BLOCK, opponent);
        NOT ANIMATION(ANIM_TYPE_MOVE, MOVE_GOAT_HEAL, player);
    } THEN {
        EXPECT_EQ(player->hp, 1);
    }
}

AI_SINGLE_BATTLE_TEST("AI uses Goat Heal when at low HP")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_TRY_TO_FAINT);
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(50); MaxHP(400); Moves(MOVE_GOAT_HEAL, MOVE_SPLASH); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); EXPECT_MOVE(opponent, MOVE_GOAT_HEAL); }
    }
}
