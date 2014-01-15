#pragma once
#include "StdAfx.h"

class MenuAnalyzer
{
private:
	static MenuAnalyzer* singleton;
public:
	MenuAnalyzer(void);
	~MenuAnalyzer(void);
	Memory MenuSelector(Memory);
	static MenuAnalyzer* GetMenuAnalyzer();
private:
	bool packetToLogin(__out IN_Login&,__in Memory& memory);
	bool packetToSignup(__out IN_Signup&, __in Memory& memory);
	bool packetToLogout(__out IN_Logout&, __in Memory& memory);
	bool packetToLeave(__out IN_Leave&, __in Memory& memory);
	bool packetToSearch(__out IN_Search&, __in Memory& memory);
	bool packetToMore(__out IN_More&, __in Memory& memory);
	bool packetToWriteComment(__out IN_Write_comment&, __in Memory& memory);
	bool packetToModifyComment(__out IN_Modify_comment&, __in Memory& memory);
	bool packetToDeleteComment(__out IN_Delete_comment&, __in Memory& memory);
	bool packetToLike(__out IN_Like&, __in Memory& memory);
	bool packetToDislike(__out IN_Dislike&, __in Memory& memory);
	bool packetToReport(__out IN_Report&, __in Memory& memory);

	bool packetFromLogin(__out u_char*&, __in OUT_Login&);
	bool packetFromSignup(__out u_char*&, __in OUT_Signup&);
	bool packetFromLogout(__out u_char*&, __in OUT_Logout&);
	bool packetFromLeave(__out u_char*&, __in OUT_Leave&);
	bool packetFromSearch(__out u_char*&, __in OUT_Search&);
	bool packetToMore(__out u_char*&, __in OUT_More&);
	bool packetFromSWriteComment(__out u_char*&, __in OUT_More&);
	bool packetFromModifyComment(__out u_char*&, __in OUT_Write_comment&);
	bool packetFromDeleteComment(__out u_char*&, __in OUT_Modify_comment&);
	bool packetFromLike(__out u_char*&, __in OUT_Delete_comment&);
	bool packetFromDislike(__out u_char*&, __in OUT_Like&);
	bool packetFromReport(__out u_char*&, __in OUT_Dislike&);
};