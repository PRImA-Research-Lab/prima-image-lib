#pragma once

#include <vector>
#include <set>
#include "BaseException.h"
#include "ExtraString.h"
#include "IdRegister.h"

#ifndef DllExport
 #ifndef PRIMA_DLL_IMPORT
  #define DllExport __declspec( dllexport )
 #else
  #define DllExport __declspec( dllimport )
 #endif
#endif

using namespace std;

namespace PRImA 
{

class CParameterChangeListener;

/*
 * Class CParameter
 *
 * Base class for algorithm parameters.
 *
 * CC 18.11.2009 - created
 */

class DllExport CParameter
{
public:
	inline CParameter() {};
	CParameter(int id, CUniString name, CUniString description);
	virtual ~CParameter(void);

	void					Init(int id, CUniString name, CUniString description); //Init method (to be used if constructed with default contructor)
	inline int				GetType() { return m_Type; };
	inline CUniString		GetName() { return m_Name; };
	inline CUniString		GetDescription() { return m_Description; };
	inline int				GetSortIndex() { return m_SortIndex; };
	inline void				SetSortIndex(int index) { m_SortIndex = index; };
	inline int				GetVersion() { return m_Version; };
	inline int				GetId() { return m_Id; };
	inline bool				IsVisible() { return m_Visible; };

	inline void				SetId(int id) { m_Id = id; };
	inline void				SetDescription(const char * descr) { m_Description = CUniString(descr); };
	inline void				SetDescription(CUniString & descr) { m_Description = descr; };
	inline void				SetName(CUniString & name) { m_Name = name; };
	virtual inline bool		IsSet() { return m_IsSet; };
	virtual inline void		SetSet(bool isSet) { m_IsSet = isSet; };
	inline void				SetVersion(int version) { m_Version = version; };
	inline void				SetVisible(bool visible) { m_Visible = visible; };

	inline bool				IsReadOnly() { return m_ReadOnly; };	//Is an indicator for a GUI (SetValue is always allowed)
	inline void				SetReadOnly(bool ro) { m_ReadOnly = ro; };

	inline void				AddChangeListener(CParameterChangeListener * listener) { m_ChangeListeners.push_back(listener); };
	inline void				RemoveChangeListeners() { m_ChangeListeners.clear(); };

	static int				Compare(CParameter * p1, CParameter * p2);

	void					SetValue(CParameter * source);

	virtual CParameter	*	Clone() = 0;
	virtual void			CopyTo(CParameter * target);

	virtual CUniString		GetLogMessage() = 0;


	static const int TYPE_UNKNOWN	= 0;
	static const int TYPE_DOUBLE	= 1;
	static const int TYPE_BOOL		= 2;
	static const int TYPE_INT		= 3;
	static const int TYPE_STRING	= 4;
	static const int TYPE_MULTI		= 5;

protected:
	inline void		SetType(int type) { m_Type = type; };
	void			NotifyChangeListeners();

private:
	CUniString		m_Description;
	int				m_SortIndex;
	bool			m_ReadOnly;
	vector<CParameterChangeListener*> m_ChangeListeners;
	int				m_Version;
	bool			m_Visible;	//Hint for GUI

protected:
	CUniString		m_Name;
	bool			m_IsSet;
	int				m_Type;
	int				m_Id;
};


/*
 * Class CDoubleParameter
 *
 * Parameter holding a double value.
 *
 * CC 19.11.2009 - created
 */

class DllExport CDoubleParameter : public CParameter
{
public:
	CDoubleParameter();
	CDoubleParameter(CUniString name, CUniString description,
					 double initialValue, double min = 0.0, double max = 0.0, double step = 0.0);
	CDoubleParameter(int id, CUniString name, CUniString description,
					 double initialValue, double min = 0.0, double max = 0.0, double step = 0.0);
	CDoubleParameter(int id, CUniString name, CUniString description,
					 double initialValue, bool isSet, double min = 0.0, double max = 0.0, double step = 0.0);

	void			Init(CUniString name, CUniString description,
						 double initialValue, double min = 0.0, double max = 0.0, double step = 0.0); //Init method (to be used if constructed with default contructor)
	void			Init(CUniString name, CUniString description,
						 double initialValue, bool isSet, double min = 0.0, double max = 0.0, double step = 0.0); //Init method (to be used if constructed with default contructor)
	void			Init(int id, CUniString name, CUniString description,
						 double initialValue, double min = 0.0, double max = 0.0, double step = 0.0); //Init method (to be used if constructed with default contructor)
	void			Init(int id, CUniString name, CUniString description,
						 double initialValue, bool isSet, double min, double max, double step); 

