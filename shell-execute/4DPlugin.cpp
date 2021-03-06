/* --------------------------------------------------------------------------------
 #
 #	4DPlugin.cpp
 #	source generated by 4D Plugin Wizard
 #	Project : shell-execute
 #	author : miyako
 #	2015/08/21
 #
 # --------------------------------------------------------------------------------*/


#include "4DPluginAPI.h"
#include "4DPlugin.h"

void PluginMain(PA_long32 selector, PA_PluginParameters params)
{
	try
	{
		PA_long32 pProcNum = selector;
		sLONG_PTR *pResult = (sLONG_PTR *)params->fResult;
		PackagePtr pParams = (PackagePtr)params->fParameters;

		CommandDispatcher(pProcNum, pResult, pParams); 
	}
	catch(...)
	{

	}
}

void CommandDispatcher (PA_long32 pProcNum, sLONG_PTR *pResult, PackagePtr pParams)
{
	switch(pProcNum)
	{
// --- shell-execute

		case 1 :
			Find_best_application(pResult, pParams);
			break;

		case 2 :
			GET_PROCESS_LIST(pResult, pParams);
			break;

		case 3 :
			OPEN_WITH_APPLICATION(pResult, pParams);
			break;

		case 4 :
			PRINT_WITH_APPLICATION(pResult, pParams);
			break;

	}
}

// --------------------------------- shell-execute --------------------------------


void Find_best_application(sLONG_PTR *pResult, PackagePtr pParams)
{
	C_TEXT Param1;
	C_TEXT returnValue;

	Param1.fromParamAtIndex(pParams, 1);

#if VERSIONWIN
	LPCTSTR lpFile = (LPCTSTR)Param1.getUTF16StringPtr();
	wchar_t lpResult[MAX_PATH];
	if((unsigned int)FindExecutable(lpFile, NULL, lpResult) >> 5){// >32
        returnValue.setUTF16String((const PA_Unichar*)lpResult, wcslen(lpResult));
	}
#else
    NSString *path = Param1.copyPath();
    CFURLRef url;
    if(LSGetApplicationForInfo(
        kLSUnknownType,
        kLSUnknownCreator,
        (CFStringRef)[path pathExtension],
        kLSRolesAll,
        NULL,
        &url) != kLSApplicationNotFoundErr){
            NSString *appPath = (NSString *)CFURLCopyFileSystemPath(url, kCFURLHFSPathStyle);
            CFRelease(url);
            returnValue.setUTF16String(appPath);
            [appPath release];
	}
    [path release];
#endif

	returnValue.setReturn(pResult);
}

void GET_PROCESS_LIST(sLONG_PTR *pResult, PackagePtr pParams)
{
    ARRAY_TEXT Param1;
    ARRAY_TEXT Param2;
    ARRAY_LONGINT Param3;

	Param1.fromParamAtIndex(pParams, 1);
	Param2.fromParamAtIndex(pParams, 2);
	Param3.fromParamAtIndex(pParams, 3);

	Param1.setSize(1);
	Param2.setSize(1);
	Param3.setSize(1);
    
#if VERSIONWIN
	using namespace std;
	
	HANDLE hProcessSnap = INVALID_HANDLE_VALUE;
	PROCESSENTRY32 pe32;
	
	HANDLE hModuleSnap = INVALID_HANDLE_VALUE;
	MODULEENTRY32 me32;
	
	hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	
	if(hProcessSnap != INVALID_HANDLE_VALUE)
	{
		pe32.dwSize = sizeof(PROCESSENTRY32);
		
		if(Process32First(hProcessSnap, &pe32))
		{
			do
			{
				hModuleSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, pe32.th32ProcessID);
				wstring process_name = pe32.szExeFile;
				if(hModuleSnap != INVALID_HANDLE_VALUE)
				{
					me32.dwSize = sizeof(MODULEENTRY32);
					
					if(Module32First(hModuleSnap, &me32))
					{
						do
						{
							wstring module_name = me32.szModule;
							
							if(module_name == process_name)
							{
								CUTF16String p1 = (const PA_Unichar*)me32.szModule;
								Param1.appendUTF16String(&p1);
								CUTF16String p2 = (const PA_Unichar*)me32.szExePath;
								Param2.appendUTF16String(&p2);								
								Param3.appendIntValue(me32.th32ProcessID);							
							}
							
						}while(Module32Next(hModuleSnap, &me32));
						CloseHandle(hModuleSnap);
					}
				}
				
			}while(Process32Next(hProcessSnap, &pe32));	
			CloseHandle(hProcessSnap);			 
		}
	}
