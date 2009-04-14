#include "dropitem.h"

DropItem::DropItem(int id)
	: GameObject(id)
{
	init();
	m_item =0;
}

DropItem::DropItem(Item* item)
	: GameObject(item->m_id)
{
	m_item = item;
	setSubtype(item->m_subtype);
	init();
}

void DropItem::init()
{
	setBaseType(DROPITEM);
	setType("DROPITEM");
	
	setAngle(Random::random()*3.14159*2);

	float hadd = 0.1;
	float acc = -20;
	float v0 =4;
	setHeight(2.0 +hadd);
	
	
	float t = -v0/(2*acc)+sqrt(v0*v0/(4*acc*acc) - 2/acc);
	m_drop_speed = 4/t;
	m_angle_x = 4;
	
	m_time =0;
}

DropItem::~DropItem()
{
	
}

void DropItem::toString(CharConv* cv)
{
	GameObject::toString(cv);
	m_item->toString(cv);
}

void DropItem::fromString(CharConv* cv)
{
	GameObject::toString(cv);
	m_item->fromString(cv);
}


bool DropItem::update(float time)
{
	m_time += time;
	float hadd = 0.1;
			
	if (getHeight()>hadd)
	{
		float acc = -20;
		float v0 =4;
		setHeight(hadd+ 2 + v0*m_time/1000 + acc*m_time*m_time/1000000);
		m_angle_x -= m_drop_speed*time/1000;
		
		if (getHeight() < hadd)
		{
			setHeight(hadd);
			m_angle_x=0;
		}
		DEBUG5("speed %f height %f angle %f time %f",m_drop_speed,getHeight(),m_angle_x,m_time); 
	}
	return true;
}

int DropItem::getLocationId()
{
	short sx = (int) (getPosition().m_x*2.0 + 0.5);
	short sy = (int) (getPosition().m_y*2.0 + 0.5);
	return 10000*sx + sy;
}
