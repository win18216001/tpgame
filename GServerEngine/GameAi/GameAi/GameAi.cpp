// GameAi.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"

/// 状态机测试

#include "Public/StateSet.h"
#include "Public/EntityManager.h"
#include "Public/StateMachine.h"
#include "Public/Event.h"
#include "Entity/CHumanEntity.h"
#include "Entity/CMonsterEntity.h"

/// 脚本测试头文件
#include "Script/LuaInterface.h"


int _tmain(int argc, _TCHAR* argv[])
{
	
	return 0;
}

/** 
/// 脚本测试
	// tp_script::CLuaInterface lua ;
	// lua.Init();

	// lua.ExeScript( 0 ,"test.lua" );
	// lua.ExeScript( 0, "fun.lua" );

GetInstObj(CStateSet).Create();

CHumanEntity		human1(Ent_Player);
CHumanEntity		human2(Ent_Player);
CHumanEntity		human3(Ent_Player);

// 	CMonsterEntity		Monster1(Ent_Monster);
// 	CMonsterEntity		Monster2(Ent_Monster);
// 	CMonsterEntity		Monster3(Ent_Monster);

GetInstObj(CEntityManager).RegEntityManager( &human1 );
GetInstObj(CEntityManager).RegEntityManager( &human2 );
GetInstObj(CEntityManager).RegEntityManager( &human3 );

// 	GetInst(CEntityManager).RegEntityManager( &Monster1 );
// 	GetInst(CEntityManager).RegEntityManager( &Monster2 );
// 	GetInst(CEntityManager).RegEntityManager( &Monster3 );

while ( 1 )
{
/// 处理所有的ai
GetInstObj(CEntityManager).Update();
/// 处理所有的事件
GetInstObj(CEventDispatch).DispacthEevent();

Sleep( 100 );
}
*/