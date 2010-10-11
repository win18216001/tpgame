#include "Stdafx.h"
#include "State.h"
#include "StateMachine.h"	 
#include "../Entity/PlayerBase.h"
#include "../Entity/GoalKeeper.h"		  
#include "../Goal.h"
#include "../SoccerTeam.h"
#include "../SoccerPitch.h"
#include "../Messageing/MessageDispatcher.h"
#include "../SteeringBehaviors.h"
#include "../../Public/MsgImpl.h"
#include "../../Config.h"		

//uncomment to send state info to debug window
//#define GOALY_STATE_INFO_ON


EmptyFun(void,GlobalKeeperState,Enter,GoalKeeper);
EmptyFun(void,GlobalKeeperState,Execute,GoalKeeper);
EmptyFun(void,GlobalKeeperState,Exit,GoalKeeper);

bool GlobalKeeperState::OnMessage(GoalKeeper* keeper, const tagMessage& telegram)
{
	switch(telegram.Msg)
	{
	case Msg_GoHome:
		{
			keeper->SetDefaultHomeRegion();

			keeper->GetFSM()->ChangeState(&GetInstObj(ReturnHome));
		}

		break;

	case Msg_ReceiveBall:
		{
			keeper->GetFSM()->ChangeState(&GetInstObj(InterceptBall));
		}

		break;

	}//end switch

	return false;
}


//--------------------------- TendGoal -----------------------------------
//
//  This is the main state for the goalkeeper. When in this state he will
//  move left to right across the goalmouth using the 'interpose' steering
//  behavior to put himself between the ball and the back of the net.
//
//  If the ball comes within the 'goalkeeper range' he moves out of the
//  goalmouth to attempt to intercept it. (see next state)
//------------------------------------------------------------------------


EmptyMsg(bool,TendGoal,OnMessage,GoalKeeper);

void TendGoal::Enter(GoalKeeper* keeper)
{
	//����
	keeper->Steering()->InterposeOn(GetInstObj(CGameSetup).GoalKeeperTendingDistance);

	/// interpose��ʹ�����崦�����Ŀ��֮�䣬�õ�������Ŀ��
	keeper->Steering()->SetTarget(keeper->GetRearInterposeTarget());
}

void TendGoal::Execute(GoalKeeper* keeper)
{
	/// �������λ�õĸı䣬�󷽵Ĳ���Ŀ��λ��Ҳ���ı䣬���Ա�����ÿ�����²������
	keeper->Steering()->SetTarget(keeper->GetRearInterposeTarget());

	/// �������뷶Χ������Աץס����Ȼ��ı�״̬���򴫻ص�������
	if (keeper->BallWithinKeeperRange())
	{
		keeper->Ball()->Trap();

		keeper->Pitch()->SetGoalKeeperHasBall(true);

		keeper->GetFSM()->ChangeState(&GetInstObj(PutBallBackInPlay));

		return;
	}

	/// �������Ԥ����ľ��룬����Ա�ѵ��Ǹ�λ�þ�����ס��
	//if ball is within a predefined distance, the keeper moves out from
	//position to try and intercept it.
	if (keeper->BallWithinRangeForIntercept() && !keeper->Team()->InControl())
	{
		keeper->GetFSM()->ChangeState(&GetInstObj(InterceptBall));
	}

	/// �������Ա��������̫Զ�ˣ�����û�жԷ���Ա����в����Ӧ���ƻ�����
	//if the keeper has ventured too far away from the goal-line and there
	//is no threat from the opponents he should move back towards it
	if (keeper->TooFarFromGoalMouth() && keeper->Team()->InControl())
	{
		keeper->GetFSM()->ChangeState(&GetInstObj(ReturnHome));

		return;
	}
}


void TendGoal::Exit(GoalKeeper* keeper)
{
	keeper->Steering()->InterposeOff();
}


//------------------------- ReturnHome: ----------------------------------
//
//  In this state the goalkeeper simply returns back to the center of
//  the goal region before changing state back to TendGoal
//------------------------------------------------------------------------

EmptyMsg(bool,ReturnHome,OnMessage,GoalKeeper);

void ReturnHome::Enter(GoalKeeper* keeper)
{
	keeper->Steering()->ArriveOn();
}

