#include "dropitem.h"

void DropItem::toString(CharConv* cv)
{

	m_item->toString(cv,0);
	cv->toBuffer(m_x);
	cv->toBuffer(m_y);
	

	
}

void DropItem::fromString(CharConv* cv)
{
	short pos;
	
	m_item->fromString(cv,pos);
	cv->fromBuffer<short>(m_x );
	cv->fromBuffer<short>(m_y);
	
}


int DropItem::getId()
{
	return 10000* m_x + m_y;
}

string DropItem::getNameId()
{
	ostringstream out_stream;
	
	out_stream.str("");
	out_stream << m_item->getName() << ":" << getId();
	return out_stream.str();
}
