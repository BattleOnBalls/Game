#ifndef MessageType_h
#define MessageType_h

/*用于消息通讯解析的协议号*/
namespace MessageType
{
	enum eMsgtype
	{
		eMsg_START_GAME_SINGLE,             //开始游戏*
		eMsg_START_GAME_SINGLE_RESULT,      //开始游戏返回结果*
		eMsg_MOVE,                          //玩家移动操作*
		eMsg_DIVIDE,                        //玩家分身操作*
		eMsg_SPIT_SPORE,                    //玩家吐孢子操作*
		eMsg_SPIT_SPORE_RESULT,             //玩家吐孢子结果*
		eMsg_PLAYER_CONCENTRATE,            //玩家中和操作*
		eMsg_UPDATE_POSITION,               //更新玩家位置*
		eMsg_UPDATE_SPORE,                  //更新孢子*
		eMsg_UPDATE_TIME,                   //更新游戏时间*
		eMsg_ENTER_PLAYER,                  //新玩家进入*
		eMsg_ADD_PRICK,                     //加刺*
		eMsg_EAT_PRICK,                     //吞刺*
		eMsg_A_ASSUMPTION,                 //解决冲突的一个设想，大概没有用(cry
	};
};

#endif /* MessageType_h */