	inline double	GetMin() { return m_Min; };
	inline double	GetMax() { return m_Max; };
	inline double	GetStep() { return m_Step; };
	double			GetValue(bool throwExceptionIfValueNotSet = true);

	void			SetValue(double value, bool notifyListeners); 
	inline void		SetValue(double value) { SetValue(value, true); };
	inline void		SetMin(double min) { m_Min = min; };
	inline void		SetMax(double max) { m_Max = max; };
	inline void		SetStep(double step) { m_Step = step; };

	bool			HasBoundaries();

	virtual CParameter	*	Clone();
	virtual void			CopyTo(CParameter * target);
	CUniString			GetLogMessage();

private:
	double		m_Value;
	double		m_Min;
	double		m_Max;
	double		m_Step;
};


/*
 * Class CIntParameter
 *
 * Parameter holding a int value.
 *
 * CC 30.11.2009 - created
 */

class DllExport CIntParameter : public CParameter
{
public:
	CIntParameter();
	CIntParameter(CUniString name, CUniString description,
				  int initialValue, int min = 0, int max = 0, int step = 0);
	CIntParameter(int id, CUniString name, CUniString description,
				  int initialValue, int min = 0, int max = 0, int step = 0);
	CIntParameter(int id, CUniString name, CUniString description,
				  int initialValue, bool isSet, int min = 0, int max = 0, int step = 0);

	void		Init(CUniString name, CUniString description,
					 int initialValue, int min = 0, int max = 0, int step = 0); //Init method (to be used if constructed with default contructor)
	void		Init(CUniString name, CUniString description,
					 int initialValue, bool isSet, int min = 0, int max = 0, int step = 0); //Init method (to be used if constructed with default contructor)
	void		Init(int id, CUniString name, CUniString description,
					 int initialValue, int min = 0, int max = 0, int step = 0); //Init method (to be used if constructed with default contructor)
	void		Init(int id, CUniString name, CUniString description,
					 int initialValue, bool isSet, int min, int max, int step); 

	inline int	GetMin() { return m_Min; };
	inline int	GetMax() { return m_Max; };
	inline int	GetStep() { return m_Step; };
	int			GetValue(bool throwExceptionIfValueNotSet = true);

	void			SetValue(int value, bool notifyListeners); 
	inline void		SetValue(int value) { SetValue(value, true); };
	inline void		SetMin(int min) { m_Min = min; };
	inline void		SetMax(int max) { m_Max = max; };
	inline void		SetStep(int step) { m_Step = step; };

	bool			HasBoundaries();

	virtual CParameter	*	Clone();
	virtual void			CopyTo(CParameter * target);
	CUniString			GetLogMessage();
	
private:
	int		m_Value;
	int		m_Min;
	int		m_Max;
	int		m_Step;
};


/*
 * Class CBoolParameter
 *
 * Parameter holding a boolean value.
 *
 * CC 23.11.2009 - created
 */

class DllExport CBoolParameter : public CParameter
{
public:
	CBoolParameter();
	CBoolParameter(CUniString name, CUniString description,
					 bool initialValue);
	CBoolParameter(int id, CUniString name, CUniString description,
					 bool initialValue);
	CBoolParameter(int id, CUniString name, CUniString description,
					 bool initialValue, bool isSet);

	void Init(CUniString name, CUniString description,
			  bool initialValue); //Init method (to be used if constructed with default contructor)
	void Init(CUniString name, CUniString description,
			  bool initialValue, bool isSet); //Init method (to be used if constructed with default contructor)
	void Init(int id, CUniString name, CUniString description,
			  bool initialValue); //Init method (to be used if constructed with default contructor)
	void Init(int id, CUniString name, CUniString description,
			  bool initialValue, bool isSet); 

	bool	GetValue(bool throwExceptionIfValueNotSet = true);

	void		SetValue(bool value, bool notifyListeners); 
	inline void	SetValue(bool value) { SetValue(value, true); };

