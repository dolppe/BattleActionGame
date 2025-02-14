#include "BattleCombatData.h"

#include "AttackCollisionMethod.h"
#include "BattleCombatManagerComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(BattleCombatData)

UAttackCollisionData_SocketBasedLineTrace::UAttackCollisionData_SocketBasedLineTrace()
{
	CollisionMethodType = ECollisionMethodType::SocketBasedLineTrace;
}

UAttackCollisionData_DirectionalSweep::UAttackCollisionData_DirectionalSweep()
{
	CollisionMethodType = ECollisionMethodType::DirectionalSweep;
}

UAttackCollisionData_CircularAOE::UAttackCollisionData_CircularAOE()
{
	CollisionMethodType = ECollisionMethodType::CircularAOE;
}

UBattleCombatData::UBattleCombatData()
{
}
