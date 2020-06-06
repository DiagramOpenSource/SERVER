
#include <windows.h>
#include <windowsx.h>
#include <direct.h>
#include <shellapi.h>
#define _WIN32_IE 0x0200
#include <shlobj.h>


extern "C" BOOL CreateShortcut(char *sData,char *sDataFile,char *sArguments,char *grupo);


BOOL CreateShortcut(char *sData,char *sDataFile,char *sArguments,char *grupo)
{
	HRESULT hres;
	IShellLink* pIShellLink;
	char szTempPath[MAX_PATH];
	char sworkdir[MAX_PATH];
	BOOL rr = FALSE;	
	int i;

	/*
	sData.Replace('<','-');
	sData.Replace('>','-');
	sData.Replace('\\','-');
	sData.Replace('/','-');
	sData.Replace(':','-');
	sData.Replace('*','-');
	sData.Replace('?','-');
	sData.Replace('"','-');
	*/

	szTempPath[0] = 0;

	hres = CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, 
	IID_IShellLink, (LPVOID *)&pIShellLink);

	if (SUCCEEDED(hres))
	{
		IPersistFile *pIPersistFile;
		LPITEMIDLIST pidl = NULL;
		HRESULT hr;
		LPMALLOC pMalloc;		
		int nFolder = CSIDL_COMMON_PROGRAMS;
		
		{
			OSVERSIONINFO     verInfo = {0};
			
			verInfo.dwOSVersionInfoSize = sizeof (verInfo);     
			GetVersionEx(&verInfo); 
			switch (verInfo.dwPlatformId)     
			{ 
				/*
			case VER_PLATFORM_WIN32_NT: 
				GetTaskList     = GetTaskListNT; 
				break;  
			*/
			case VER_PLATFORM_WIN32_WINDOWS:        
				nFolder = CSIDL_PROGRAMS;
				break;      
			}			
		}

		
		{
			typedef HRESULT (CALLBACK* DLLSHGetSpecialFolderPath)(HWND hwndOwner,LPTSTR lpszPath,int nFolder,BOOL fCreate);	
			HINSTANCE   hShell32;
			DLLSHGetSpecialFolderPath pDLLSHGetSpecialFolderPath;
			hShell32 = LoadLibrary("shell32.dll");			
			if (hShell32)
			{				
				pDLLSHGetSpecialFolderPath = (DLLSHGetSpecialFolderPath)GetProcAddress(hShell32,"SHGetSpecialFolderPathA");
				if (pDLLSHGetSpecialFolderPath)
				{
					pDLLSHGetSpecialFolderPath(NULL,szTempPath,nFolder,TRUE);
				}
				else
				{
					hr = SHGetSpecialFolderLocation(NULL, nFolder, &pidl);
					if (hr == NOERROR)
					{
						SHGetPathFromIDList(pidl, szTempPath);
						hr = SHGetMalloc(&pMalloc);
						if (hr == NOERROR)
						{
							pMalloc->Free(pidl);
							pMalloc->Release();
						}
					}
				}
				FreeLibrary(hShell32);
			}
		}
		/*SHGetSpecialFolderPath(NULL,szTempPath,CSIDL_COMMON_PROGRAMS,TRUE);*/

		strcat(szTempPath,"\\");
		strcat(szTempPath,grupo);
		mkdir(szTempPath);
		strcat(szTempPath,"\\");
		strcat(szTempPath,sData);
		strcat(szTempPath,".lnk");		

		hres = pIShellLink->SetPath(sDataFile);

		strcpy(sworkdir,sDataFile);
		for (i = strlen(sworkdir)-1;i >= 0;i--)
		{
			if (sworkdir[i] == '/' || sworkdir[i] == '\\')
			{
				sworkdir[i] = 0;
				break;
			}
		}
		pIShellLink->SetWorkingDirectory(sworkdir);

		pIShellLink->SetIconLocation(sDataFile,1);

        pIShellLink->SetArguments(  sArguments );		

		hres = pIShellLink->QueryInterface(IID_IPersistFile, 
		(LPVOID *)&pIPersistFile);

		if (SUCCEEDED(hres))
		{
			OLECHAR  szTempPathW[MAX_PATH]; 
			memset((void *)szTempPathW,0,sizeof(szTempPathW));
			MultiByteToWideChar(CP_ACP,0,szTempPath,strlen(szTempPath),szTempPathW,MAX_PATH);
			hres = pIPersistFile->Save(szTempPathW, TRUE);
			pIPersistFile->Release();

			rr = TRUE;
		}

		pIShellLink->Release();
	}

	return rr;
}
