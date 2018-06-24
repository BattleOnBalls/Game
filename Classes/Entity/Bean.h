#ifndef Bean_hpp
#define Bean_hpp

#include "Entity.h"

//小豆豆
class Bean : public Entity
{
public:
	//构造函数
	Bean();
	//析构函数
	~Bean();

	//创建小豆豆
	static Bean* create(const std::string &filename);
	//初始化小豆豆
	bool init(const std::string &filename);
};

#endif