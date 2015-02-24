/* Copyright (c) 2013 - All Rights Reserved
 *   Thomas Hauth  <Thomas.Hauth@cern.ch>
 *   Joram Berger  <Joram.Berger@cern.ch>
 *   Dominik Haitz <Dominik.Haitz@kit.edu>
 */

#pragma once

#include "VarCache.h"
#include "Artus/Utility/interface/Utility.h"


/**
   Implements a Setting with automatic read + caching from a Boost PropertyTree
   You can access the value via myObject.GetSNAME
*/

#define IMPL_SETTING_PRIVATE(TYPE, SNAME, READGLOBAL) \
private: \
	TYPE m_##SNAME; \
public: \
	std::string Key##SNAME () const { \
		return (#SNAME); \
	} \
	std::string FullKey##SNAME () const { \
		if ( READGLOBAL || GetPropTreePath() == "" ) { \
			return #SNAME; \
		} else { \
			return GetPropTreePath() + "." + #SNAME; \
		} \
	} \
	mutable VarCache<TYPE> Cache##SNAME; \
	TYPE Get##SNAME ( ) const { \
		if (Cache##SNAME.IsCached()) { \
			return Cache##SNAME.GetValue(); \
		} \
		TYPE val = TYPE(); \
		try { \
			val = GetPropTree()->get< TYPE >( FullKey##SNAME ()); \
		} catch(...) { \
			try { \
				val = GetPropTree()->get< TYPE >( Key##SNAME ()); \
			} catch(...) { \
				LOG(FATAL) << "Could not read value for config tag \"" << (#SNAME) << "\" in pipeline or global settings! It is either not specified or the specified type is incompatible!"; \
			} \
		} \
		Cache##SNAME.SetCache( val ); \
		return val; \
	}

/**
   Implements a Setting with automatic read + caching from a Boost PropertyTree
   You can set a default value which will be used if the entry was not found in the PropertyTree
   You can access the value via myObject.GetSNAME
*/

#define IMPL_SETTING_DEFAULT_PRIVATE(TYPE, SNAME, DEFAULT_VAL, READGLOBAL) \
private: \
	TYPE m_##SNAME; \
public: \
	std::string Key##SNAME () const { \
		return (#SNAME); \
	} \
	std::string FullKey##SNAME () const { \
		if ( READGLOBAL || GetPropTreePath() == "" ) { \
			return #SNAME; \
		} else { \
			return GetPropTreePath() + "." + #SNAME; \
		} \
	} \
	mutable VarCache<TYPE> Cache##SNAME; \
	TYPE Get##SNAME ( ) const { \
		if (Cache##SNAME.IsCached()) { \
			return Cache##SNAME.GetValue(); \
		} \
		TYPE val; \
		try { \
		val = GetPropTree()->get< TYPE >( FullKey##SNAME () ); \
		} catch(...) { \
		val = GetPropTree()->get< TYPE >( Key##SNAME (), DEFAULT_VAL ); \
		} \
		Cache##SNAME.SetCache( val ); \
		return val; \
	}

#define IMPL_SETTING(TYPE, SNAME) IMPL_SETTING_PRIVATE(TYPE, SNAME, false)
#define IMPL_SETTING_DEFAULT(TYPE, SNAME, DEFAULT_VAL) IMPL_SETTING_DEFAULT_PRIVATE(TYPE, SNAME, DEFAULT_VAL, false)

// #define IMPL_GLOBAL_SETTING(TYPE, SNAME) IMPL_SETTING_PRIVATE(TYPE, SNAME, true)
// #define IMPL_GLOBAL_SETTING_DEFAULT(TYPE, SNAME, DEFAULT_VAL) IMPL_SETTING_DEFAULT_PRIVATE(TYPE, SNAME, DEFAULT_VAL, true)

#define IMPL_SETTING_STRINGLIST( SNAME ) \
VarCache<stringvector> m_##SNAME; \
stringvector& Get##SNAME () const { \
	try { \
		RETURN_CACHED(m_##SNAME, PropertyTreeSupport::GetAsStringList(GetPropTree(), GetPipelinePrefix() + #SNAME )) \
	} \
	catch(...) { \
		try { \
			RETURN_CACHED(m_##SNAME, PropertyTreeSupport::GetAsStringList(GetPropTree(), #SNAME )) \
		} \
		catch(...) { \
			LOG(FATAL) << "Could not read value for config tag \"" << (#SNAME) << "\" in pipeline or global settings! It is either not specified or the specified type is incompatible!"; \
			throw; \
		} \
	} \
}

#define IMPL_SETTING_STRINGLIST_DEFAULT( SNAME, DEFAULT_VAL ) \
VarCache<stringvector> m_##SNAME; \
stringvector& Get##SNAME () const { \
	try { \
		RETURN_CACHED(m_##SNAME, PropertyTreeSupport::GetAsStringList(GetPropTree(), GetPipelinePrefix() + #SNAME )) \
	} \
	catch(...) { \
		try { \
			RETURN_CACHED(m_##SNAME, PropertyTreeSupport::GetAsStringList(GetPropTree(), #SNAME )) \
		} \
		catch(...) { \
			RETURN_CACHED(m_##SNAME, DEFAULT_VAL ) \
		} \
	} \
}

#define IMPL_SETTING_SORTED_STRINGLIST( SNAME ) \
VarCache<stringvector> m_##SNAME; \
stringvector& Get##SNAME () const { \
	try { \
		RETURN_CACHED(m_##SNAME, Utility::Sorted(PropertyTreeSupport::GetAsStringList(GetPropTree(), GetPipelinePrefix() + #SNAME ))) \
	} \
	catch(...) { \
		try { \
			RETURN_CACHED(m_##SNAME, Utility::Sorted(PropertyTreeSupport::GetAsStringList(GetPropTree(), #SNAME ))) \
		} \
		catch(...) { \
			LOG(FATAL) << "Could not read value for config tag \"" << (#SNAME) << "\" in pipeline or global settings! It is either not specified or the specified type is incompatible!"; \
			throw; \
		} \
	} \
}

#define IMPL_SETTING_SORTED_STRINGLIST_DEFAULT( SNAME, DEFAULT_VAL ) \
VarCache<stringvector> m_##SNAME; \
stringvector& Get##SNAME () const { \
	try { \
		RETURN_CACHED(m_##SNAME, Utility::Sorted(PropertyTreeSupport::GetAsStringList(GetPropTree(), GetPipelinePrefix() + #SNAME ))) \
	} \
	catch(...) { \
		try { \
			RETURN_CACHED(m_##SNAME, Utility::Sorted(PropertyTreeSupport::GetAsStringList(GetPropTree(), #SNAME ))) \
		} \
		catch(...) { \
			RETURN_CACHED(m_##SNAME, DEFAULT_VAL ) \
		} \
	} \
}

#define IMPL_SETTING_DOUBLELIST( SNAME ) \
VarCache<doublevector> m_##SNAME; \
doublevector& Get##SNAME () const { \
	try { \
		RETURN_CACHED(m_##SNAME, PropertyTreeSupport::GetAsDoubleList(GetPropTree(), GetPipelinePrefix() + #SNAME )) \
	} \
	catch(...) { \
		try { \
			RETURN_CACHED(m_##SNAME, PropertyTreeSupport::GetAsDoubleList(GetPropTree(), #SNAME )) \
		} \
		catch(...) { \
			LOG(FATAL) << "Could not read value for config tag \"" << (#SNAME) << "\" in pipeline or global settings! It is either not specified or the specified type is incompatible!"; \
			throw; \
		} \
	} \
}

#define IMPL_SETTING_DOUBLELIST_DEFAULT( SNAME, DEFAULT_VAL ) \
VarCache<doublevector> m_##SNAME; \
doublevector& Get##SNAME () const { \
	try { \
		RETURN_CACHED(m_##SNAME, PropertyTreeSupport::GetAsDoubleList(GetPropTree(), GetPipelinePrefix() + #SNAME )) \
	} \
	catch(...) { \
		try { \
			RETURN_CACHED(m_##SNAME, PropertyTreeSupport::GetAsDoubleList(GetPropTree(), #SNAME )) \
		} \
		catch(...) { \
			RETURN_CACHED(m_##SNAME, DEFAULT_VAL ) \
		} \
	} \
}

#define IMPL_SETTING_SORTED_DOUBLELIST( SNAME ) \
VarCache<doublevector> m_##SNAME; \
doublevector& Get##SNAME () const { \
	try { \
		RETURN_CACHED(m_##SNAME, Utility::Sorted(PropertyTreeSupport::GetAsDoubleList(GetPropTree(), GetPipelinePrefix() + #SNAME ))) \
	} \
	catch(...) { \
		try { \
			RETURN_CACHED(m_##SNAME, Utility::Sorted(PropertyTreeSupport::GetAsDoubleList(GetPropTree(), #SNAME ))) \
		} \
		catch(...) { \
			LOG(FATAL) << "Could not read value for config tag \"" << (#SNAME) << "\" in pipeline or global settings! It is either not specified or the specified type is incompatible!"; \
			throw; \
		} \
	} \
}

#define IMPL_SETTING_SORTED_DOUBLELIST_DEFAULT( SNAME, DEFAULT_VAL ) \
VarCache<doublevector> m_##SNAME; \
doublevector& Get##SNAME () const { \
	try { \
		RETURN_CACHED(m_##SNAME, Utility::Sorted(PropertyTreeSupport::GetAsDoubleList(GetPropTree(), GetPipelinePrefix() + #SNAME ))) \
	} \
	catch(...) { \
		try { \
			RETURN_CACHED(m_##SNAME, Utility::Sorted(PropertyTreeSupport::GetAsDoubleList(GetPropTree(), #SNAME ))) \
		} \
		catch(...) { \
			RETURN_CACHED(m_##SNAME, DEFAULT_VAL ) \
		} \
	} \
}

#define IMPL_SETTING_FLOATLIST( SNAME ) \
VarCache<floatvector> m_##SNAME; \
floatvector& Get##SNAME () const { \
	try { \
		RETURN_CACHED(m_##SNAME, PropertyTreeSupport::GetAsFloatList(GetPropTree(), #SNAME )) \
	} \
	catch(...) { \
		try { \
			RETURN_CACHED(m_##SNAME, PropertyTreeSupport::GetAsFloatList(GetPropTree(), #SNAME )) \
		} \
		catch(...) { \
			LOG(FATAL) << "Could not read value for config tag \"" << (#SNAME) << "\" in pipeline or global settings! It is either not specified or the specified type is incompatible!"; \
			throw; \
		} \
	} \
}

#define IMPL_SETTING_FLOATLIST_DEFAULT( SNAME, DEFAULT_VAL) \
VarCache<floatvector> m_##SNAME; \
floatvector& Get##SNAME () const { \
	try { \
		RETURN_CACHED(m_##SNAME, PropertyTreeSupport::GetAsFloatList(GetPropTree(), #SNAME )) \
	} \
	catch(...) { \
		try { \
			RETURN_CACHED(m_##SNAME, PropertyTreeSupport::GetAsFloatList(GetPropTree(), #SNAME )) \
		} \
		catch(...) { \
			RETURN_CACHED(m_##SNAME, DEFAULT_VAL ) \
		} \
	} \
}

#define IMPL_SETTING_SORTED_FLOATLIST( SNAME ) \
VarCache<floatvector> m_##SNAME; \
floatvector& Get##SNAME () const { \
	try { \
		RETURN_CACHED(m_##SNAME, Utility::Sorted(PropertyTreeSupport::GetAsFloatList(GetPropTree(), GetPipelinePrefix() + #SNAME ))) \
	} \
	catch(...) { \
		try { \
			RETURN_CACHED(m_##SNAME, Utility::Sorted(PropertyTreeSupport::GetAsFloatList(GetPropTree(), #SNAME ))) \
		} \
		catch(...) { \
			LOG(FATAL) << "Could not read value for config tag \"" << (#SNAME) << "\" in pipeline or global settings! It is either not specified or the specified type is incompatible!"; \
			throw; \
		} \
	} \
}

#define IMPL_SETTING_SORTED_FLOATLIST_DEFAULT( SNAME, DEFAULT_VAL ) \
VarCache<floatvector> m_##SNAME; \
floatvector& Get##SNAME () const { \
	try { \
		RETURN_CACHED(m_##SNAME, Utility::Sorted(PropertyTreeSupport::GetAsFloatList(GetPropTree(), GetPipelinePrefix() + #SNAME ))) \
	} \
	catch(...) { \
		try { \
			RETURN_CACHED(m_##SNAME, Utility::Sorted(PropertyTreeSupport::GetAsFloatList(GetPropTree(), #SNAME ))) \
		} \
		catch(...) { \
			RETURN_CACHED(m_##SNAME, DEFAULT_VAL ) \
		} \
	} \
}

#define IMPL_SETTING_INTLIST( SNAME ) \
VarCache<intvector> m_##SNAME; \
intvector& Get##SNAME () const { \
	try { \
		RETURN_CACHED(m_##SNAME, PropertyTreeSupport::GetAsIntList(GetPropTree(), GetPipelinePrefix() + #SNAME )) \
	} \
	catch(...) { \
		try { \
			RETURN_CACHED(m_##SNAME, PropertyTreeSupport::GetAsIntList(GetPropTree(), #SNAME )) \
		} \
		catch(...) { \
			LOG(FATAL) << "Could not read value for config tag \"" << (#SNAME) << "\" in pipeline or global settings! It is either not specified or the specified type is incompatible!"; \
			throw; \
		} \
	} \
}

#define IMPL_SETTING_INTLIST_DEFAULT( SNAME, DEFAULT_VAL ) \
VarCache<intvector> m_##SNAME; \
intvector& Get##SNAME () const { \
	try { \
		RETURN_CACHED(m_##SNAME, PropertyTreeSupport::GetAsIntList(GetPropTree(), GetPipelinePrefix() + #SNAME )) \
	} \
	catch(...) { \
		try { \
			RETURN_CACHED(m_##SNAME, PropertyTreeSupport::GetAsIntList(GetPropTree(), #SNAME )) \
		} \
		catch(...) { \
			RETURN_CACHED(m_##SNAME, DEFAULT_VAL) \
		} \
	} \
}

#define IMPL_SETTING_SORTED_INTLIST( SNAME ) \
VarCache<intvector> m_##SNAME; \
intvector& Get##SNAME () const { \
	try { \
		RETURN_CACHED(m_##SNAME, Utility::Sorted(PropertyTreeSupport::GetAsIntList(GetPropTree(), GetPipelinePrefix() + #SNAME ))) \
	} \
	catch(...) { \
		try { \
			RETURN_CACHED(m_##SNAME, Utility::Sorted(PropertyTreeSupport::GetAsIntList(GetPropTree(), #SNAME ))) \
		} \
		catch(...) { \
			LOG(FATAL) << "Could not read value for config tag \"" << (#SNAME) << "\" in pipeline or global settings! It is either not specified or the specified type is incompatible!"; \
			throw; \
		} \
	} \
}

#define IMPL_SETTING_SORTED_INTLIST_DEFAULT( SNAME, DEFAULT_VAL ) \
VarCache<intvector> m_##SNAME; \
intvector& Get##SNAME () const { \
	try { \
		RETURN_CACHED(m_##SNAME, Utility::Sorted(PropertyTreeSupport::GetAsIntList(GetPropTree(), GetPipelinePrefix() + #SNAME ))) \
	} \
	catch(...) { \
		try { \
			RETURN_CACHED(m_##SNAME, Utility::Sorted(PropertyTreeSupport::GetAsIntList(GetPropTree(), #SNAME ))) \
		} \
		catch(...) { \
			RETURN_CACHED(m_##SNAME, DEFAULT_VAL) \
		} \
	} \
}


#define IMPL_SETTING_UINT64LIST( SNAME ) \
VarCache<uint64vector> m_##SNAME; \
uint64vector& Get##SNAME () const { \
	try { \
		RETURN_CACHED(m_##SNAME, PropertyTreeSupport::GetAsUInt64List(GetPropTree(), GetPipelinePrefix() + #SNAME )) \
	} \
	catch(...) { \
		try { \
			RETURN_CACHED(m_##SNAME, PropertyTreeSupport::GetAsUInt64List(GetPropTree(), #SNAME )) \
		} \
		catch(...) { \
			LOG(FATAL) << "Could not read value for config tag \"" << (#SNAME) << "\" in pipeline or global settings! It is either not specified or the specified type is incompatible!"; \
			throw; \
		} \
	} \
}

#define IMPL_SETTING_UINT64LIST_DEFAULT( SNAME, DEFAULT_VAL ) \
VarCache<uint64vector> m_##SNAME; \
uint64vector& Get##SNAME () const { \
	try { \
		RETURN_CACHED(m_##SNAME, PropertyTreeSupport::GetAsUInt64List(GetPropTree(), GetPipelinePrefix() + #SNAME )) \
	} \
	catch(...) { \
		try { \
			RETURN_CACHED(m_##SNAME, PropertyTreeSupport::GetAsUInt64List(GetPropTree(), #SNAME )) \
		} \
		catch(...) { \
			RETURN_CACHED(m_##SNAME, DEFAULT_VAL ) \
		} \
	} \
}


#define IMPL_SETTING_SORTED_UINT64LIST( SNAME ) \
VarCache<uint64vector> m_##SNAME; \
uint64vector& Get##SNAME () const { \
	try { \
		RETURN_CACHED(m_##SNAME, Utility::Sorted(PropertyTreeSupport::GetAsUInt64List(GetPropTree(), GetPipelinePrefix() + #SNAME ))) \
	} \
	catch(...) { \
		try { \
			RETURN_CACHED(m_##SNAME, Utility::Sorted(PropertyTreeSupport::GetAsUInt64List(GetPropTree(), #SNAME ))) \
		} \
		catch(...) { \
			LOG(FATAL) << "Could not read value for config tag \"" << (#SNAME) << "\" in pipeline or global settings! It is either not specified or the specified type is incompatible!"; \
			throw; \
		} \
	} \
}

#define IMPL_SETTING_SORTED_UINT64LIST_DEFAULT( SNAME, DEFAULT_VAL ) \
VarCache<uint64vector> m_##SNAME; \
uint64vector& Get##SNAME () const { \
	try { \
		RETURN_CACHED(m_##SNAME, Utility::Sorted(PropertyTreeSupport::GetAsUInt64List(GetPropTree(), GetPipelinePrefix() + #SNAME ))) \
	} \
	catch(...) { \
		try { \
			RETURN_CACHED(m_##SNAME, Utility::Sorted(PropertyTreeSupport::GetAsUInt64List(GetPropTree(), #SNAME ))) \
		} \
		catch(...) { \
			RETURN_CACHED(m_##SNAME, DEFAULT_VAL ) \
		} \
	} \
}
