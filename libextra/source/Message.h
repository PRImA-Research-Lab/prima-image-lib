#pragma once

#include "extrastring.h"
#include <vector>
#include <set>

namespace PRImA
{

/*
 * Class CMessage
 *
 * Message with ID that can be used for infos, warnings and errors.
 *
 * CC 17.10.2012 - created
 */

class CMessage
{
public:
	static const int LEVEL_INFO		= 1;
	static const int LEVEL_WARNING	= 2;
	static const int LEVEL_ERROR	= 3;

public:
	CMessage(int id);
	CMessage(int id, int level);
	CMessage(int id, CUniString caption);
	CMessage(int id, CUniString caption, CUniString description);
	CMessage(int id, int level, CUniString caption);
	CMessage(int id, int level, CUniString caption, CUniString description);
	CMessage(int id, int level, CUniString caption, CUniString description, CUniString custom);

	~CMessage(void);

	CMessage * Clone();

	inline int GetId() { return m_Id; };
	inline int GetLevel() { return m_Level; };
	inline bool IsInfo() { return m_Level == LEVEL_INFO; };
	inline bool IsWarning() { return m_Level == LEVEL_WARNING; };
	inline bool IsError() { return m_Level == LEVEL_ERROR; };
	inline CUniString GetCaption() { return m_Caption; };
	inline CUniString GetDescription() { return m_Description; };
	inline CUniString GetCustomField() { return m_Custom; };

private:
	int m_Id;
	int m_Level;
	CUniString m_Caption;
	CUniString m_Description;
	CUniString m_Custom;
};


/*
 * Class CMessages
 *
 * List of messages
 *
 * CC 17.10.2012 - created
 */

class CMessages
{
public:
	CMessages();
	~CMessages();

	inline int GetSize() { return (int)m_List.size(); };
	inline CMessage * GetMessage(int index) { return m_List.at(index); };

	void AddMessage(CMessage * message);

	bool HasWarnings();
	bool HasErrors();
	bool HasMessage(int id);
	int GetNumberOfWarnings();
	int GetNumberOfErrors();

	void Clear();

	CMessage * RemoveMessage(int index);

	CUniString ComposeText(int level = 0, bool extraLinebreakAfterCaption = true);

private:
	bool HasLevel(int level);
	int GetNumberOfMessages(int level);

private:
	vector<CMessage*> m_List;
	set<int> m_Levels;
};


} // end namespace