void ReturnHome::Execute(GoalKeeper* keeper)
{
	keeper->Steering()->SetTarget(keeper->HomeRegion()->Center());

	/// ������ʼ�����㹻�������߶��ֿ������򣬸ı�״̬����
	//if close enough to home or the opponents get control over the ball,
	//change state to tend goal
	if (keeper->InHomeRegion() || !keeper->Team()->InControl())
	{
		keeper->GetFSM()->ChangeState(&GetInstObj(TendGoal));
	}
}

void ReturnHome::Exit(GoalKeeper* keeper)
{
	keeper->Steering()->ArriveOff();
}



//----------------- InterceptBall ----------------------------------------
//
//  In this state the GP will attempt to intercept the ball using the
//  pursuit steering behavior, but he only does so so long as he remains
//  within his home region.
//------------------------------------------------------------------------

EmptyMsg(bool,InterceptBall,OnMessage,GoalKeeper);

void InterceptBall::Enter(GoalKeeper* keeper)
{
	keeper->Steering()->PursuitOn();  

#ifdef GOALY_STATE_INFO_ON
	debug_con << "Goaly " << keeper->ID() << " enters InterceptBall" <<  "";
#endif
}

void InterceptBall::Execute(GoalKeeper* keeper)
{ 
	///	 �������Ա�����ź�Զ����ô��Ӧ���ƻ��Լ������򣬳������������������Ա
	///  ����������£���Ӧ�þ�������ס��
	//if the goalkeeper moves to far away from the goal he should return to his
	//home region UNLESS he is the closest player to the ball, in which case,
	//he should keep trying to intercept it.
	if (keeper->TooFarFromGoalMouth() && !keeper->isClosestPlayerOnPitchToBall())
	{
		keeper->GetFSM()->ChangeState(&GetInstObj(ReturnHome));

		return;
	}

	/// �����������Ա�ֿɴ����ķ�Χ��Ӧ��ץס��Ȼ���ڰ�����������
	//if the ball becomes in range of the goalkeeper's hands he traps the 
	//ball and puts it back in play
	if (keeper->BallWithinKeeperRange())
	{
		keeper->Ball()->Trap();

		keeper->Pitch()->SetGoalKeeperHasBall(true);

		keeper->GetFSM()->ChangeState(&GetInstObj(PutBallBackInPlay));

		return;
	}
}

void InterceptBall::Exit(GoalKeeper* keeper)
{
	keeper->Steering()->PursuitOff();
}


EmptyFun(void,PutBallBackInPlay,Exit,GoalKeeper);
EmptyMsg(bool,PutBallBackInPlay,OnMessage,GoalKeeper);

void PutBallBackInPlay::Enter(GoalKeeper* keeper)
{
	//let the team know that the keeper is in control
	keeper->Team()->SetControllingPlayer(keeper);

	//send all the players �ص���ʼλ��
	keeper->Team()->Opponents()->ReturnAllFieldPlayersToHome();
	keeper->Team()->ReturnAllFieldPlayersToHome();
}


void PutBallBackInPlay::Execute(GoalKeeper* keeper)
{
	PlayerBase*  receiver = NULL;
	Vector2D     BallTarget;

	/// �����Ƿ��ж�Ա�ڸ�ǰ����λ�ã��������ǿ��ܿ��Դ�������������������ô����
	//test if there are players further forward on the field we might
	//be able to pass to. If so, make a pass.
	if (keeper->Team()->FindPass(keeper,
		receiver,
		BallTarget,
		GetInstObj(CGameSetup).MaxPassingForce,
		GetInstObj(CGameSetup).GoalkeeperMinPassDist))
	{     
		//make the pass   
		keeper->Ball()->Kick(Vec2DNormalize(BallTarget - keeper->Ball()->Pos()),
			GetInstObj(CGameSetup).MaxPassingForce,keeper);

		/// ����Ա���ڿ���
		//goalkeeper no longer has ball 
		keeper->Pitch()->SetGoalKeeperHasBall(false);

		/// �ý����Ա֪������������
		//let the receiving player know the ball's comin' at him
		Dispatcher->DispatchMsg(SEND_MSG_IMMEDIATELY,
			keeper->GetID(),
			receiver->GetID(),
			Msg_ReceiveBall,
			&BallTarget);

		/// �ص�����״̬
		//go back to tending the goal   
		keeper->GetFSM()->ChangeState(&GetInstObj(TendGoal));

		return;
	}  

	keeper->SetVelocity(Vector2D());
}
