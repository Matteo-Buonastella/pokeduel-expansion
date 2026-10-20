#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(MoveHasAdditionalEffect(MOVE_ROCK_CRUSH_SWORD, MOVE_EFFECT_MOON_BLOCK) == TRUE);
    ASSUME(IsMoonMove(MOVE_MOONBLAST));
}

SINGLE_BATTLE_TEST("Rock Crush Sword prevents the usage of moon moves")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Speed(100); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(50); }
    } WHEN {
        TURN { MOVE(player, MOVE_ROCK_CRUSH_SWORD, WITH_RNG(RNG_SECONDARY_EFFECT, FALSE)); MOVE(opponent, MOVE_MOONBLAST); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_ROCK_CRUSH_SWORD, player);
        HP_BAR(opponent);
        MESSAGE("The effects of Rock Crush Sword prevent the opposing Wobbuffet from using certain moves!");
        NOT ANIMATION(ANIM_TYPE_MOVE, MOVE_MOONBLAST, opponent);
    }
}

SINGLE_BATTLE_TEST("Rock Crush Sword prevents moon moves for 2 turns")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_MOONBLAST); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_MOONBLAST); MOVE(player, MOVE_ROCK_CRUSH_SWORD, WITH_RNG(RNG_SECONDARY_EFFECT, FALSE)); }
        TURN { FORCED_MOVE(opponent); }
        TURN { MOVE(opponent, MOVE_MOONBLAST); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_MOONBLAST, opponent);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_ROCK_CRUSH_SWORD, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_STRUGGLE, opponent);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_MOONBLAST, opponent);
    }
}

SINGLE_BATTLE_TEST("Rock Crush Sword usage when target is already prevented from using moon moves doesn't reset timer")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_MOONBLAST); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_MOONBLAST); MOVE(player, MOVE_ROCK_CRUSH_SWORD, WITH_RNG(RNG_SECONDARY_EFFECT, FALSE)); }
        TURN { FORCED_MOVE(opponent); MOVE(player, MOVE_ROCK_CRUSH_SWORD, WITH_RNG(RNG_SECONDARY_EFFECT, FALSE)); }
        TURN { MOVE(opponent, MOVE_MOONBLAST); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_MOONBLAST, opponent);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_ROCK_CRUSH_SWORD, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_STRUGGLE, opponent);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_ROCK_CRUSH_SWORD, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_MOONBLAST, opponent);
    }
}

SINGLE_BATTLE_TEST("Rock Crush Sword doesn't prevent moves that aren't moon moves")
{
    GIVEN {
        ASSUME(!IsMoonMove(MOVE_SCRATCH));
        PLAYER(SPECIES_WOBBUFFET) { Speed(100); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(50); }
    } WHEN {
        TURN { MOVE(player, MOVE_ROCK_CRUSH_SWORD, WITH_RNG(RNG_SECONDARY_EFFECT, FALSE)); MOVE(opponent, MOVE_SCRATCH); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_ROCK_CRUSH_SWORD, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SCRATCH, opponent);
    }
}

SINGLE_BATTLE_TEST("Rock Crush Sword doesn't prevent moon moves if the user has Sheer Force")
{
    GIVEN {
        PLAYER(SPECIES_TAUROS) { Ability(ABILITY_SHEER_FORCE); Speed(100); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(50); }
    } WHEN {
        TURN { MOVE(player, MOVE_ROCK_CRUSH_SWORD); MOVE(opponent, MOVE_MOONBLAST); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_ROCK_CRUSH_SWORD, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_MOONBLAST, opponent);
    }
}
