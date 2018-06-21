#include "Joystick.h"

const float SJ_PI = 3.14;
const float SJ_RAD2DEG = 180.0f / SJ_PI; //弧度转换成角度
const float SJ_DEG2RAD = SJ_PI / 180.0f; //角度转换成弧度

enum JoystickZOrder
{
	JOYSTICK_BACKGROUND_Z,
	JOYSTICK_THUMB_Z
};

Joystick::Joystick()
{
	_thumb = nullptr;
	_joystickRadius = 0.0;
	_joystickRadiusSq = 0.0;
	_thumbRadius = 0.0;
	_thumbRadiusSq = 0.0;
	_deadRadius = 0.0;
	_deadRadiusSq = 0.0;
	_velocity = Vec2::ZERO;
	_isDPad = false;
	_hasDead = false;
	_autoCenter = false;
	_directionNum = 4;
}

Joystick::~Joystick()
{

}

Joystick * Joystick::create(const std::string & backgroundPath, const std::string & thumbPath)
{
	Joystick * joystick = new Joystick();
	if (joystick && joystick->init(backgroundPath, thumbPath))
	{
		joystick->autorelease();
		return joystick;
	}
	CC_SAFE_DELETE(joystick);
	return nullptr;
}

bool Joystick::init(const std::string & backgroundPath, const std::string & thumbPath)
{
	if (!Node::init())
	{
		return false;
	}

	auto background = Sprite::create(backgroundPath.c_str());
	_joystickRadius = background->getContentSize().width / 2;
	_joystickRadiusSq = _joystickRadius * _joystickRadius;
	this->addChild(background, JOYSTICK_BACKGROUND_Z);
	this->setContentSize(background->getContentSize());

	_thumb = Sprite::create(thumbPath.c_str());
	_thumbRadius = _thumb->getContentSize().width / 2;
	_thumbRadiusSq = _thumbRadius * _thumbRadius;
	this->addChild(_thumb, JOYSTICK_THUMB_Z);

	return true;
}

bool Joystick::onTouchBegan(Touch * touch, Event * event)
{
	Vec2 position = touch->getLocation();

	position = this->convertToNodeSpace(position);
	//Do a fast rect check before doing a circle hit check:
	if (position.lengthSquared() <= _joystickRadiusSq) {
		this->updateVelocity(position);
		return true;
	}
	return false;
}

void Joystick::onTouchMoved(Touch * touch, Event * event)
{
	Vec2 position = touch->getLocation();
	position = this->convertToNodeSpace(position);
	this->updateVelocity(position);
}

void Joystick::onTouchEnded(Touch * touch, Event * event)
{
	Vec2 position = Vec2::ZERO;
	if (!_autoCenter)
	{
		position = touch->getLocation();
		position = this->convertToNodeSpace(position);
	}
	this->updateVelocity(position);
}

void Joystick::onTouchCancelled(Touch * touch, Event * event)
{
	this->onTouchEnded(touch, event);
}


Vec2 Joystick::getVelocity()
{
	return _velocity;
}

void Joystick::updateVelocity(Vec2 point)
{
	// Calculate distance and angle from the center.
	double d = point.length();
	double dSq = d * d;

	if (dSq <= _deadRadiusSq) {
		_velocity = Vec2::ZERO;
		_thumb->setPosition(point);
		return;
	}

	float angle = atan2f(point.y, point.x); // in radians
	if (angle < 0) {
		angle += SJ_PI * 2;
	}
	float cosAngle;
	float sinAngle;

	if (_isDPad) {
		float anglePerSector = 360.0f / _directionNum * SJ_DEG2RAD;
		angle = round(angle / anglePerSector) * anglePerSector;
	}

	cosAngle = cosf(angle);
	sinAngle = sinf(angle);

	float dx = d * cosAngle;
	float dy = d * sinAngle;
	// NOTE: Velocity goes from -1.0 to 1.0.
	if (dSq > _joystickRadiusSq) {
		dx = cosAngle * _joystickRadius;
		dy = sinAngle * _joystickRadius;
	}

	_velocity = Vec2(dx, dy);
	_velocity.normalize();

	_thumb->setPosition(dx, dy);
}

void Joystick::setDirectionNum(int num)
{
	_isDPad = true;
	_directionNum = num;
}


void Joystick::setDeadRadius(float r)
{
	_deadRadius = r;
	_deadRadiusSq = r * r;
	_hasDead = true;
}

void Joystick::setAutoCenter(bool b)
{
	_autoCenter = b;
}

float Joystick::round(float r) //四舍五入
{
	return (r > 0.0) ? floor(r + 0.5) : ceil(r - 0.5);
}