	virtual CParameter	*	Clone();
	virtual void			CopyTo(CParameter * target);
	CUniString			GetLogMessage();
	
private:
	bool		m_Value;
};


/*
 * Class CStringParameter
 *
 * Parameter holding a string value.
 *
 * CC 14.12.2009 - created
 */

class DllExport CStringParameter : public CParameter
{
public:
	CStringParameter();
	CStringParameter(CUniString name, CUniString description,
					 CUniString initialValue);
	CStringParameter(int id, CUniString name, CUniString description,
					 CUniString initialValue);
	CStringParameter(int id, CUniString name, CUniString description,
					 CUniString initialValue, bool isSet);

	virtual void Init(CUniString name, CUniString description,
			  CUniString initialValue); //Init method (to be used if constructed with default contructor)
	virtual void Init(CUniString name, CUniString description,
			  CUniString initialValue, bool isSet); //Init method (to be used if constructed with default contructor)
	virtual void Init(int id, CUniString name, CUniString description,
			  CUniString initialValue); //Init method (to be used if constructed with default contructor)
	void Init(int id, CUniString name, CUniString description,
			  CUniString initialValue, bool isSet);

	void Init(CUniString name, CUniString description); //Init method (to be used if constructed with default contructor)
	void Init(int id, CUniString name, CUniString description); //Init method (to be used if constructed with default contructor)

	CUniString	GetValue(bool throwExceptionIfValueNotSet = true);

	virtual void			SetValue(CUniString value, bool notifyListeners); 
	virtual inline void		SetValue(CUniString value) { SetValue(value, true); };

	inline int	GetTextType() { return m_TextType; };
	inline void	SetTextType(int type) { m_TextType = type; };

	inline set<CUniString>	*	GetValidValues() { return &m_ValidValues; };

	virtual CParameter	*	Clone();
	virtual void			CopyTo(CParameter * target);
	CUniString			GetLogMessage();

public:
	static const int TEXTTYPE_MULTILINE		= 0;
	static const int TEXTTYPE_SINGLELINE	= 1;
	static const int TEXTTYPE_LIST			= 2;
	
protected:
	CUniString		m_Value;
	int				m_TextType;			//Hint for GUI
	set<CUniString> m_ValidValues;		//For list params
};


/*
 * Class CIdParameter
 *
 * Extended string parameter with id registration.
 *
 * CC 19.02.2010 - created
 */
class DllExport CIdParameter : public CStringParameter
{
public:
	CIdParameter();
	CIdParameter(CUniString name, CUniString description,
				CUniString initialValue, CIdRegister<void *> * idRegister);
	CIdParameter(int id, CUniString name, CUniString description,
				CUniString initialValue, CIdRegister<void *> * idRegister);

	void Init(CUniString name, CUniString description,
			  CUniString initialValue, CIdRegister<void *> * idRegister); //Init method (to be used if constructed with default contructor)

	void Init(CUniString name, CUniString description,
				CIdRegister<void *> * idRegister); //Init method (to be used if constructed with default contructor)
	void Init(int id, CUniString name, CUniString description,
			  CUniString initialValue, CIdRegister<void *> * idRegister); //Init method (to be used if constructed with default contructor)

	void Init(int id, CUniString name, CUniString description,
				CIdRegister<void *> * idRegister); //Init method (to be used if constructed with default contructor)

	inline void	SetValue(CUniString value) { SetValue(value, true); };
	void		SetValue(CUniString value, bool notifyListeners); 

	inline void	SetIdRegister(CIdRegister<void *> * idRegister) { m_IdRegister = idRegister; };

	virtual CParameter	*	Clone();
	virtual void			CopyTo(CParameter * target);

protected:
	void Init(int id, CUniString name, CUniString description,
			  CUniString initialValue, bool isSet, CIdRegister<void *> * idRegister); 

private:
	CIdRegister<void *> * m_IdRegister;
};


/*
 * Class CIdTypeMap
 *
 *
 * CC 08.03.2010 - created
 */
class DllExport CIdTypeMap
{
public:
	virtual CUniString GetType(int id)=0;
};


/*
 * Class CParameterMap
 *
 * Associative array of parameters. Parameters can be retrieved index, ID or name.
 *
 * CC 14.12.2009 - created
 */

class DllExport CParameterMap
{
public:
	CParameterMap();
	~CParameterMap();

