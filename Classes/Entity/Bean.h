#ifndef Bean_hpp
#define Bean_hpp

#include "Entity.h"

//С����
class Bean : public Entity
{
public:
	//���캯��
	Bean();
	//��������
	~Bean();

	//����С����
	static Bean* create(const std::string &filename);
	//��ʼ��С����
	bool init(const std::string &filename);
};

#endif