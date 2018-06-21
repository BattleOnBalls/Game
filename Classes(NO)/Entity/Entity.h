#ifndef Entity_hpp
#define Entity_hpp

#include "cocos2d.h"

USING_NS_CC;

//实体基类
class Entity : public Sprite
{
public:
    //获取半径
    float getRadius();
    //设置半径
    void setRadius(float radius);
    //获取分数
    int getScore();
    //设置分数
    virtual void setScore(int score);
    
protected:
    float _radius;
    int _score;
};

#endif /* Entity_hpp */