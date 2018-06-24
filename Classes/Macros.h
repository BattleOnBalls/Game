#ifndef _MACROS_H_
#define _MACROS_H_

#define MAP_WIDTH 4800     //地图宽度，单位像素
#define MAP_HEIGHT 3600     //地图高度，单位像素

#define DESIGN_SCREEN_WIDTH 800     //设计屏幕分辨率，宽度
#define DESIGN_SCREEN_HEIGHT 450     //设计屏幕分辨率，高度


//BEAN_MACROS
#define BEAN_RADIUS 8     //豆豆的半径
#define BEAN_SCORE 1     //一个豆豆的分值


//SPORE_MACROS
#define SPORE_RADIUS 14     //孢子的半径
#define SPORE_SCORE 10     //一个孢子的分值


//PRICK_MACROS
#define PRICK_RADIUS 62     //绿刺的半径
#define PRICK_SCORE 100     //绿刺的分值


//PLAYERDIVISION_MACROS
#define PLAYER_INITIAL_RADIUS 19     //玩家球球的初始半径
#define PLAYER_INITIAL_SCORE 10     //玩家初始分值
#define PLAYER_INITIAL_ISPEED 6      //玩家球球的初始速度
#define PLAYER_MIN_SPEED 1     //球球运动的最小速度
#define PLAYER_MIN_EAT_SPORE_SCORE 20     //达到这个分值，球球才可以吃孢子
#define AFTER_PRICK_SCORE 20     //被裂之后裂出的小球的分值


//PLAYER_MACROS
#define PI 3.14f
#define PLAYER_MIN_DIVIDE_SCORE 40     //达到这个分值，球球才可以分裂
#define PLAYER_MIN_SPIT_SCORE 60     //达到这个分值，球球才能吐孢子

#define PLAYER_MAX_DIVISION_NUM 16     //玩家最大分身数量
#define EAT_PRICK_DIVISION_NUM 9     //碰到绿刺产生的分身数量

#define PLAYER_MIN_DIVIDE_DISTANCE 180     //玩家分身之间的最小距离
#define PLAYER_MIN_SPIT_DISTANCE 220     //玩家吐的孢子与分身之间的最小距离
#define PRICK_SPLIT_DISTANCE 128      //碰到绿刺后分裂出的小球与原体的距离
#define PLAYER_CONCENTRATE_SPEED 1.0     //玩家中和速度


//DATALAYER_MACROS
#define GAME_TOTAL_TIME 100     //游戏总时间
#define RANK_MAX_ITEM_NUM 10     //排行榜上最多显示多少个玩家
#define RANK_ITEM_HEIGHT 20     //排行榜里一个框框显示一个玩家，这是一个框框的高度
#define RANK_ITEM_MARGIN 3     //排行榜里两块框框的间隙


//GAMESCENE_MACROS
#define SPITE_POS_X 650     //吐球图标x坐标
#define SPITE_POS_Y 60     //吐球图标y坐标
#define DIVIDE_POS_X 720     //分裂图标x坐标
#define DIVISDE_POS_Y 60     //分裂图标y坐标


//GAMELAYER_MACROS
#define MAP_DIVISION_X 6     //地图横向划分，整个地图的宽抵6个屏幕的宽
#define MAP_DIVISION_Y 8     //地图纵向划分，整个地图的高抵8个屏幕的高
#define MAP_DIVISION_BEAN_NUM 100     //每个划分区域的豆子数量

//TRANSLATE_BETWEEN_SCENE
extern int SKIN;     //保存皮肤号以便在各场景使用
extern std::string NAME;     //玩家的名称
extern bool REMEMBER;     //如果玩家是再次进入menuscene，那么不再随机产生昵称，保留原来的昵称
extern int SCORE;     //玩家分数

#endif