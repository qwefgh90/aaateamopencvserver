#include "stdafx.h"
#include "Packet.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPacket::CPacket(P_PROTODATA data,int index)
{
	strcpy(m_Data.szContent,data->szContent);
	m_Data.nType = data->nType;
	m_Index = index;
	if (m_Data.nType == SM_CONNECT)
		m_Data.grade = data->grade;
	else
		m_Data.grade = 0;
}
CPacket::CPacket()
{
	memset(&m_Data,0,sizeof(PROTODATA));
}
CPacket::~CPacket()
{

}
