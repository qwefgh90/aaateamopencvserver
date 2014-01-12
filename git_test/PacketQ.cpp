#include "stdafx.h"

#include "PacketQ.h"

CPacketQ::CPacketQ()
{
	InitializeCriticalSection( &m_CS );
	hEvent = CreateEvent(NULL,true,false,NULL);
}

CPacketQ::~CPacketQ()
{
	DeleteCriticalSection( &m_CS );
}

// 큐작업을 할때는 동기화를 보장해주어야한다.
int CPacketQ::GetSize()
{
	return m_PacketQ.size();
}

bool CPacketQ::Get(CPacket &p)
{
	WaitForSingleObject(hEvent,INFINITE);

	EnterCriticalSection( &m_CS );
	if( m_PacketQ.empty() )
	{
		ResetEvent(hEvent);
		LeaveCriticalSection( &m_CS );
		return false;
	}

	p = *m_PacketQ.begin();
	m_PacketQ.pop_front();

	LeaveCriticalSection( &m_CS );

	if( !m_PacketQ.size() )
	{
		ResetEvent(hEvent);
	}

	return true;
}

void CPacketQ::Put(CPacket &p)
{
	EnterCriticalSection( &m_CS );
	m_PacketQ.push_back(p);
	LeaveCriticalSection( &m_CS );

	SetEvent(hEvent);
}

void CPacketQ::Clear()
{
	EnterCriticalSection( &m_CS );
	m_PacketQ.clear();
	LeaveCriticalSection( &m_CS );
}
