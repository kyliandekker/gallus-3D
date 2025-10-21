#pragma once

#include <rapidjson/document.h>
#include <string>

namespace rapidjson
{
	inline bool GetString(const rapidjson::Value& a_Document, const std::string& a_Key, std::string& a_Value)
	{
		if (!a_Document.HasMember(a_Key.c_str()))
		{
			return false;
		}
		if (!a_Document[a_Key.c_str()].IsString())
		{
			return false;
		}
		a_Value = a_Document[a_Key.c_str()].GetString();
		return true;
	}

	inline bool GetFloat(const rapidjson::Value& a_Document, const std::string& a_Key, float& a_Value)
	{
		if (!a_Document.HasMember(a_Key.c_str()))
		{
			return false;
		}
		if (!a_Document[a_Key.c_str()].IsFloat())
		{
			return false;
		}
		a_Value = a_Document[a_Key.c_str()].GetFloat();
		return true;
	}

	inline bool GetBool(const rapidjson::Value& a_Document, const std::string& a_Key, bool& a_Value)
	{
		if (!a_Document.HasMember(a_Key.c_str()))
		{
			return false;
		}
		if (!a_Document[a_Key.c_str()].IsBool())
		{
			return false;
		}
		a_Value = a_Document[a_Key.c_str()].GetBool();
		return true;
	}

	inline bool GetLong(const rapidjson::Value& a_Document, const std::string& a_Key, long long& a_Value)
	{
		if (!a_Document.HasMember(a_Key.c_str()))
		{
			return false;
		}
		if (!a_Document[a_Key.c_str()].IsInt64())
		{
			return false;
		}
		a_Value = a_Document[a_Key.c_str()].GetInt64();
		return true;
	}

	inline bool GetInt(const rapidjson::Value& a_Document, const std::string& a_Key, int& a_Value)
    {
		if (!a_Document.HasMember(a_Key.c_str()))
		{
			return false;
		}
		if (!a_Document[a_Key.c_str()].IsInt())
		{
			return false;
		}
		a_Value = a_Document[a_Key.c_str()].GetInt();
		return true;
    }

	// Helper to set or add a string member
	inline void SetOrAddMember(Value& obj, const char* name, const char* value, Document::AllocatorType& allocator)
	{
		if (obj.HasMember(name))
		{
			obj[name].SetString(value, allocator);
		}
		else
		{
			Value key(name, allocator);        // copy the key string
			Value val(value, allocator);       // copy the value string
			obj.AddMember(key, val, allocator);
		}
	}

	// Overload for int
	inline void SetOrAddMember(Value& obj, const char* name, int value, Document::AllocatorType& allocator)
	{
		if (obj.HasMember(name))
		{
			obj[name].SetInt(value);
		}
		else
		{
			Value key(name, allocator);
			obj.AddMember(key, value, allocator);
		}
	}

	inline void SetOrAddMember(
		rapidjson::Value& obj,
		const char* name,
		rapidjson::Value& value,
		rapidjson::Document::AllocatorType& allocator)
	{
		if (obj.HasMember(name))
		{
			obj[name] = value; // overwrite with new object/array/value
		}
		else
		{
			rapidjson::Value key(name, allocator);
			obj.AddMember(key, value, allocator);
		}
	}

	// For arrays specifically (still rapidjson::Value&, but for clarity)
	inline void SetOrAddArrayMember(
		rapidjson::Value& obj,
		const char* name,
		rapidjson::Value& array,
		rapidjson::Document::AllocatorType& allocator)
	{
		if (obj.HasMember(name))
		{
			obj[name] = array;
		}
		else
		{
			rapidjson::Value key(name, allocator);
			obj.AddMember(key, array, allocator);
		}
	}
}