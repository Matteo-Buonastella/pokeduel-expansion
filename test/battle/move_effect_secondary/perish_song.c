#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(MoveHasAdditionalEffectWithChance(MOVE_DEVILS_CLAW, MOVE_EFFECT_PERISH_SONG, 100) == TRUE);
}

SINGLE_BATTLE_TEST("Devil's Claw gives the target a perish count")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_DEVILS_CLAW); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_DEVILS_CLAW, player);
        HP_BAR(opponent);
        MESSAGE("The opposing Wobbuffet will faint in three turns!");
        MESSAGE("The opposing Wobbuffet's perish count fell to 3!");
    } THEN {
        EXPECT(opponent->volatiles.perishSong);
        EXPECT(!player->volatiles.perishSong);
    }
}

SINGLE_BATTLE_TEST("Devil's Claw's perish count makes the target faint after 3 turns")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_DEVILS_CLAW); }
        TURN { }
        TURN { }
        TURN { }
    } SCENE {
        MESSAGE("The opposing Wobbuffet's perish count fell to 3!");
        MESSAGE("The opposing Wobbuffet's perish count fell to 2!");
        MESSAGE("The opposing Wobbuffet's perish count fell to 1!");
        MESSAGE("The opposing Wobbuffet fainted!");
    }
}

SINGLE_BATTLE_TEST("Devil's Claw doesn't reset an existing perish count")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_DEVILS_CLAW); }
        TURN { MOVE(player, MOVE_DEVILS_CLAW); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_DEVILS_CLAW, player);
        MESSAGE("The opposing Wobbuffet will faint in three turns!");
        MESSAGE("The opposing Wobbuffet's perish count fell to 3!");
        ANIMATION(ANIM_TYPE_MOVE, MOVE_DEVILS_CLAW, player);
        NOT MESSAGE("The opposing Wobbuffet will faint in three turns!");
        MESSAGE("The opposing Wobbuffet's perish count fell to 2!");
    }
}

SINGLE_BATTLE_TEST("Devil's Claw doesn't give a perish count if it hits a Substitute")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Speed(50); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_SUBSTITUTE); MOVE(player, MOVE_DEVILS_CLAW); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_DEVILS_CLAW, player);
        NOT MESSAGE("The opposing Wobbuffet will faint in three turns!");
    } THEN {
        EXPECT(!opponent->volatiles.perishSong);
    }
}

SINGLE_BATTLE_TEST("Devil's Claw's perish count isn't blocked by Soundproof")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_VOLTORB) { Ability(ABILITY_SOUNDPROOF); }
    } WHEN {
        TURN { MOVE(player, MOVE_DEVILS_CLAW); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_DEVILS_CLAW, player);
        MESSAGE("The opposing Voltorb will faint in three turns!");
    } THEN {
        EXPECT(opponent->volatiles.perishSong);
    }
}

SINGLE_BATTLE_TEST("Devil's Claw doesn't give a perish count if the user has Sheer Force")
{
    GIVEN {
        PLAYER(SPECIES_TAUROS) { Ability(ABILITY_SHEER_FORCE); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_DEVILS_CLAW); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_DEVILS_CLAW, player);
        NOT MESSAGE("The opposing Wobbuffet will faint in three turns!");
    } THEN {
        EXPECT(!opponent->volatiles.perishSong);
    }
}

SINGLE_BATTLE_TEST("Devil's Claw doesn't give a perish count if the target has Shield Dust")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_DUSTOX) { Ability(ABILITY_SHIELD_DUST); }
    } WHEN {
        TURN { MOVE(player, MOVE_DEVILS_CLAW); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_DEVILS_CLAW, player);
        NOT MESSAGE("The opposing Dustox will faint in three turns!");
    } THEN {
        EXPECT(!opponent->volatiles.perishSong);
    }
}
