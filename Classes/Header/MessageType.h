#ifndef MessageType_h
#define MessageType_h

/*������ϢͨѶ������Э���*/
namespace MessageType
{
	enum eMsgtype
	{
		eMsg_START_GAME_SINGLE,             //��ʼ��Ϸ*
		eMsg_START_GAME_SINGLE_RESULT,      //��ʼ��Ϸ���ؽ��*
		eMsg_MOVE,                          //����ƶ�����*
		eMsg_DIVIDE,                        //��ҷ������*
		eMsg_SPIT_SPORE,                    //��������Ӳ���*
		eMsg_SPIT_SPORE_RESULT,             //��������ӽ��*
		eMsg_PLAYER_CONCENTRATE,            //����кͲ���*
		eMsg_UPDATE_POSITION,               //�������λ��*
		eMsg_UPDATE_SPORE,                  //��������*
		eMsg_UPDATE_TIME,                   //������Ϸʱ��*
		eMsg_ENTER_PLAYER,                  //����ҽ���*
		eMsg_ADD_PRICK,                     //�Ӵ�*
		eMsg_EAT_PRICK,                     //�̴�*
		eMsg_A_ASSUMPTION,                 //�����ͻ��һ�����룬���û����(cry
	};
};

#endif /* MessageType_h */