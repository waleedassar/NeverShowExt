// NeverShowExt.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "windows.h"



int _tmain(int argc, _TCHAR* argv[])
{
	SetErrorMode(SEM_FAILCRITICALERRORS);

	unsigned long inDesc = 0;
	wchar_t** pDescs = (wchar_t**)LocalAlloc(LMEM_ZEROINIT,0x500);
	if(!pDescs)
	{
		wprintf(L"Error allocating memory for SubKeys array\r\n");
		return -3;
	}
	
	
	unsigned long NumSubKeys = 0;
	unsigned long MaxSubKeyNameLength = 0;
	unsigned long ret = RegQueryInfoKey(HKEY_CLASSES_ROOT,0,0,0,&NumSubKeys,&MaxSubKeyNameLength,0,0,0,0,0,0);
	if(ret != 0)
	{
		wprintf(L"Error RegQueryInfoKey(), return value: %X\r\n",ret);
		LocalFree(pDescs);
		return -1;
	}

	if(NumSubKeys == 0 || MaxSubKeyNameLength == 0)
	{
		wprintf(L"Unexpected behavior RegQueryInfoKey()");
		LocalFree(pDescs);
		return -2;
	}

	//wprintf(L"Number of SubKeys: %X\r\n",NumSubKeys);

	unsigned long NameLen = MaxSubKeyNameLength*2;
	wchar_t* Name = (wchar_t*)LocalAlloc(LMEM_ZEROINIT,NameLen);
	if(Name == 0)
	{
		wprintf(L"Error allocating memory for subkey names\r\n");
		return -3;
	}

	//First HKEY_CLASSES_ROOT iteration
	unsigned long i = 0;
	while(i < NumSubKeys)
	{
		unsigned long NameLength = NameLen;
		memset(Name,0,NameLength);

	    ret = RegEnumKeyEx(HKEY_CLASSES_ROOT,i,Name,&NameLength,0,0,0,0);
		if(ret != 0)
		{
			wprintf(L"Error RegEnumKeyEx() index: %X,return value: %X\r\n",i,ret);
			LocalFree(pDescs);
			return -4;
		}
		else
		{
			HKEY hSubKey = 0;
			unsigned long retX = RegOpenKeyEx(HKEY_CLASSES_ROOT,Name,0,KEY_QUERY_VALUE ,&hSubKey);
			if(retX != 0)
			{
				//wprintf(L"Error RegOpenKeyEx(), return value: %X\r\n",retX);
			}
			else
			{
				retX = RegQueryValueEx(hSubKey,L"NeverShowExt",0,0,0,0);
				if(retX != 0)
				{
					//wprintf(L"Error RegQueryValueEx(), return value: %X\r\n",retX);
				}
				else
				{
					//wprintf(L"Name: %s\r\n",Name);
					unsigned long NameLenX = (wcslen(Name)*2) + 2;
					wchar_t* NameX = (wchar_t*)LocalAlloc(LMEM_ZEROINIT,NameLenX);
					if(!NameX)
					{
						wprintf(L"Error allocating memory for subkey name\r\n");
						RegCloseKey(hSubKey);
						LocalFree(Name);
						LocalFree(pDescs);
						return -3;
					}
					wcscpy(NameX,Name);
					pDescs[inDesc] = NameX;
					inDesc++;
				}
				RegCloseKey(hSubKey);
			}
		}
		i += 1;
	}
	//Second HKEY_CLASSES_ROOT iteration
	unsigned long ii = 0;
	while(ii < NumSubKeys)
	{
		unsigned long NameLength_ = NameLen;
		memset(Name,0,NameLength_);

	    ret = RegEnumKeyEx(HKEY_CLASSES_ROOT,ii,Name,&NameLength_,0,0,0,0);
		if(ret != 0)
		{
			wprintf(L"Error RegEnumKeyEx() index: %X,return value: %X\r\n",ii,ret);
			if(pDescs)
			{
				unsigned long c = 0;
				while(pDescs[c]!=0)
				{
					LocalFree(pDescs[c]);
					c += 1;
				}
				LocalFree(pDescs);
			}
			return -4;
		}
		else
		{
			HKEY hSubKey_ = 0;
			unsigned long retX_ = RegOpenKeyEx(HKEY_CLASSES_ROOT,Name,0,KEY_QUERY_VALUE ,&hSubKey_);
			if(retX_ != 0)
			{
				//wprintf(L"Error RegOpenKeyEx(), return value: %X\r\n",retX_);
			}
			else
			{
				unsigned long ValueType = 0;
				wchar_t ValueData[0x100]={0};
				unsigned long ValueDataLen = 0x200;
				retX_ = RegQueryValueEx(hSubKey_,0,0,&ValueType,(unsigned char*)&ValueData,&ValueDataLen);
				if(retX_ != 0)
				{
					//wprintf(L"Error RegQueryValueEx(), return value: %X\r\n",retX_);
				}
				else
				{
					unsigned long c = 0;
					while(c < inDesc)
					{
						wchar_t* Desc = pDescs[c];
						if(!wcsicmp(ValueData,Desc))
						{
							wprintf(L"Ext: %s (Type: %s)\r\n",Name,ValueData);
							//wprintf(L"ZYYName: %s\r\n",ValueData);
						}
						c += 1;
					}
				}
				RegCloseKey(hSubKey_);
			}
		}
		ii += 1;
	}
	//----------------------------------
	
END:
	LocalFree(Name);
	if(pDescs)
	{
		unsigned long cc = 0;
		while(cc < inDesc)
		{
			LocalFree(pDescs[cc]);
			pDescs[cc]=0;
			cc += 1;
		}
		LocalFree(pDescs);
	}
	return 0;
}

