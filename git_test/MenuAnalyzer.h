#pragma once
#include "StdAfx.h"
#include "Member_manager.h"
#include "Store_manager.h"
class MenuAnalyzer
{
private:
	static MenuAnalyzer* singleton;
public:
	MenuAnalyzer(void);
	~MenuAnalyzer(void);
	static MenuAnalyzer* GetMenuAnalyzer();
	bool MenuAnalyzer::MenuSelector(Memory& in_memory,Memory& out_memory);
	bool MenuAnalyzer::split(__in vector<string*>& v,string str, string sep);
	Member_manager* member_manager;
	Store_manager* store_manager;
	void freeImage(Memory& m);

private:
	//packet to structure of login
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
	bool packetToReport(__out IN_Report&, __in Memory& memory);
	bool packetToCache(__out IN_Cache&, __in Memory& memory);

	//structure of login to packet structure
	bool packetFromLogin(__out Memory&, __in OUT_Login&);
	bool packetFromSignup(__out Memory&, __in OUT_Signup&);
	bool packetFromLogout(__out Memory&, __in OUT_Logout&);
	bool packetFromLeave(__out Memory&, __in OUT_Leave&);
	bool packetFromSearch(__out Memory&, __in OUT_Search&);
	bool packetFromMore(__out Memory&, __in OUT_More&);
	bool packetFromWriteComment(__out Memory& out, __in OUT_Write_comment& in);
	bool packetFromModifyComment(__out Memory& out, __in OUT_Modify_comment& in);
	bool packetFromDeleteComment(__out Memory& out, __in OUT_Delete_comment& in);
	bool packetFromLike(__out Memory&, __in OUT_Like&);
	bool packetFromReport(__out Memory& out, __in _OUT_Report& in);
	bool packetFromError(__out Memory& out,__in u_char err_code);
	bool packetFromCache(__out Memory& out,__in OUT_Cache& in);
};