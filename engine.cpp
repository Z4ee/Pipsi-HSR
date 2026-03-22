#include "pch.h"

#include "engine.h"

System::String* CreateSystemString(LPCSTR String, int Length)
{
	return System::String::CreateStringFromEncoding((System::Byte*)String, Length, System::Text::Encoding::get_UTF8());
}

System::String* CreateSystemString(LPCWSTR String, int Length)
{
	return System::String::CreateStringFromEncoding((System::Byte*)String, Length, System::Text::Encoding::get_Unicode());
}
