#ifndef _MACROS_H_
#define _MACROS_H_

#define MAP_WIDTH 4800     //��ͼ��ȣ���λ����
#define MAP_HEIGHT 3600     //��ͼ�߶ȣ���λ����

#define DESIGN_SCREEN_WIDTH 800     //�����Ļ�ֱ��ʣ����
#define DESIGN_SCREEN_HEIGHT 450     //�����Ļ�ֱ��ʣ��߶�


//BEAN_MACROS
#define BEAN_RADIUS 8     //�����İ뾶
#define BEAN_SCORE 1     //һ�������ķ�ֵ


//SPORE_MACROS
#define SPORE_RADIUS 14     //���ӵİ뾶
#define SPORE_SCORE 10     //һ�����ӵķ�ֵ


//PRICK_MACROS
#define PRICK_RADIUS 62     //�̴̵İ뾶
#define PRICK_SCORE 100     //�̴̵ķ�ֵ


//PLAYERDIVISION_MACROS
#define PLAYER_INITIAL_RADIUS 19     //�������ĳ�ʼ�뾶
#define PLAYER_INITIAL_SCORE 10     //��ҳ�ʼ��ֵ
#define PLAYER_INITIAL_ISPEED 6      //�������ĳ�ʼ�ٶ�
#define PLAYER_MIN_SPEED 1     //�����˶�����С�ٶ�
#define PLAYER_MIN_EAT_SPORE_SCORE 20     //�ﵽ�����ֵ������ſ��Գ�����
#define AFTER_PRICK_SCORE 20     //����֮���ѳ���С��ķ�ֵ


//PLAYER_MACROS
#define PI 3.14f
#define PLAYER_MIN_DIVIDE_SCORE 40     //�ﵽ�����ֵ������ſ��Է���
#define PLAYER_MIN_SPIT_SCORE 60     //�ﵽ�����ֵ���������������

#define PLAYER_MAX_DIVISION_NUM 16     //�������������
#define EAT_PRICK_DIVISION_NUM 9     //�����̴̲����ķ�������

#define PLAYER_MIN_DIVIDE_DISTANCE 180     //��ҷ���֮�����С����
#define PLAYER_MIN_SPIT_DISTANCE 220     //����µ����������֮�����С����
#define PRICK_SPLIT_DISTANCE 128      //�����̴̺���ѳ���С����ԭ��ľ���
#define PLAYER_CONCENTRATE_SPEED 1.0     //����к��ٶ�


//DATALAYER_MACROS
#define GAME_TOTAL_TIME 100     //��Ϸ��ʱ��
#define RANK_MAX_ITEM_NUM 10     //���а��������ʾ���ٸ����
#define RANK_ITEM_HEIGHT 20     //���а���һ�������ʾһ����ң�����һ�����ĸ߶�
#define RANK_ITEM_MARGIN 3     //���а���������ļ�϶


//GAMESCENE_MACROS
#define SPITE_POS_X 650     //����ͼ��x����
#define SPITE_POS_Y 60     //����ͼ��y����
#define DIVIDE_POS_X 720     //����ͼ��x����
#define DIVISDE_POS_Y 60     //����ͼ��y����


//GAMELAYER_MACROS
#define MAP_DIVISION_X 6     //��ͼ���򻮷֣�������ͼ�Ŀ��6����Ļ�Ŀ�
#define MAP_DIVISION_Y 8     //��ͼ���򻮷֣�������ͼ�ĸߵ�8����Ļ�ĸ�
#define MAP_DIVISION_BEAN_NUM 100     //ÿ����������Ķ�������

//TRANSLATE_BETWEEN_SCENE
extern int SKIN;     //����Ƥ�����Ա��ڸ�����ʹ��
extern std::string NAME;     //��ҵ�����
extern bool REMEMBER;     //���������ٴν���menuscene����ô������������ǳƣ�����ԭ�����ǳ�
extern int SCORE;     //��ҷ���

#endif