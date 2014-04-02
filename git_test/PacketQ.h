
#include "stdafx.h"
#if !defined(AFX_PACKETQ_H__D1A6047C_789A_4294_9DAB_9BDCF8AB0329__INCLUDED_)
#define AFX_PACKETQ_H__D1A6047C_789A_4294_9DAB_9BDCF8AB0329__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "Packet.h"

using namespace std;

class CPacketQ  
{
	CRITICAL_SECTION m_CS;
	HANDLE hEvent;
	list<CPacket> m_PacketQ;
public:
	void Clear();
	void Put(CPacket &p);
	bool Get(CPacket &p);
	int GetSize();
	CPacketQ();
	virtual ~CPacketQ();

};

#endif // !defined(AFX_PACKETQ_H__D1A6047C_789A_4294_9DAB_9BDCF8AB0329__INCLUDED_)