
#include "stdafx.h"
#if !defined(AFX_INDEXQ_H__E1F43DDC_47A6_4CAA_BCB5_283D3B4EA4B7__INCLUDED_)
#define AFX_INDEXQ_H__E1F43DDC_47A6_4CAA_BCB5_283D3B4EA4B7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
class CIndexQ  
{
	CRITICAL_SECTION m_CS;
	int *m_QPtr;

	int m_Front;
	int	m_Rear;
	int m_Size;
public:
	void Clear()	{	m_Front = m_Rear = 0;					}
	bool IsEmpty()	{	return (m_Front==m_Rear);				}
	bool IsFull()	{	return (((m_Front+1)%m_Size)==m_Rear);	}

	bool Get(int *var)
	{
		EnterCriticalSection( &m_CS );
		if ( IsEmpty() )
		{
			LeaveCriticalSection( &m_CS );
			return false;
		}
		*var = m_QPtr[ m_Rear ];
		m_Rear = (m_Rear+1)%m_Size;
		LeaveCriticalSection( &m_CS );
		return true;
	}
	bool Put(int var)
	{
		EnterCriticalSection( &m_CS );
		if ( IsFull() )
		{
			LeaveCriticalSection( &m_CS );
			return false;
		}
		m_QPtr[ m_Front ] = var;
		m_Front = (m_Front+1)%m_Size;
		LeaveCriticalSection( &m_CS );
		return true;
	}
	bool Peek(int *var)
	{
		EnterCriticalSection( &m_CS );
		if ( IsEmpty() )
		{
			LeaveCriticalSection( &m_CS );
			return false;
		}
		*var = m_QPtr[ m_Rear ];
		LeaveCriticalSection( &m_CS );
		return true;
	}

	CIndexQ(int size=2000) 
	{
		InitializeCriticalSection( &m_CS );
		m_Size = size;
		m_QPtr = 0;
		
		if ( size>0 )
			m_QPtr = new int[m_Size+1];

		Clear();
	}
	virtual ~CIndexQ()
	{
		delete [] m_QPtr;
		DeleteCriticalSection( &m_CS );
	}

};

#endif // !defined(AFX_INDEXQ_H__E1F43DDC_47A6_4CAA_BCB5_283D3B4EA4B7__INCLUDED_)
