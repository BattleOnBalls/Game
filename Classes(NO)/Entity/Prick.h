#ifndef _PRICK_H_
#define _PRICK_H_

#include "Entity.h"

class Prick : public Entity
{
public:
	Prick() { }
	~Prick() { }

	//picturename���̴̵�ͼƬ
	static Prick * create(const std::string& picturename);
	bool init(const std::string& picturename);
};

#endif