#else
	NSArray *launchedApplications = [[NSWorkspace sharedWorkspace] runningApplications];
	
	unsigned int i;
	
	for(i = 0; i < [launchedApplications count]; ++i)
	{
		NSString *NSApplicationName = (NSString *)[[launchedApplications objectAtIndex:i] valueForKey:@"NSApplicationName"];
		Param1.appendUTF16String(NSApplicationName);
		
		NSString *NSApplicationPath = (NSString *)[[launchedApplications objectAtIndex:i] valueForKey:@"NSApplicationPath"];
		NSURL *url = [[NSURL alloc]initFileURLWithPath:NSApplicationPath];
		
		if(url)
		{	
			NSString *filePath = (NSString *)CFURLCopyFileSystemPath((CFURLRef)url, kCFURLHFSPathStyle);
			Param2.appendUTF16String(filePath);
			[filePath release];
			[url release];
		}else{
			Param2.appendUTF16String(@"");
		}
		
		NSNumber *NSApplicationProcessIdentifier = (NSNumber *)[[launchedApplications objectAtIndex:i] valueForKey:@"NSApplicationProcessIdentifier"];
		Param3.appendIntValue([NSApplicationProcessIdentifier intValue]);
	}				
#endif
}

void OPEN_WITH_APPLICATION(sLONG_PTR *pResult, PackagePtr pParams)
{
	C_TEXT Param1;
	C_TEXT Param2;

	Param1.fromParamAtIndex(pParams, 1);
	Param2.fromParamAtIndex(pParams, 2);

#if VERSIONWIN
	LPCTSTR lpFile = (LPCTSTR)Param1.getUTF16StringPtr();
	LPCTSTR lpParameters = (LPCTSTR)Param1.getUTF16StringPtr();
	ShellExecute(NULL, L"open", lpFile, lpParameters, NULL, SW_SHOWNORMAL);
#else
	NSString *document_path = Param1.copyPath();
    if(document_path){
        NSString *application_path = Param2.copyPath();
        if(application_path){
            [[NSWorkspace sharedWorkspace]
            openFile:document_path
            withApplication:application_path];
            [application_path release];
        }
        [document_path release];
    }
#endif
}

void PRINT_WITH_APPLICATION(sLONG_PTR *pResult, PackagePtr pParams)
{
	C_TEXT Param1;
	C_TEXT Param2;

	Param1.fromParamAtIndex(pParams, 1);
	Param2.fromParamAtIndex(pParams, 2);

#if VERSIONWIN
	LPCTSTR lpFile = (LPCTSTR)Param1.getUTF16StringPtr();
	LPCTSTR lpParameters = (LPCTSTR)Param1.getUTF16StringPtr();
	ShellExecute(NULL, L"print", lpFile, lpParameters, NULL, SW_SHOWNORMAL);
#else
	NSURL *document_url = Param1.copyUrl();
    if(document_url){
        NSString *application_path = Param2.copyPath();
        if(application_path){
            NSBundle *bundle = [[NSBundle alloc]initWithPath:application_path];
            if(bundle){
                NSString *bundleIdentifier = [bundle bundleIdentifier];
                [[NSWorkspace sharedWorkspace]
                openURLs:[NSArray arrayWithObject:document_url]
                withAppBundleIdentifier:bundleIdentifier
                              options:NSWorkspaceLaunchAndPrint
                                      |NSWorkspaceLaunchWithoutActivation
                                      |NSWorkspaceLaunchAsync
       additionalEventParamDescriptor:NULL
                    launchIdentifiers:NULL];
                [bundle release];
            }
            [application_path release];
        }
        [document_url release];
    }
#endif
}

