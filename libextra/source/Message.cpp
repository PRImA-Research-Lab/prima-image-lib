#include "Message.h"


namespace PRImA
{

/*
 * Class CMessage
 *
 * Message with ID that can be used for infos, warnings and errors.
 *
 * CC 17.10.2012 - created
 */

/*
 * Constructor for an info message without caption and description.
 *
 * 'id' - Unique ID for the type of message.
 */
CMessage::CMessage(int id)
{
	CMessage(id, LEVEL_INFO);
}

/*
 * Constructor for a message without caption and description.
 *
 * 'id' - Unique ID for the type of message.
 * 'level' - Info, warning or error (see LEVEL_... constants)
 */
CMessage::CMessage(int id, int level)
{
	CMessage(id, level, L"", L"");
}

/*
 * Constructor for an info message with caption.
 *
 * 'id' - Unique ID for the type of message.
 * 'caption' - Short description of the message.
 */
CMessage::CMessage(int id, CUniString caption)
{
	CMessage(id, LEVEL_INFO, caption, L"");
}

/*
 * Constructor for an info message with caption and description.
 *
 * 'id' - Unique ID for the type of message.
 * 'caption' - Short description of the message.
 * 'description' - Long description of the message.
 */
CMessage::CMessage(int id, CUniString caption, CUniString description)
{
	CMessage(id, LEVEL_INFO, caption, description);
}

/*
 * Constructor for a message with caption.
 *
 * 'id' - Unique ID for the type of message.
 * 'level' - Info, warning or error (see LEVEL_... constants)
 * 'caption' - Short description of the message.
 */
CMessage::CMessage(int id, int level, CUniString caption)
{
	CMessage(id, level, caption, L"");
}

/*
 * Constructor for a message with caption and description.
 *
 * 'id' - Unique ID for the type of message.
 * 'level' - Info, warning or error (see LEVEL_... constants)
 * 'caption' - Short description of the message.
 * 'description' - Long description of the message.
 */
CMessage::CMessage(int id, int level, CUniString caption, CUniString description)
{
	m_Id = id;
	m_Level = level;
	m_Caption = caption;
	m_Description = description;
}

/*
 * Constructor for a message with caption, description and custom field.
 *
 * 'id' - Unique ID for the type of message.
 * 'level' - Info, warning or error (see LEVEL_... constants)
 * 'caption' - Short description of the message.
 * 'description' - Long description of the message.
 * 'custom' - Generic text field which can be used for references to layout object, for instance
 */
CMessage::CMessage(int id, int level, CUniString caption, CUniString description, CUniString custom)
{
	m_Id = id;
	m_Level = level;
	m_Caption = caption;
	m_Description = description;
	m_Custom = custom;
}

/*
 * Destructor
 */
CMessage::~CMessage(void)
{
}

/*
 * Creates a deep copy
 */
CMessage * CMessage::Clone()
{
	CMessage * copy = new CMessage(m_Id, m_Level, m_Caption, m_Description, m_Custom);
	return copy;
}

/*
 * Class CMessages
 *
 * List of messages
 *
 * CC 17.10.2012 - created
 */

/*
 * Constructor
 */
CMessages::CMessages()
{
}

/*
 * Destructor
 */
CMessages::~CMessages()
{
	Clear();
}

/*
 * Deletes all messages and clears this list
 */
void CMessages::Clear()
{
	for (unsigned int i=0; i<m_List.size(); i++)
		delete m_List.at(i);
	m_List.clear();
}

/*
 * Removes the message at the given position
 */
CMessage * CMessages::RemoveMessage(int index)
{
	CMessage * ret = m_List[index];
	m_List.erase(m_List.begin() + index);
	return ret;
}

/*
 * Checks if there are any messages with level 'Warning'
 */
bool CMessages::HasWarnings()
{
	return HasLevel(CMessage::LEVEL_WARNING);
}

/*
 * Checks if there are any messages with level 'Error'
 */
bool CMessages::HasErrors()
{
	return HasLevel(CMessage::LEVEL_ERROR);
}

/*
 * Checks if this list contains a message with the given ID
 */
bool CMessages::HasMessage(int id)
{
	for (unsigned int i=0; i<m_List.size(); i++)
		if (m_List.at(i)->GetId() == id)
			return true;
	return false;
}

/*
 * Checks if there are any messages with given level (e.g. 'LEVEL_WARNING')
 */
bool CMessages::HasLevel(int level)
{
	return m_Levels.find(level) != m_Levels.end();
}

/*
 * Returns the number of messages with level 'Warning'
 */
int CMessages::GetNumberOfWarnings()
{
	return GetNumberOfMessages(CMessage::LEVEL_WARNING);
}

/*
 * Returns the number of messages with level 'Error'
 */
int CMessages::GetNumberOfErrors()
{
	return GetNumberOfMessages(CMessage::LEVEL_ERROR);
}

/*
 * Returns the number of messages with the given level (e.g. 'LEVEL_WARNING')
 */
int CMessages::GetNumberOfMessages(int level)
{
	int count = 0;
	for (unsigned int i=0; i<m_List.size(); i++)
		if (m_List.at(i)->GetLevel() == level)
			count++;
	return count;
}

/*
 * Adds the given message to this list (transfers ownership to this list)
 */
void CMessages::AddMessage(CMessage * message) 
{ 
	if (message == NULL)
		return;
	m_List.push_back(message);
	m_Levels.insert(message->GetLevel());
}

/*
 * Creates a text containing all messages
 *
 * 'level' - Filter for level (optional)
 */
CUniString CMessages::ComposeText(int level /*= 0*/, bool extraLinebreakAfterCaption /*= true*/)
{
	CUniString body;
	int counter = 0;
	for (unsigned int i=0; i<m_List.size(); i++)
	{
		CMessage * message = m_List.at(i);
		if (message != NULL && (level == 0 || level == message->GetLevel()))
		{
			counter++;
			if (!body.IsEmpty())
				body.Append(L"\r\n\r\n");

			//Show message number if more than one message
			if (m_List.size() > 1)
			{
				body.Append(L"(");
				body.Append(counter);
				body.Append(L") ");
			}

			if (message->GetLevel() == CMessage::LEVEL_INFO)
				body.Append(L"INFO: ");
			else if (message->GetLevel() == CMessage::LEVEL_WARNING)
				body.Append(L"WARNING: ");
			else if (message->GetLevel() == CMessage::LEVEL_ERROR)
				body.Append(L"ERROR: ");

			if (!message->GetCaption().IsEmpty())
				body.Append(message->GetCaption());
			body.Append(L"\r\n");
			if (extraLinebreakAfterCaption)
				body.Append(L"\r\n");
			if (!message->GetDescription().IsEmpty())
				body.Append(message->GetDescription());
		}
	}

	if (counter == 0) //No message
		return CUniString();

	CUniString text;
	if (counter > 1)
	{
		text.Append(counter);
		text.Append(" messages:\n\n");
	}
	text.Append(body);
	return text;
}


} // end namespace