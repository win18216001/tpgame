
#pragma  once 

#include <vector>
#include "../Render/Vector2D.h"
#include "SupportSpotCalculator.h"

class Goal;
class PlayerBase;
class FieldPlayer;
class SoccerPitch;
class GoalKeeper;
class SupportSpotCalculator;
template<class T>
class StateMachine;
class SupportSpotCalculator;
	  
class SoccerTeam 
{
public:

	enum team_color {blue, red};		  /// �������

public:

	SoccerTeam(Goal*        home_goal,
		Goal*        opponents_goal,
		SoccerPitch* pitch,
		team_color   color);

	~SoccerTeam();

	//the usual suspects
	void        Render()const;
	void        Update();

	//calling this changes the state of all field players to that of 
	//ReturnToHomeRegion. Mainly used when a goal keeper has
	//possession
	void        ReturnAllFieldPlayersToHome()const;

	//returns true if player has a clean shot at the goal and sets ShotTarget
	//to a normalized vector pointing in the direction the shot should be
	//made. Else returns false and sets heading to a zero vector
	///  �����Ƿ������Ž���
	bool        CanShoot(Vector2D  BallPos,
						 double    power, 
						 Vector2D& ShotTarget = Vector2D())const;

	///
	/// ��Ա���ôη����������Ƿ��ܹ�����������ԣ���ô˭����ѵĴ������
	/// ʲôλ��������ѵĴ���λ��
	/// 
	bool        FindPass(const PlayerBase*const passer,
						 PlayerBase*&  receiver,
						 Vector2D&     PassTarget,
						 double        power,
						 double        MinPassingDistance)const;

	/// ����һ�������ߺ�һ����Ӧ�ߣ�����������Խ����߸��������ɲ�ͬλ�ã�
	/// �ж��Ƿ���԰�ȫ�Ĵ���������Դ���
	/// ��ô�÷�������Ѵ��򱣴��ڲ���PassTarget��
	bool        GetBestPassToReceiver(const PlayerBase* const passer,
									  const PlayerBase* const receiver,
									  Vector2D& PassTarget,
									  const double power)const;

	/// ����FORM��target�Է���Աopp�Ƿ���Զ�������
	bool        isPassSafeFromOpponent( Vector2D    from,
										Vector2D    target,
										const PlayerBase* const receiver,
										const PlayerBase* const opp,
										double       PassingForce)const;

	/// ����FORM��target�Է����е���Ա�Ƿ���Զ�������
	bool        isPassSafeFromAllOpponents(Vector2D from,
						Vector2D target,
						const PlayerBase* const receiver,
						double     PassingForce)const;

	/// �Է���Ա���ж�Ա�ķ�����Χ
	bool        isOpponentWithinRadius(Vector2D pos, double rad);

	/// ��������Ա����
	void        RequestPass(FieldPlayer* requester)const;

	/// ������Ѵ���λ�ã�����������ʵĶԷ���Աǰ������ص�
	PlayerBase* DetermineBestSupportingAttacker();


	const std::vector<PlayerBase*>& Members()const{return m_Players;}  

	StateMachine<SoccerTeam>* GetFSM()const{return m_pStateMachine;}

	Goal*const           HomeGoal()const{return m_pHomeGoal;}
	Goal*const           OpponentsGoal()const{return m_pOpponentsGoal;}

	SoccerPitch*const    Pitch()const{return m_pPitch;}           

	SoccerTeam*const     Opponents()const{return m_pOpponents;}
	void                 SetOpponents(SoccerTeam* opps){m_pOpponents = opps;}

	team_color           Color()const{return m_Color;}

	void                 SetPlayerClosestToBall(PlayerBase* plyr){m_pPlayerClosestToBall=plyr;}
	PlayerBase*          PlayerClosestToBall()const{return m_pPlayerClosestToBall;}

	double               ClosestDistToBallSq()const{return m_dDistSqToBallOfClosestPlayer;}

	Vector2D             GetSupportSpot()const{return m_pSupportSpotCalc->GetBestSupportingSpot();}

	PlayerBase*          SupportingPlayer()const{return m_pSupportingPlayer;}
	void                 SetSupportingPlayer(PlayerBase* plyr){m_pSupportingPlayer = plyr;}

	PlayerBase*          Receiver()const{return m_pReceivingPlayer;}
	void                 SetReceiver(PlayerBase* plyr){m_pReceivingPlayer = plyr;}

	PlayerBase*          ControllingPlayer()const{return m_pControllingPlayer;}
	void                 SetControllingPlayer(PlayerBase* plyr)
	{
		m_pControllingPlayer = plyr;

		//rub it in the opponents faces!
		Opponents()->LostControl();
	}


	bool  InControl()const{if(m_pControllingPlayer)return true; else return false;}
	void  LostControl(){m_pControllingPlayer = NULL;}

	PlayerBase*  GetPlayerFromID(int id)const;


	void SetPlayerHomeRegion(int plyr, int region)const;

	void DetermineBestSupportingPosition()const
	{
		m_pSupportSpotCalc->DetermineBestSupportingPosition();
	}

	void UpdateTargetsOfWaitingPlayers()const;

	//returns false if any of the team are not located within their home region
	bool AllPlayersAtHome()const;

	std::string Name() const 
	{
		if (m_Color == blue) 
			return "Blue"; 
		return "Red";
	}

	bool  IsChaseBall() const	   { return m_IsChase; }
	void  SetChaseBall(bool chase) { m_IsChase = chase;}

	bool  IsThrowIn() const	   { return m_IsThrowIn; }
	void  SetThrowIn(bool throwin) { m_IsThrowIn = throwin;}

private:
	//creates all the players for this team
	void CreatePlayers();

	///> ÿһ֡���㵱ǰ�����������Ա
	void CalculateClosestPlayerToBall();			

	bool m_IsChase; 
	bool m_IsThrowIn;

private:

	StateMachine<SoccerTeam>*  m_pStateMachine;///> ��ǰ�����״̬��
	team_color                m_Color;	       ///> ������ɫ
	std::vector<PlayerBase*>  m_Players;	   ///> �������еĳ�Ա
	SoccerPitch*              m_pPitch;		   ///> ָ�����ָ��
	Goal*                     m_pOpponentsGoal;///> �Է�����
	Goal*                     m_pHomeGoal;	   ///> �Լ�����
	SoccerTeam*               m_pOpponents;	   ///> �жԶ���ָ��

	PlayerBase*               m_pControllingPlayer;	 ///> �����Ա
	PlayerBase*               m_pSupportingPlayer;	 ///> ��Ӧ��Ա
	PlayerBase*               m_pReceivingPlayer;	 ///> ָ������Ա
	PlayerBase*               m_pPlayerClosestToBall;///> ָ����������Ķ�Ա

	double                     m_dDistSqToBallOfClosestPlayer;///> �������Ա����ľ���

	SupportSpotCalculator*    m_pSupportSpotCalc;	 ///>  �������߶�Ա����Ϊ��

};