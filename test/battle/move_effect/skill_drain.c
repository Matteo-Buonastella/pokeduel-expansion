#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(GetMoveEffect(MOVE_SKILL_DRAIN) == EFFECT_SKILL_DRAIN);
    ASSUME(MoveHasAdditionalEffect(MOVE_SKILL_DRAIN, MOVE_EFFECT_ABSORB));
    ASSUME(MoveHasAdditionalEffect(MOVE_SKILL_DRAIN, MOVE_EFFECT_SUPPRESS_ABILITY));
}

SINGLE_BATTLE_TEST("Skill Drain recovers 50% of the damage dealt")
{
    s16 damage;
    s16 healed;
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { HP(1); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_SKILL_DRAIN); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SKILL_DRAIN, player);
        HP_BAR(opponent, captureDamage: &damage);
        HP_BAR(player, captureDamage: &healed);
    } THEN {
        EXPECT_MUL_EQ(damage, Q_4_12(-0.5), healed);
    }
}

SINGLE_BATTLE_TEST("Skill Drain doubles in power if the target has an active ability", s16 damage)
{
    enum Move move;

    PARAMETRIZE { move = MOVE_GASTRO_ACID; }
    PARAMETRIZE { move = MOVE_CELEBRATE; }

    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET) { MaxHP(999); HP(999); }
    } WHEN {
        TURN { MOVE(player, move); }
        TURN { MOVE(player, MOVE_SKILL_DRAIN); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SKILL_DRAIN, player);
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_MUL_EQ(results[0].damage, Q_4_12(2.0), results[1].damage);
    }
}

SINGLE_BATTLE_TEST("Skill Drain suppresses the target's ability")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_SKILL_DRAIN); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SKILL_DRAIN, player);
        HP_BAR(opponent);
        MESSAGE("The opposing Wobbuffet's Ability was suppressed!");
    } THEN {
        EXPECT(opponent->volatiles.gastroAcid);
    }
}

SINGLE_BATTLE_TEST("Skill Drain doesn't suppress an ability that is already suppressed")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_GASTRO_ACID); }
        TURN { MOVE(player, MOVE_SKILL_DRAIN); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SKILL_DRAIN, player);
        HP_BAR(opponent);
        NOT MESSAGE("The opposing Wobbuffet's Ability was suppressed!");
    }
}

SINGLE_BATTLE_TEST("Skill Drain doesn't suppress the target's ability if the user has Sheer Force")
{
    GIVEN {
        PLAYER(SPECIES_TAUROS) { Ability(ABILITY_SHEER_FORCE); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_SKILL_DRAIN); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SKILL_DRAIN, player);
        HP_BAR(opponent);
        NOT MESSAGE("The opposing Wobbuffet's Ability was suppressed!");
    } THEN {
        EXPECT(!opponent->volatiles.gastroAcid);
    }
}

SINGLE_BATTLE_TEST("Skill Drain doesn't suppress the target's ability if it holds an Ability Shield")
{
    GIVEN {
        ASSUME(gItemsInfo[ITEM_ABILITY_SHIELD].holdEffect == HOLD_EFFECT_ABILITY_SHIELD);
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET) { Item(ITEM_ABILITY_SHIELD); }
    } WHEN {
        TURN { MOVE(player, MOVE_SKILL_DRAIN); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SKILL_DRAIN, player);
        HP_BAR(opponent);
        NOT MESSAGE("The opposing Wobbuffet's Ability was suppressed!");
    } THEN {
        EXPECT(!opponent->volatiles.gastroAcid);
    }
}
