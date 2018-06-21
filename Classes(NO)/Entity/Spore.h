#ifndef _SPORE_H_
#define _SPORE_H_

#include "Entity.h"

class Spore : public Entity
{
public:
	Spore() { }
	~Spore() { }

	//以皮肤图片名称创建spore
	static Spore* create(const std::string& picturename);
	bool init(const std::string& picturename);

	//以皮肤图片序号创建spore
	static Spore* create(int id);
	bool init(int id);

	int getID() { return skin_id; }
private:
	int skin_id;
};

#endif