#include "StdAfx.h"
#include "Parameter.h"
#include <algorithm>
#include <cmath>
#include "ini.h"

namespace PRImA
{


/*
 * Class CParameter
 *
 * Base class for algorithm parameters.
 *
 * CC 18.11.2009 - created
 */

/*
 * Constructor
 */
CParameter::CParameter(int id, CUniString name, CUniString description)
{
	m_Type = 0;
	Init(id, name, description);
}

/*
 * Destructor
 */
CParameter::~CParameter(void)
{
}

/*
 * Initialises all fields
 */
void CParameter::Init(int id, CUniString name, CUniString description)
{
	m_Name = CUniString(name);
	m_Description = CUniString(description);
	m_IsSet = false;
	m_SortIndex = -1;
	m_ReadOnly = false;
	m_Version = 0;
	m_Id = id;
	m_Visible = true;
}

/*
 * Compares the position of the given parameters using their sort indexes.
 * Returns 1 if p1 < p2; 0 otherwise
 */
int	CParameter::Compare(CParameter * p1, CParameter * p2)
{
	return p1->m_SortIndex < p2->m_SortIndex;
}

/*
 * Calls the OnChange method of all registerd listeners.
 */
void CParameter::NotifyChangeListeners()
{
	for (unsigned int i=0; i<m_ChangeListeners.size(); i++)
		m_ChangeListeners[i]->OnParameterChange();
}

/*
 * Generic set value method. Copies the value of the source paramter to this parameter.
 * Supported types: BOOL, INT, DOUBLE, STRING
 */
void CParameter::SetValue(CParameter * source)
{
	CParameter * target = this;
	if (target->GetType() == TYPE_MULTI)
	{
		if (source->GetType() == TYPE_BOOL)
			((CMultiParameter*)target)->SetValue(((CBoolParameter*)source)->GetValue());
		else if (source->GetType() == TYPE_INT)
			((CMultiParameter*)target)->SetValue(((CIntParameter*)source)->GetValue());
		else if (source->GetType() == TYPE_DOUBLE)
			((CMultiParameter*)target)->SetValue(((CDoubleParameter*)source)->GetValue());
		else if (source->GetType() == TYPE_STRING)
			((CMultiParameter*)target)->SetValue(((CStringParameter*)source)->GetValue());
	}
	else
	{
		if (source->GetType() != this->GetType())
			return;
		if (source->GetType() == TYPE_BOOL)
			((CBoolParameter*)target)->SetValue(((CBoolParameter*)source)->GetValue(false));
		else if (source->GetType() == TYPE_INT)
			((CIntParameter*)target)->SetValue(((CIntParameter*)source)->GetValue(false));
		else if (source->GetType() == TYPE_DOUBLE)
			((CDoubleParameter*)target)->SetValue(((CDoubleParameter*)source)->GetValue(false));
		else if (source->GetType() == TYPE_STRING)
			((CStringParameter*)target)->SetValue(((CStringParameter*)source)->GetValue(false));
	}
}

/*
 * Copies all fiels of this parameter to the given one (used for Clone()).
 */
void CParameter::CopyTo(CParameter * target)
{
	target->m_Description = m_Description;
	target->m_Id = m_Id;
	target->m_IsSet = m_IsSet;
	target->m_Name = m_Name;
	target->m_ReadOnly = m_ReadOnly;
	target->m_SortIndex = m_SortIndex;
	target->m_Type = m_Type;
	target->m_Version = m_Version;
	target->m_Visible = m_Visible;
}


/*
 * Class CDoubleParameter
 *
 * Parameter holding a double value.
 *
 * CC 19.11.2009 - created
 */

/*
 * Construtor (creates empty parameter)
 */
CDoubleParameter::CDoubleParameter()
{
	Init(0, "", "", 0, false, 0, 0, 0);
}

/*
 * Constructor without ID (0 will be used as ID)
 */
CDoubleParameter::CDoubleParameter(CUniString name, CUniString description,
								   double initialValue, double min, double max, double step)
{
	Init(0, name, description, initialValue,  min, max, step);
}

/*
 * Constructor with ID
 */
CDoubleParameter::CDoubleParameter(int id, CUniString name, CUniString description,
								   double initialValue, double min, double max, double step)
{
	Init(id, name, description, initialValue,  min, max, step);
}

/*
 * Constructor with ID
 */
CDoubleParameter::CDoubleParameter(int id, CUniString name, CUniString description,
								   double initialValue, bool isSet, double min, double max, double step)
{
	Init(id, name, description, initialValue, isSet, min, max, step);
}

/*
 * Initialises all fields
 */
void CDoubleParameter::Init(CUniString name, CUniString description,
							double initialValue, double min, double max, double step)
{
	Init(0, name, description, initialValue,  min, max, step);
}

/*
 * Initialises all fields
 */
void CDoubleParameter::Init(CUniString name, CUniString description,
							double initialValue, bool isSet, double min, double max, double step)
{
	Init(0, name, description, initialValue, isSet, min, max, step);
}

/*
 * Initialises all fields
 */
void CDoubleParameter::Init(int id, CUniString name, CUniString description,
							double initialValue, double min, double max, double step)
{
	Init(id, name, description, initialValue, true, min, max, step);
}

/*
 * Initialises all fields
 */
void CDoubleParameter::Init(int id, CUniString name, CUniString description,
						 double initialValue, bool isSet, double min, double max, double step)
{
	CParameter::Init(id, name, description);
	m_Value = initialValue;
	m_IsSet = isSet;
	m_Max = max;
	m_Min = min;
	m_Step = step;
	SetType(TYPE_DOUBLE);
}

/*
 * Creates a deep copy
 */
CParameter * CDoubleParameter::Clone()
{
	CParameter * ret = new CDoubleParameter();
	this->CopyTo(ret);
	return ret;
}

/*
 * Copies all contents of this parameter to the given target parameter.
 */
void CDoubleParameter::CopyTo(CParameter * target)
{
	CParameter::CopyTo(target);
	((CDoubleParameter*)target)->m_Min = m_Min;
	((CDoubleParameter*)target)->m_Max = m_Max;
	((CDoubleParameter*)target)->m_Step = m_Step;
	target->SetValue(this);
	target->SetSet(this->IsSet());
}

/*
 * Returns the value of this parameter. 
 * Throws an exception if the value is undefined ('not set').
 * 'throwExceptionIfValueNotSet' - Set to true to override the exception and return the last value
 */
double CDoubleParameter::GetValue(bool throwExceptionIfValueNotSet /*= true*/)
{
	if (!m_IsSet && throwExceptionIfValueNotSet)
	{
		CUniString msg(L"Value of parameter not defined: ");
		msg.Append(this->m_Name);
		throw CBaseException(msg);
	}
	return m_Value;
}

/*
 * Changes the value of this parameter. Cuts of at min/max if a range has been defined.
 * 'notifyListeners' - Set to true to notify all parameter change listeners
 */
void CDoubleParameter::SetValue(double value, bool notifyListeners) 
{ 
	m_Value = value; 
	m_IsSet = true;
	if (m_Max == 0.0 && m_Min == 0.0) //no limits
		return;
	if (m_Value > m_Max) 
		m_Value = m_Max; 
	else if (m_Value < m_Min) 
		m_Value = m_Min;
	if (notifyListeners)
		NotifyChangeListeners();
}

/*
 * Returns true if min and max have been defined
 */
bool CDoubleParameter::HasBoundaries()
{
	return (m_Min != 0.0 || m_Max != 0.0);
}

/*
 * Returns a formated message containing name and value of this parameter
 */
CUniString CDoubleParameter::GetLogMessage()
{
	CUniString msg(m_Name);
	msg.Append(_T("="));
	msg.Append(m_Value);
	return msg;
}


/*
 * Class CIntParameter
 *
 * Parameter holding a int value.
 *
 * CC 30.11.2009 - created
 */

/*
 * Constructor (creates empty parameter)
 */
CIntParameter::CIntParameter()
{
	Init(0, L"", L"", 0, false, 0, 0, 0);
}

/*
 * Constructor without ID (0 will be used as ID)
 */
CIntParameter::CIntParameter(CUniString name, CUniString description,
							 int initialValue, int min, int max, int step)
{
	Init(0, name, description, initialValue, min, max, step);
}

/*
 * Constructor with ID
 */
CIntParameter::CIntParameter(int id, CUniString name, CUniString description,
							 int initialValue, int min, int max, int step)
{
	Init(id, name, description, initialValue, min, max, step);
}

/*
 * Constructor with ID
 */
CIntParameter::CIntParameter(int id, CUniString name, CUniString description,
							 int initialValue, bool isSet, int min, int max, int step)
{
	Init(id, name, description, initialValue, isSet, min, max, step);
}

/*
 * Initialises all fields
 */
void CIntParameter::Init(CUniString name, CUniString description,
							 int initialValue, int min, int max, int step)
{
	Init(0, name, description, initialValue, min, max, step);
}

/*
 * Initialises all fields
 */
void CIntParameter::Init(CUniString name, CUniString description,
							 int initialValue, bool isSet, int min, int max, int step)
{
	Init(0, name, description, initialValue, isSet, min, max, step);
}

/*
 * Initialises all fields
 */
void CIntParameter::Init(int id, CUniString name, CUniString description,
							 int initialValue, int min, int max, int step)
{
	Init(id, name, description, initialValue, true, min, max, step);
}

/*
 * Initialises all fields
 */
void CIntParameter::Init(int id, CUniString name, CUniString description,
					 int initialValue, bool isSet, int min, int max, int step)
{
	CParameter::Init(id, name, description);
	m_Value = initialValue;
	m_IsSet = isSet;
	m_Max = max;
	m_Min = min;
	m_Step = step;
	SetType(TYPE_INT);
}

/*
 * Creates a deep copy
 */
CParameter * CIntParameter::Clone()
{
	CParameter * ret = new CIntParameter();
	this->CopyTo(ret);
	return ret;
}

/*
 * Copies all contents of this parameter to the given target parameter.
 */
void CIntParameter::CopyTo(CParameter * target)
{
	CParameter::CopyTo(target);
	((CIntParameter*)target)->m_Min = m_Min;
	((CIntParameter*)target)->m_Max = m_Max;
	((CIntParameter*)target)->m_Step = m_Step;
	target->SetValue(this);
	target->SetSet(this->IsSet());
}

/*
 * Returns the value of this parameter. 
 * Throws an exception if the value is undefined ('not set').
 * 'throwExceptionIfValueNotSet' - Set to true to override the exception and return the last value
 */
int CIntParameter::GetValue(bool throwExceptionIfValueNotSet /*= true*/)
{
	if (!m_IsSet && throwExceptionIfValueNotSet)
		throw CBaseException(L"Value of paremeter not defined");
	return m_Value;
}

/*
 * Changes the value of this parameter. Cuts of at min/max if a range has been defined.
 * 'notifyListeners' - Set to true to notify all parameter change listeners
 */
void CIntParameter::SetValue(int value, bool notifyListeners) 
{ 
	m_Value = value; 
	m_IsSet = true;
	if (m_Max == 0 && m_Min == 0) //no limits
		return;
	if (m_Value > m_Max) 
		m_Value = m_Max; 
	else if (m_Value < m_Min) 
		m_Value = m_Min;
	if (notifyListeners)
		NotifyChangeListeners();
}

/*
 * Returns true if min and max have been defined
 */
bool CIntParameter::HasBoundaries()
{
	return (m_Min != 0 || m_Max != 0);
}

/*
 * Returns a formated message containing name and value of this parameter
 */
CUniString CIntParameter::GetLogMessage()
{
	CUniString msg(m_Name);
	msg.Append(_T("="));
	msg.Append(m_Value);
	return msg;
}


/*
 * Class CBoolParameter
 *
 * Parameter holding a boolean value.
 *
 * CC 23.11.2009 - created
 */

/*
 * Constructor (creates empty parameter)
 */
CBoolParameter::CBoolParameter()
{
	Init(0, L"", L"", false, false);
}

/*
 * Constructor without ID (0 will be used as ID)
 */
CBoolParameter::CBoolParameter(CUniString name, CUniString description,
								 bool initialValue)
{
	Init(0, name, description, initialValue);
}

/*
 * Constructor with ID
 */
CBoolParameter::CBoolParameter(int id, CUniString name, CUniString description,
								 bool initialValue)
{
	Init(id, name, description, initialValue);
}

/*
 * Constructor with ID
 */
CBoolParameter::CBoolParameter(int id, CUniString name, CUniString description,
								 bool initialValue, bool isSet)
{
	Init(id, name, description, initialValue, isSet);
}

/*
 * Initialises all fields
 */
void CBoolParameter::Init(CUniString name, CUniString description,
						  bool initialValue) 
{
	Init(0, name, description, initialValue);
}

/*
 * Initialises all fields
 */
void CBoolParameter::Init(CUniString name, CUniString description,
						  bool initialValue, bool isSet) 
{
	Init(0, name, description, initialValue, isSet);
}

/*
 * Initialises all fields
 */
void CBoolParameter::Init(int id, CUniString name, CUniString description,
						  bool initialValue) 
{
	Init(id, name, description, initialValue, true);
}

/*
 * Initialises all fields
 */
void CBoolParameter::Init(int id, CUniString name, CUniString description,
			  bool initialValue, bool isSet)
{
	CParameter::Init(id, name, description);
	m_Value = initialValue;
	m_IsSet = isSet;
	SetType(TYPE_BOOL);
}

/*
 * Creates a deep copy
 */
CParameter * CBoolParameter::Clone()
{
	CParameter * ret = new CBoolParameter();
	this->CopyTo(ret);
	return ret;
}

/*
 * Copies all contents of this parameter to the given target parameter.
 */
void CBoolParameter::CopyTo(CParameter * target)
{
	CParameter::CopyTo(target);
	target->SetValue(this);
	target->SetSet(this->IsSet());
}

/*
 * Returns the value of this parameter. 
 * Throws an exception if the value is undefined ('not set').
 * 'throwExceptionIfValueNotSet' - Set to true to override the exception and return the last value
 */
bool CBoolParameter::GetValue(bool throwExceptionIfValueNotSet /*= true*/)
{
	if (!m_IsSet && throwExceptionIfValueNotSet)
		throw CBaseException(L"Value of paremeter not defined");
	return m_Value;
}

/*
 * Changes the value of this parameter. 
 * 'notifyListeners' - Set to true to notify all parameter change listeners
 */
void CBoolParameter::SetValue(bool value, bool notifyListeners) 
{ 
	m_Value = value; 
	m_IsSet = true;
	if (notifyListeners)
		NotifyChangeListeners();
}

/*
 * Returns a formated message containing name and value of this parameter
 */
CUniString CBoolParameter::GetLogMessage()
{
	CUniString msg(m_Name);
	msg.Append(_T("="));
	msg.Append(m_Value ? _T("true") : _T("false"));
	return msg;
}


/*
 * Class CStringParameter
 *
 * Parameter holding a string value.
 *
 * CC 14.12.2009 - created
 */

/*
 * Constructor (creates empty parameter)
 */
CStringParameter::CStringParameter()
{
	Init(0, "", "", CUniString(), false);
}

/*
 * Constructor without ID (0 will be used as ID)
 */
CStringParameter::CStringParameter(CUniString name, CUniString description,
								 CUniString initialValue)
{
	Init(0, name, description, initialValue);
}

/*
 * Constructor with ID
 */
CStringParameter::CStringParameter(int id, CUniString name, CUniString description,
								 CUniString initialValue)
{
	Init(id, name, description, initialValue);
}

/*
 * Constructor with ID
 */
CStringParameter::CStringParameter(int id, CUniString name, CUniString description,
								 CUniString initialValue, bool isSet)
{
	Init(id, name, description, initialValue, isSet);
}

/*
 * Initialises all fields
 */
void CStringParameter::Init(CUniString name, CUniString description,
						  CUniString initialValue) 
{
	Init(0, name, description, initialValue);
}

/*
 * Initialises all fields
 */
void CStringParameter::Init(CUniString name, CUniString description,
						  CUniString initialValue, bool isSet) 
{
	Init(0, name, description, initialValue, isSet);
}

/*
 * Initialises all fields
 */
void CStringParameter::Init(CUniString name, CUniString description) 
{
	Init(0, name, description);
}

/*
 * Initialises all fields
 */
void CStringParameter::Init(int id, CUniString name, CUniString description,
						  CUniString initialValue) 
{
	Init(id, name, description, initialValue, true);
}

/*
 * Initialises all fields
 */
void CStringParameter::Init(int id, CUniString name, CUniString description,
			  CUniString initialValue, bool isSet)
{
	CParameter::Init(id, name, description);
	m_Value = initialValue;
	m_IsSet = isSet;
	SetType(TYPE_STRING);
	m_TextType = TEXTTYPE_MULTILINE;
}

/*
 * Initialises all fields
 */
void CStringParameter::Init(int id, CUniString name, CUniString description) 
{
	CParameter::Init(id, name, description);
	SetType(TYPE_STRING);
	m_TextType = TEXTTYPE_MULTILINE;
}

/*
 * Creates a deep copy
 */
CParameter * CStringParameter::Clone()
{
	CParameter * ret = new CStringParameter();
	this->CopyTo(ret);
	return ret;
}

/*
 * Copies all contents of this parameter to the given target parameter.
 */
void CStringParameter::CopyTo(CParameter * target)
{
	CParameter::CopyTo(target);
	target->SetValue(this);
	((CStringParameter*)target)->m_TextType = m_TextType;
	target->SetSet(this->IsSet());
	set<CUniString> * targetValidValues = ((CStringParameter*)target)->GetValidValues();
	set<CUniString>::iterator it = m_ValidValues.begin();
	while (it != m_ValidValues.end())
	{
		targetValidValues->insert((*it));
		it++;
	}
}

/*
 * Returns the value of this parameter. 
 * Throws an exception if the value is undefined ('not set').
 * 'throwExceptionIfValueNotSet' - Set to true to override the exception and return the last value
 */
CUniString CStringParameter::GetValue(bool throwExceptionIfValueNotSet /*= true*/)
{
	if (!m_IsSet && throwExceptionIfValueNotSet)
		throw CBaseException(L"Value of paremeter not defined");
	return m_Value;
}

/*
 * Changes the value of this parameter. 
 * 'notifyListeners' - Set to true to notify all parameter change listeners
 */
void CStringParameter::SetValue(CUniString value, bool notifyListeners) 
{ 
	m_Value = value; 
	m_IsSet = true;
	if (notifyListeners)
		NotifyChangeListeners();
}

/*
 * Returns a formated message containing name and value of this parameter
 */
CUniString CStringParameter::GetLogMessage()
{
	CUniString msg(m_Name);
	msg.Append(_T("="));
	msg.Append(m_Value);
	return msg;
}


/*
 * Class CIdParameter
 *
 * Extended string parameter with id registration.
 *
 * CC 19.02.2010 - created
 */

CIdParameter::CIdParameter()
{
	Init(0, L"", L"", CUniString(), false, NULL);
}

CIdParameter::CIdParameter(CUniString name, CUniString description,
							CUniString initialValue, CIdRegister<void *> * idRegister)
{
	Init(0, name, description, initialValue, idRegister);
}

CIdParameter::CIdParameter(int id, CUniString name, CUniString description,
							CUniString initialValue, CIdRegister<void *> * idRegister)
{
	Init(id, name, description, initialValue, idRegister);
}

void CIdParameter::Init(CUniString name, CUniString description,
			  CUniString initialValue, CIdRegister<void *> * idRegister)
{
	Init(0, name, description, initialValue, idRegister);
}

void CIdParameter::Init(CUniString name, CUniString description,
				CIdRegister<void *> * idRegister)
{
	Init(0, name, description, idRegister);
}

void CIdParameter::Init(int id, CUniString name, CUniString description,
			  CUniString initialValue, CIdRegister<void *> * idRegister)
{
	Init(id, name, description, initialValue, true, idRegister);
}

void CIdParameter::Init(int id, CUniString name, CUniString description,
			  CUniString initialValue, bool isSet, CIdRegister<void *> * idRegister)
{
	m_IdRegister = idRegister;
	if (m_IdRegister != NULL && !initialValue.IsEmpty())
	{
		initialValue = m_IdRegister->RegisterId(initialValue, NULL);
		isSet = false;
	}
	CStringParameter::Init(id, name, description, initialValue, isSet);
	m_TextType = TEXTTYPE_SINGLELINE;
}


void CIdParameter::Init(int id, CUniString name, CUniString description,
				CIdRegister<void *> * idRegister)
{
	m_IdRegister = idRegister;
	CStringParameter::Init(id, name, description);
	m_TextType = TEXTTYPE_SINGLELINE;
}

CParameter * CIdParameter::Clone()
{
	CParameter * ret = new CIdParameter();
	this->CopyTo(ret);
	return ret;
}

/*
 * Copies all contents of this parameter to the given target parameter.
 */
void CIdParameter::CopyTo(CParameter * target)
{
	CParameter::CopyTo(target);
	//((CIdParameter*)target)->m_IdRegister = this->m_IdRegister; //CC - the IdRegister should not be copied
	target->SetValue(this);
	((CIdParameter*)target)->m_TextType = m_TextType;
	target->SetSet(this->IsSet());
}

void CIdParameter::SetValue(CUniString value, bool notifyListeners) 
{ 
	if (m_IdRegister != NULL && value != m_Value)
	{
		if (!m_Value.IsEmpty())
			m_IdRegister->UnregisterId(m_Value);
		if (!value.IsEmpty())
			value = m_IdRegister->RegisterId(value, NULL);
		CStringParameter::SetValue(value, notifyListeners);
	}
}


/*
 * Class CParameterMap
 *
 * Associative array of parameters. Parameters can be retrieved index, ID or name.
 *
 * CC 14.12.2009 - created
 */

/*
 * Constructor
 */
CParameterMap::CParameterMap()
{
	m_PropIdTypeMap = NULL;
	m_Sorted = false;
}

/*
 * Destructor
 */
CParameterMap::~CParameterMap()
{
}

/*
 * Clears the map and resets name, type, and sort state.
 */
void CParameterMap::Reset()
{
	Clear();
	m_Sorted = false;
	m_SchemaType = L"";
	m_Name = L"";
}

/*
 * Returns the parameter having the specified name or NULL if no such parameter is found.
 */
CParameter * CParameterMap::Get(CUniString name)
{
	for (unsigned int i=0; i<m_Params.size(); i++)
	{
		if (CUniString(m_Params[i]->GetName()).Compare(name) == 0)
		{
			return m_Params[i];
		}
	}
	return NULL;
}

/*
 * Returns the paramter with the specified ID or NULL if no such parameter can be found.
 */
CParameter * CParameterMap::GetParamForId(int id)
{
	map<int,CParameter*>::iterator it = m_IdMap.find(id);
	if (it != m_IdMap.end())
		return (*it).second;
	return NULL;
}

/*
 * Add the given parameter to the map and re-sorts the map (if sorting enabled).
 * Note: Ownership is not transferred to the map (the parameter will not be deleted at destruction).
 *       Use DeleteAll() if required.
 */
void CParameterMap::Add(CParameter * param) 
{ 
	if (param == NULL)
		return;
	m_Params.push_back(param); 
	m_IdMap.insert(pair<int,CParameter*>(param->GetId(), param));
	if (param->GetSortIndex() < 0)
		param->SetSortIndex((int)m_Params.size());
	if (m_Sorted) 
		Sort(); 
}

/*
 * Add the given parameter to the map at the specified index and re-sorts the map (if sorting enabled).
 * Note: Ownership is not transferred to the map (the parameter will not be deleted at destruction).
 *       Use DeleteAll() if required.
 */
void CParameterMap::Insert(CParameter * param, int index)
{
	if (param == NULL)
		return;
	m_Params.insert(m_Params.begin()+index, param);
	m_IdMap.insert(pair<int, CParameter*>(param->GetId(), param));
	if (param->GetSortIndex() < 0)
		param->SetSortIndex((int)m_Params.size());
	if (m_Sorted)
		Sort();
}

/*
 * Removes all parameters, but keeps all other settings.
 */
void CParameterMap::Clear() 
{ 
	m_Params.clear(); 
	m_IdMap.clear();
}

/*
 * Returns the schema type of the param with the given ID.
 */
CUniString CParameterMap::GetSchemaType(int paramId /*= 0*/)
{
	//If a type was explicit set, return this
	if (!m_SchemaType.IsEmpty())
		return m_SchemaType;
	//Otherwise have a look in the id-type map
	if (m_PropIdTypeMap != NULL && paramId != 0)
	{
		return m_PropIdTypeMap->GetType(paramId);
	}
	return CUniString();
}

/*
 * Sorts this map using the parameters' sort indexes
 */
void CParameterMap::Sort()
{
	sort(m_Params.begin(), m_Params.end(), CParameter::Compare);
}

/*
 * Copies the values of the params of the given map to the 
 * params with the same ID of this map.
 */
void CParameterMap::CopyValues(CParameterMap * params)
{
	if (params == NULL)
		return;
	for (int i=0; i<params->GetSize(); i++)
	{
		CParameter * source = params->Get(i);
		if (source->GetId() != 0) //has valid ID?
		{
			CParameter * target = this->GetParamForId(source->GetId());
			Copy(source, target);
		}
	}
}

/*
 * Copies the values of the params of the given map to the 
 * params with the same name of this map.
 */
void CParameterMap::CopyValuesByName(CParameterMap * params)
{
	if (params == NULL)
		return;
	for (int i=0; i<params->GetSize(); i++)
	{
		CParameter * source = params->Get(i);
		if (!source->GetName().IsEmpty()) //has valid name?
		{
			CParameter * target = this->Get(source->GetName());
			Copy(source, target);
		}
	}
}

/*
 * Copies value, min, max, step, isSet from source to target parameter.
 */
void CParameterMap::Copy(CParameter * source, CParameter * target)
{
	if (target != NULL)
	{
		//Min, max, step
		if (source->GetType() == CParameter::TYPE_INT)
		{
			CIntParameter * intSource = (CIntParameter *)source;
			CIntParameter * intTarget = (CIntParameter *)target;
			intTarget->SetMin(intSource->GetMin());
			intTarget->SetMax(intSource->GetMax());
			intTarget->SetStep(intSource->GetStep());
		}
		if (source->GetType() == CParameter::TYPE_DOUBLE)
		{
			CDoubleParameter * dblSource = (CDoubleParameter *)source;
			CDoubleParameter * dblTarget = (CDoubleParameter *)target;
			dblTarget->SetMin(dblSource->GetMin());
			dblTarget->SetMax(dblSource->GetMax());
			dblTarget->SetStep(dblSource->GetStep());
		}
		target->SetValue(source);
		target->SetSet(source->IsSet());
	}
}

/*
 * Merges this parameter map with the given one.
 * Non existing parameters will be added.
 * Existing parameters will be replaced if they are older (version) (only works for parameters with IDs).
 * 'overwrite' - If true, existing parameters will always be overwritte.
 *               If false, existing parameters will only be overwritten, if their version is older.
 * 'parameterRangeRestriction...' - Restricts the merge operation to paramters with IDs in the specified range
 */
void CParameterMap::MergeWith(CParameterMap * params, bool overwrite /*= false*/, int parameterRangeRestrictionStart /* = 0 */, int parameterRangeRestrictionEnd  /* = MAXINT */)
{
	for (int i=0; i<params->GetSize(); i++)
	{
		CParameter * source = params->Get(i);

		if (source->GetId() == 0) //No ID, just add
			Add(source->Clone());
		else if (source->GetId() >= parameterRangeRestrictionStart && source->GetId() <= parameterRangeRestrictionEnd)  //has valid ID?
		{
			CParameter * target = this->GetParamForId(source->GetId());
			if (target == NULL)
			{
				Add(source->Clone());
			}
			else //Parameter already exists -> overwrite?
			{
				if (overwrite || source->GetVersion() > target->GetVersion())
				{
					//Min, max, step
					if (source->GetType() == CParameter::TYPE_INT)
					{
						CIntParameter * intSource = (CIntParameter *)source;
						CIntParameter * intTarget = (CIntParameter *)target;
						intTarget->SetMin(intSource->GetMin());
						intTarget->SetMax(intSource->GetMax());
						intTarget->SetStep(intSource->GetStep());
					}
					if (source->GetType() == CParameter::TYPE_DOUBLE)
					{
						CDoubleParameter * dblSource = (CDoubleParameter *)source;
						CDoubleParameter * dblTarget = (CDoubleParameter *)target;
						dblTarget->SetMin(dblSource->GetMin());
						dblTarget->SetMax(dblSource->GetMax());
						dblTarget->SetStep(dblSource->GetStep());
					}
					//Valid values
					if (source->GetType() == CParameter::TYPE_STRING)
					{
						CStringParameter * strSource = (CStringParameter*)source;
						CStringParameter * strTarget = (CStringParameter*)target;
						if (strSource->GetValidValues() != NULL && strTarget->GetValidValues() != NULL)
						{
							strTarget->GetValidValues()->clear();
							for (set<CUniString>::iterator it=strSource->GetValidValues()->begin(); it != strSource->GetValidValues()->end(); it++)
								strTarget->GetValidValues()->insert((*it));
						}
					}

					//Value
					target->SetValue(source);
					//Other fields
					target->SetSet(source->IsSet());
					target->SetVersion(source->GetVersion());
					target->SetName(source->GetName());
					target->SetDescription(source->GetDescription());
					target->SetSortIndex(source->GetSortIndex());
					target->SetVisible(source->IsVisible());
					target->SetReadOnly(source->IsReadOnly());
				}
			}
		}
	}
}

/*
 * Removes the given parameter from the map.
 */
void CParameterMap::Remove(CParameter * param, bool deleteParam /*= false*/)
{
	for (unsigned int i=0; i<m_Params.size(); i++)
	{
		if (m_Params[i] == param)
		{
			Remove(i, deleteParam);
			return;
		}
	}
}

/*
 * Removes parameter with the given index from the map.
 */
void CParameterMap::Remove(int index, bool deleteParam /*= false*/)
{
	//Remove from id map
	if (m_Params[index]->GetId() != NULL)
	{
		map<int,CParameter*>::iterator it = m_IdMap.find(m_Params[index]->GetId());
		if (it != m_IdMap.end())
			m_IdMap.erase(it);
	}
	//Remove from vector
	if (deleteParam)
		delete m_Params[index];
	m_Params.erase(m_Params.begin() + index);
}

/*
 * Removes the parameter with the given id from the map.
 */
void CParameterMap::RemoveParamWithId(int id, bool deleteParam /*= false*/)
{
	CParameter * p = GetParamForId(id);
	if (p != NULL)
		Remove(p, deleteParam);
}

/*
 * Deletes all parameters clears the map.
 */
void CParameterMap::DeleteAll()
{
	for (unsigned int i=0; i<m_Params.size(); i++)
		delete m_Params[i];
	Clear();
}

/*
 * Creates a deep copy
 */
CParameterMap * CParameterMap::Clone()
{
	CParameterMap * copy = new CParameterMap();

	copy->SetSorted(m_Sorted);
	copy->SetSchemaType(m_SchemaType);
	copy->SetName(m_Name);

	for (int i=0; i<(int)m_Params.size(); i++)
	{
		copy->Add(m_Params[i]->Clone());
	}
	return copy;
}

/*
 * Loads the parameters from an ini file. The parameter name has to correspond with the
 * keys in the ini file.
 */
void CParameterMap::LoadFromIni(CUniString filePath, CUniString section)
{
	CIniReader reader(filePath);
	CParameter * param;
	for (int i=0; i<(int)m_Params.size(); i++)
	{
		param = m_Params[i];
		CString val = reader.getKeyValue(param->GetName(), section);
		if (!val.IsEmpty())
		{
			if (param->GetType() == CParameter::TYPE_INT)
				((CIntParameter*)param)->SetValue(_tstoi(val));
			else if (param->GetType() == CParameter::TYPE_DOUBLE)
				((CDoubleParameter*)param)->SetValue(_tstof(val));
			else if (param->GetType() == CParameter::TYPE_BOOL)
				((CBoolParameter*)param)->SetValue(CUniString(val) == CUniString(_T("true")) || CUniString(val) == CUniString(_T("TRUE")) || CUniString(val) == CUniString(_T("1")));
			else if (param->GetType() == CParameter::TYPE_STRING)
				((CStringParameter*)param)->SetValue(CUniString(val));
		}
	}
}

/*
 * Saves the parameters to the specified ini file. The parameter names are 
 * used as the keys in the ini file.
 */
void CParameterMap::SaveToIni(CUniString filePath, CUniString section)
{
	CIniReader reader(filePath);
	CParameter * param;
	for (int i=0; i<(int)m_Params.size(); i++)
	{
		param = m_Params[i];
		CUniString val;
		if (param->IsSet())
		{
			if (param->GetType() == CParameter::TYPE_INT)
				val.Append(((CIntParameter*)param)->GetValue());
			else if (param->GetType() == CParameter::TYPE_DOUBLE)
				val.Append(((CDoubleParameter*)param)->GetValue());
			else if (param->GetType() == CParameter::TYPE_BOOL)
				val.Append(((CBoolParameter*)param)->GetValue() ? _T("true") : _T("false"));
			else if (param->GetType() == CParameter::TYPE_STRING)
				val.Append(((CStringParameter*)param)->GetValue());
		}
		reader.setKey(val, param->GetName(), section);
	}
}


/*
 * Class CMultiParameter
 *
 * Parameter that holds several child paramerer (of the same type).
 * Changing the parent parameter also changes als child parameters.
 *
 * CC 08.02.2010
 */

/*
 * Constructor without ID (0 will be used as ID)
 */
CMultiParameter::CMultiParameter(CUniString name, CUniString description)
{
	CParameter::Init(0, name, description);
	SetType(TYPE_MULTI);
	m_Subtype = TYPE_UNKNOWN;
}

/*
 * Constructor with ID
 */
CMultiParameter::CMultiParameter(int id, CUniString name, CUniString description)
{
	CParameter::Init(id, name, description);
	SetType(TYPE_MULTI);
	m_Subtype = TYPE_UNKNOWN;
}

/*
 * Creates a deep copy
 */
CParameter * CMultiParameter::Clone()
{
	CParameter * ret = new CMultiParameter();
	this->CopyTo(ret);
	return ret;
}

/*
 * Copies all contents of this parameter to the given target parameter.
 */
void CMultiParameter::CopyTo(CParameter * target)
{
	CParameter::CopyTo(target);
	//Clone and add children
	for (unsigned int i=0; i<m_Children.size(); i++)
	{
		((CMultiParameter*)target)->AddChild(m_Children[i]->Clone());
	}
	((CMultiParameter*)target)->m_Subtype = m_Subtype;
}

/*
 * Adds the given parameter to the child list.
 * If the parameter is the first child, its type is used to initialize
 * the subtype of this multi parameter. If it is not the first child
 * and the parameter types doesn't match, an exception is thrown.
 */
void CMultiParameter::AddChild(CParameter * child) 
{ 
	int childType = child->GetType();
	if (m_Subtype == TYPE_UNKNOWN)
	{
		if (childType != TYPE_MULTI)
			m_Subtype = childType;
		else //For TYPE_MULTI use the subtype of the child instead of the type
			m_Subtype = ((CMultiParameter*)child)->GetSubType();
	}
	else 
	{
		if (childType != TYPE_MULTI)
		{
			if (m_Subtype != childType)
				throw CBaseException(L"Paramer type of child doesn't match the sub type of the multi parameter.");
		}
		else //For TYPE_MULTI use the subtype of the child instead of the type
		{
			if (m_Subtype != ((CMultiParameter*)child)->GetSubType())
				throw CBaseException(L"Paramer type of child doesn't match the sub type of the multi parameter.");
		}
	}
	m_Children.push_back(child);
}

/*
 * If all children have the same value, one of the children is
 * returned to get the value from that. Otherwise NULL is returned.
 */
CParameter * CMultiParameter::GetValue()
{
	if (HaveChildrenMultipleValues())
		return NULL;
	if (m_Children[0]->GetType() == TYPE_MULTI) //for multi params call GetValue recursively
	{
		return ((CMultiParameter*)m_Children[0])->GetValue();
	}
	return m_Children[0];
}

/*
 * Checks if all child parameters have the same value.
 */
bool CMultiParameter::HaveChildrenMultipleValues()
{
	if (m_Children.empty())
		return true;

	bool multipleValues = false;
	unsigned int i;

	int type = m_Children[0]->GetType();
	bool multi = false;
	if (type == TYPE_MULTI)
	{
		//For multi param use subtype
		type = ((CMultiParameter*)m_Children[0])->GetSubType();
		multi = true;
	}

	//Boolean
	if (type == CParameter::TYPE_BOOL)
	{
		//Get the value of the first child
		bool lastValue;
		if (multi)
		{
			CParameter * lastValueParam =  ((CMultiParameter*)m_Children[0])->GetValue();
			if (lastValueParam == NULL) //The child has multiple values
				return true;
			lastValue = ((CBoolParameter*)lastValueParam)->GetValue();
		}
		else
			lastValue = ((CBoolParameter*)m_Children[0])->GetValue();
		//Compare the value against the values of all other children
		for (i=1; i<m_Children.size(); i++)
		{
			bool currValue;
			if (m_Children[i]->GetType() == TYPE_MULTI)
			{
				CParameter * currValueParam =  ((CMultiParameter*)m_Children[i])->GetValue();
				if (currValueParam == NULL) //The child has multiple values
					return true;
				currValue = ((CBoolParameter*)currValueParam)->GetValue();
			}
			else
				currValue = ((CBoolParameter*)m_Children[i])->GetValue();
			//Compare
			if (lastValue != currValue)
				return true;
		}
	}
	//Integer
	else if (type == CParameter::TYPE_INT)
	{
		//Get the value of the first child
		int lastValue;
		if (multi)
		{
			CParameter * lastValueParam =  ((CMultiParameter*)m_Children[0])->GetValue();
			if (lastValueParam == NULL) //The child has multiple values
				return true;
			lastValue = ((CIntParameter*)lastValueParam)->GetValue();
		}
		else
			lastValue = ((CIntParameter*)m_Children[0])->GetValue();
		//Compare the value against the values of all other children
		for (i=1; i<m_Children.size(); i++)
		{
			int currValue;
			if (m_Children[i]->GetType() == TYPE_MULTI)
			{
				CParameter * currValueParam =  ((CMultiParameter*)m_Children[i])->GetValue();
				if (currValueParam == NULL) //The child has multiple values
					return true;
				currValue = ((CIntParameter*)currValueParam)->GetValue();
			}
			else
				currValue = ((CIntParameter*)m_Children[i])->GetValue();
			//Compare
			if (lastValue != currValue)
				return true;
		}
	}
	//Double
	else if (type == CParameter::TYPE_DOUBLE)
	{
		//Get the value of the first child
		double lastValue;
		if (multi)
		{
			CParameter * lastValueParam =  ((CMultiParameter*)m_Children[0])->GetValue();
			if (lastValueParam == NULL) //The child has multiple values
				return true;
			lastValue = ((CDoubleParameter*)lastValueParam)->GetValue();
		}
		else
			lastValue = ((CDoubleParameter*)m_Children[0])->GetValue();
		//Compare the value against the values of all other children
		for (i=1; i<m_Children.size(); i++)
		{
			double currValue;
			if (m_Children[i]->GetType() == TYPE_MULTI)
			{
				CParameter * currValueParam =  ((CMultiParameter*)m_Children[i])->GetValue();
				if (currValueParam == NULL) //The child has multiple values
					return true;
				currValue = ((CDoubleParameter*)currValueParam)->GetValue();
			}
			else
				currValue = ((CDoubleParameter*)m_Children[i])->GetValue();
			//Compare
			if (abs(lastValue - currValue) > 0.0000001)
				return true;
		}
	}
	//String
	else if (type == CParameter::TYPE_STRING)
	{
		//Get the value of the first child
		CUniString lastValue;
		if (multi)
		{
			CParameter * lastValueParam =  ((CMultiParameter*)m_Children[0])->GetValue();
			if (lastValueParam == NULL) //The child has multiple values
				return true;
			lastValue = ((CStringParameter*)lastValueParam)->GetValue();
		}
		else
			lastValue = ((CStringParameter*)m_Children[0])->GetValue();
		//Compare the value against the values of all other children
		for (i=1; i<m_Children.size(); i++)
		{
			CUniString currValue;
			if (m_Children[i]->GetType() == TYPE_MULTI)
			{
				CParameter * currValueParam =  ((CMultiParameter*)m_Children[i])->GetValue();
				if (currValueParam == NULL) //The child has multiple values
					return true;
				currValue = ((CStringParameter*)currValueParam)->GetValue();
			}
			else
				currValue = ((CStringParameter*)m_Children[i])->GetValue();
			//Compare
			if (lastValue.Find(currValue) != 0)
				return true;
		}
	}
	return multipleValues;
}

/*
 * Changes the value of all children.
 * Throws an exception if the type of the children (subtype) is not TYPE_BOOL
 */
void CMultiParameter::SetValue(bool value, bool notifyListeners /*= true*/)
{
	if (m_Subtype != TYPE_BOOL)
		throw CBaseException(L"Wrong type in CMultiParameter::SetValue");
	for (unsigned int i=0; i<m_Children.size(); i++)
	{
		if (m_Children[i]->GetType() == TYPE_MULTI)
			((CMultiParameter*)m_Children[i])->SetValue(value);
		else
			((CBoolParameter*)m_Children[i])->SetValue(value);
	}
	if (notifyListeners)
		NotifyChangeListeners();
}

/*
 * Changes the value of all children.
 * Throws an exception if the type of the children (subtype) is not TYPE_INT
 */
void CMultiParameter::SetValue(int value, bool notifyListeners /*= true*/)
{
	if (m_Subtype != TYPE_INT)
		throw CBaseException(L"Wrong type in CMultiParameter::SetValue");
	for (unsigned int i=0; i<m_Children.size(); i++)
		((CIntParameter*)m_Children[i])->SetValue(value);
	if (notifyListeners)
		NotifyChangeListeners();
}

/*
 * Changes the value of all children.
 * Throws an exception if the type of the children (subtype) is not TYPE_DOUBLE
 */
void CMultiParameter::SetValue(double value, bool notifyListeners /*= true*/)
{
	if (m_Subtype != TYPE_DOUBLE)
		throw CBaseException(L"Wrong type in CMultiParameter::SetValue");
	for (unsigned int i=0; i<m_Children.size(); i++)
	{
		if (m_Children[i]->GetType() == TYPE_MULTI)
			((CMultiParameter*)m_Children[i])->SetValue(value);
		else
			((CDoubleParameter*)m_Children[i])->SetValue(value);
	}
	if (notifyListeners)
		NotifyChangeListeners();
}

/*
 * Changes the value of all children.
 * Throws an exception if the type of the children (subtype) is not TYPE_STRING
 */
void CMultiParameter::SetValue(CUniString value, bool notifyListeners /*= true*/)
{
	if (m_Subtype != TYPE_STRING)
		throw CBaseException(L"Wrong type in CMultiParameter::SetValue");
	for (unsigned int i=0; i<m_Children.size(); i++)
	{
		if (m_Children[i]->GetType() == TYPE_MULTI)
			((CMultiParameter*)m_Children[i])->SetValue(value);
		else
			((CStringParameter*)m_Children[i])->SetValue(value);
	}
	if (notifyListeners)
		NotifyChangeListeners();
}

/*
 * Checks if at least one of the child parameters is set
 */
bool CMultiParameter::IsSet()
{
	//Ask all children (recursive)
	for (unsigned int i=0; i<m_Children.size(); i++)
		if (m_Children[i]->IsSet())
			return true;

	return false;
}

/*
 * Sets all child parameters to the given 'is set' state
 */
void CMultiParameter::SetSet(bool isSet)
{
	//Set it for all children
	for (unsigned int i=0; i<m_Children.size(); i++)
		m_Children[i]->SetSet(isSet);
}

/*
 * Retunrs the first CParamter withing the multi param tree
 * that is not of type MULTI.
 */
CParameter * CMultiParameter::GetFirstLeaf()
{
	if (m_Children[0]->GetType() != TYPE_MULTI)
		return m_Children[0];
	return ((CMultiParameter*)m_Children[0])->GetFirstLeaf();
}

/*
 * Returns a formatted message with the parameter name and the value
 */
CUniString CMultiParameter::GetLogMessage()
{
	CUniString msg(m_Name);
	msg.Append(_T("="));
	CParameter * val = GetValue();
	if (val != NULL)
		msg.Append(val->GetLogMessage());
	else
		msg.Append(_T("..."));
	return msg;
}


/*
 * Class CParameterChangeListener
 *
 * Is notified when the SetValue of a parameter is called.
 *
 * CC 12.02.2010 - created
 */

/*
 * Destructor
 */
CParameterChangeListener::~CParameterChangeListener()
{
}


} //end namespace