	void					Add(CParameter * param);
	void					Insert(CParameter * param, int index);
	void					Clear();
	void					Reset();

	inline int				GetSize() { return (int)m_Params.size(); };
	CUniString				GetSchemaType(int paramId = 0);

	inline CParameter	*	Get(int index) { return m_Params[index]; };
	CParameter			*	Get(CUniString name);
	CParameter			*	GetParamForId(int id);
	CIdTypeMap			*	GetPropIdTypeMap() { return m_PropIdTypeMap; };

	inline bool				IsSorted() { return m_Sorted; };
	inline void				SetSorted(bool sorted) { m_Sorted = sorted; if (sorted) Sort(); };
	inline void				SetSchemaType(CUniString type) { m_SchemaType = type; }
	inline void				SetPropIdTypeMap(CIdTypeMap * map) { m_PropIdTypeMap = map; };

	void					CopyValues(CParameterMap * params);
	void					CopyValuesByName(CParameterMap * params);
	void					MergeWith(CParameterMap * params, bool overwrite = false, int parameterRangeRestrictionStart = 0, int parameterRangeRestrictionEnd = MAXINT);

	void					Remove(CParameter * param, bool deleteParam = false);
	void					Remove(int index, bool deleteParam = false);
	void					RemoveParamWithId(int id, bool deleteParam = false);

	void					DeleteAll();
	inline CUniString		GetName() { return m_Name; };
	inline void				SetName(CUniString name) { m_Name = name; };
	CParameterMap		*	Clone();

	void					LoadFromIni(CUniString filePath, CUniString section);
	void					SaveToIni(CUniString filePath, CUniString section);

private:
	void	Sort();
	void	Copy(CParameter * source, CParameter * target);

private:
	//TODO Use real map, if GetParameter(string) is used extensivly
	vector<CParameter *>	m_Params;
	map<int, CParameter*>	m_IdMap;
	bool					m_Sorted;
	CUniString				m_SchemaType; //e.g. GraphicRegionType
	CUniString				m_Name;
	CIdTypeMap			*	m_PropIdTypeMap; //Map [property id, type]
};


/*
 * Class CMultiParameter
 *
 * Parameter that holds several child paramerer (of the same type).
 * Changing the parent parameter also changes als child parameters.
 * The first parameter added defines the sub type of the multi parameter.
 *
 * CC 08.02.2010
 */
class DllExport CMultiParameter : CParameter
{
public:
	CMultiParameter(CUniString name, CUniString description);
	CMultiParameter(int id, CUniString name, CUniString description);
private:
	inline CMultiParameter() { m_Type = TYPE_MULTI; m_Id = 0; };

public:
	void	AddChild(CParameter * child);

	inline int		GetSubType() { return m_Subtype; };
	CParameter	*	GetValue();
	CParameter	*	GetFirstLeaf();
	void			SetValue(bool value, bool notifyListeners = true);
	void			SetValue(int value, bool notifyListeners = true);
	void			SetValue(double value, bool notifyListeners = true);
	void			SetValue(CUniString value, bool notifyListeners = true);
	bool			HaveChildrenMultipleValues();
	bool			IsSet();
	void			SetSet(bool isSet);

	virtual CParameter	*	Clone();
	virtual void			CopyTo(CParameter * target);
	CUniString			GetLogMessage();

private:
	vector<CParameter *>	m_Children;
	int						m_Subtype;
};


/*
 * Class CParameterChangeListener
 *
 * Is notified when the SetValue of a parameter is called.
 *
 * CC 12.02.2010 - created
 */
class DllExport CParameterChangeListener
{
public:
	virtual ~CParameterChangeListener();
	virtual void OnParameterChange() = 0;
};


/*
 * Class CParameterReader
 *
 * Interface for reading parameters from a file.
 *
 * CC 04.03.2010 - created
 */
class DllExport CParameterReader
{
public:
	virtual CParameterMap * ReadParameters(CUniString fileName) = 0;
	virtual CParameterMap * ReadParameters() = 0;
};


/*
 * Class CParameterWriter
 *
 * Interface for writing parameters to a file.
 *
 * CC 04.03.2010 - created
 */
class DllExport CParameterWriter
{
public:
	virtual void WriteParameters(CParameterMap * params, CUniString fileName) = 0;
	virtual void AppendParameters(CParameterMap * params) = 0;
};

}