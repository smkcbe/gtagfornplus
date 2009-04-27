#include "stdafx.h"
#include "FileListDialog.h"

FileListDialog::FileListDialog()
: DockingDlgInterface(IDD_DOCK_DLG)
, IsShown( false ){
//	SearchString=L"OpRttCallTrace";
	size_of_content = 0;
}

FileListDialog::~FileListDialog(){}

void FileListDialog::getCurrentDir()
{
	TCHAR path[MAX_PATH];
	::SendMessage(g_NppData._nppHandle, NPPM_GETCURRENTDIRECTORY, MAX_PATH, (LPARAM)path); 

	Dir=std::wstring(path);
}

void FileListDialog::getSearchString() {

	//from MultiClipboard plugin :)

	// Find the length of the text
	int textLength =(int)::SendMessage(g_NppData._scintillaMainHandle, SCI_GETSELTEXT, 0, 0 ); 

	// Create the buffer that will hold the selection text
	std::vector< char > buffer( textLength );
	// And fill it up
	::SendMessage(g_NppData._scintillaMainHandle,SCI_GETSELTEXT, 0, (LPARAM)&buffer[0] );

	// Get code page of scintilla
	UINT codePage = (UINT)::SendMessage(g_NppData._scintillaMainHandle, SCI_GETCODEPAGE,0,0);

	// Create the buffer that will hold the selection text converted into wide char
	std::vector< wchar_t > wbuffer( textLength );
	::MultiByteToWideChar( codePage, 0, &buffer[0], -1, &wbuffer[0], textLength );

	// Set the return value
	SearchString = &wbuffer[0];
}

void FileListDialog::Init( )
{
	DockingDlgInterface::init( g_hInstance, g_NppData._nppHandle );
}

void FileListDialog::ShowDialog( bool Show )
{
	if ( !isCreated() )
	{
		RECT coOrd;
		create(&TBData);
		//if ( !NLGetText( g_hInstance, g_NppData._nppHandle, TEXT("Gtag Search Results"), TBData.pszName, MAX_PATH) )
			lstrcpy( TBData.pszName, TEXT("Gtag Search Results") );

		TBData.uMask			= DWS_DF_CONT_LEFT | DWS_ICONTAB;
		TBData.pszModuleName	= (LPCTSTR)getPluginFileName();
		TBData.dlgID			= FILELIST_DOCKABLE_WINDOW_INDEX ;
		// define the default docking behaviour
		::SendMessage( _hParent, NPPM_DMMREGASDCKDLG, 0,  (LPARAM)&TBData);
		coOrd.left = 5;
		coOrd.top  = 5;
		coOrd.right=600;
		coOrd.bottom=175;
		gtagSearchResult.init(g_hInstance,getHSelf(),coOrd);
		coOrd.left = 610;
		coOrd.top  = 5;
		coOrd.right=665;
		coOrd.bottom=175;
		gtagFunctionList.init(g_hInstance,getHSelf(),coOrd);
	}

	display( Show );
	gtagSearchResult.display(Show);	
	IsShown = Show;
}

void FileListDialog::Search(int SearchType) 
{

    size_t iMyCounter = 0, iReturnVal = 0, iPos = 0;
    DWORD dwExitCode = 0;
    std::wstring sTempStr = L"";
	size_t SecondsToWait =0;

	ShowDialog(true);

    if (Parameters.size() != 0)    {
        if (Parameters[0] != L' ')        {
            Parameters.insert(0,L" ");
        }
    }

    sTempStr = FullPathToExe;
    wchar_t * pwszParam = new wchar_t[Parameters.size() + 1];
    if (pwszParam == 0)
    {
        return;
    }
    const wchar_t* pchrTemp = Parameters.c_str();
	int paramsize=Parameters.size();
	wcscpy_s(pwszParam, paramsize+1 , pchrTemp );


	wchar_t * pwszDir = new wchar_t[Dir.size() + 1];
    if (pwszDir == 0)
    {
        return;
    }
    pchrTemp = Dir.c_str();
    wcscpy_s(pwszDir, Dir.size() + 1, pchrTemp);

    STARTUPINFOW siStartupInfo;
    PROCESS_INFORMATION piProcessInfo;
    memset(&siStartupInfo, 0, sizeof(siStartupInfo));
    memset(&piProcessInfo, 0, sizeof(piProcessInfo));
    siStartupInfo.cb = sizeof(siStartupInfo);
	siStartupInfo.dwFlags=STARTF_USESHOWWINDOW|STARTF_USESTDHANDLES;
	siStartupInfo.wShowWindow = SW_HIDE;
	
	HANDLE g_hChildStd_OUT_Wr = NULL;
	HANDLE g_hChildStd_OUT_Rd = NULL;
	SECURITY_ATTRIBUTES saAttr; 
	saAttr.nLength = sizeof(SECURITY_ATTRIBUTES); 
	saAttr.bInheritHandle = TRUE; 
	saAttr.lpSecurityDescriptor = NULL; 

   if ( ! CreatePipe(&g_hChildStd_OUT_Rd, &g_hChildStd_OUT_Wr, &saAttr, 0) ) 
		return;


    siStartupInfo.hStdOutput = g_hChildStd_OUT_Wr;
	

    if (CreateProcessW(const_cast<LPCWSTR>(FullPathToExe.c_str()),
                            pwszParam, 0, 0, true,
							CREATE_DEFAULT_ERROR_MODE, 0, pwszDir,
                            &siStartupInfo, &piProcessInfo) != false)
    {
        dwExitCode = WaitForSingleObject(piProcessInfo.hProcess, (SecondsToWait * 1000));//shd change to wait object
    }
    else
    {
        iReturnVal = GetLastError();
    }
	
	CloseHandle(g_hChildStd_OUT_Wr);
	ReadFromPipe(g_hChildStd_OUT_Rd,SearchType);
    /* Free memory */
    delete[]pwszParam;
    pwszParam = 0;

    /* Release handles */
    CloseHandle(piProcessInfo.hProcess);
    CloseHandle(piProcessInfo.hThread);

    return;
} 

void FileListDialog::ReadFromPipe(HANDLE g_hChildStd_OUT_Rd,int SearchType) 
{ 
   DWORD dwRead, dwWritten=0,len; 
   char charBuf[CHAR_SIZE]; 
   char *context,*buf,*chBuf=charBuf;
   TCHAR tchBuf[UCHAR_SIZE];
   BOOL bSuccess = FALSE;
   SearchStringSize = SearchString.size();

   if(SearchType == GTAG_FILES)
	   	gtagSearchResult.ClearAll();
   gtagFunctionList.ClearAll();
   
    for (;;) 
   { 
	  if(dwWritten){
		strncpy_s(chBuf,CHAR_SIZE,buf,dwWritten);
		chBuf+=(dwWritten);
	  }
      bSuccess = ReadFile( g_hChildStd_OUT_Rd, chBuf, CHAR_SIZE-dwWritten, &dwRead, NULL);
      if( ! bSuccess || dwRead == 0 ) break; 
	  chBuf=charBuf;	  
	  dwWritten += dwRead;
	  buf=strtok_s(chBuf,"\n",&context);
	  while( buf != NULL ) {
		len = strlen(buf)+1;
		if(len<=dwWritten){
			mbstowcs(tchBuf,buf,len);
			dwWritten-=len;
			if(SearchType==GTAG_FUNCTION &&(StrCmpNW(tchBuf,SearchString.c_str(),SearchStringSize))==0){
				if(StrCmpNW(&tchBuf[SearchStringSize],L" ",1) == 0){
					int looper;
					int numstart=0;
					std::wstring line_num;
					for(looper=SearchStringSize+1;looper<99;looper++) {
						if(StrCmpNW(tchBuf+looper,L" ",1)==0)
							if(numstart)
								break;
							else
								continue;
						else{
							line_num.append(tchBuf+looper,1);
							numstart=1;
						}
					}
					linenum_list.push_back(StrToIntW(line_num.c_str()));
				}
			}
			else if(SearchType==GTAG_FILES)	{
				//for now dont add h files
				file_name = tchBuf;
				if(file_name.find(L".h",file_name.size()-5)==std::wstring::npos)
					gtagSearchResult.AddItem(tchBuf);
			}
			else if(SearchType==CTAG_FUNCTION){
				std::wstring buffer = tchBuf;
				size_t sz = buffer.find(L"function");
				if(sz!=std::wstring::npos && sz!=0){
					unsigned int looper=sz;
					int numstart=0;
					std::wstring line_num;
					for(looper=sz+11;looper<sz+21;looper++) {
						if(StrCmpNW(tchBuf+looper,L" ",1)==0)
							if(numstart)
								break;
							else
								continue;
						else{
							line_num.append(tchBuf+looper,1);
							numstart=1;
						}
					}
					ctag_linenum_list.push_back(StrToIntW(line_num.c_str()));
					ctag_func_list.push_back(tchBuf+looper+1+size_of_content);
				}
			}
			buf = strtok_s( NULL, "\n",&context );
		}
		else
			break;
	  }

   }
#ifdef DEBUG
FreeConsole();
#endif
CloseHandle(g_hChildStd_OUT_Rd);
} 


void FileListDialog::OnListSelectionChanged(){
	int sel;
	sel=gtagSearchResult.GetCurrentSelectionIndex();

}
void FileListDialog::OnFileListDoubleClicked(){
	gtagSearchResult.GetCurrentSelection(file_name);
	if(file_name.find(L".h",file_name.size()-5)!=std::wstring::npos)
		return;
	linenum_list.clear();
	ctag_func_list.clear();
	ctag_linenum_list.clear();
	size_of_content = file_name.size();
	FullPathToExe=L".\\plugins\\gtagfornplus\\global.exe";
	if(IsDefSearch){
		//Parameters = L" -af ";
		int start=0;
		int end=0;
		if((start=file_name.find(L":",2))==std::wstring::npos)
			return;
		else
			if((end=file_name.find(L":",start+1))==std::wstring::npos)
				return;
			else{
				std::wstring num_char = file_name.substr(start+1,end-start);
				std::wstring file_name_part = file_name.substr(0,start);
				int num=StrToIntW(num_char.c_str());
				OpenFileAndGotoLine(file_name_part.c_str(),num);
			}
	}
	else
		Parameters = L" -arf ";
	Parameters.append(SearchString);
	Parameters.append(L" ");
	Parameters.append(file_name);
	Search(GTAG_FUNCTION);
	if(!IsDefSearch){
		FullPathToExe=L".\\plugins\\gtagfornplus\\ctags.exe";
		Parameters = L" -x --sort=no ";
		Parameters.append(file_name);
		Search(CTAG_FUNCTION);
		if((ctag_func_list.size()==ctag_linenum_list.size())&& ctag_func_list.size()!=0){
			int list_size=ctag_linenum_list.size();
			int looper=0;
			std::wstring last_func;
			std::vector<int>::iterator it1;
			std::vector<int>::iterator it2 = ctag_linenum_list.begin();
			std::vector<std::wstring>::iterator it3 = ctag_func_list.begin();
			for ( it1=linenum_list.begin() ; it1 != linenum_list.end(); it1++ ){
				for(looper;looper<list_size;looper++,it2++){
					if(*it2>=*it1){
						std::wstring list_item;
						if(looper){
							it3--;
							list_item.clear();
							list_item.append(*it3);
							list_item.append(L" : ");
						}
						else
							list_item.append(L"Global Declaration : ");

						TCHAR str[20];
						::_itot_s(*it1,str,20,10);
						list_item.append(str);
						gtagFunctionList.AddItem(list_item);
						it3++;
						break;
						}
					if(looper!=(list_size-2))it3++;
				}
				if(looper==list_size){
					//belongs to last function
							it3--;
							std::wstring list_item;
							list_item.clear();
							list_item.append(*it3);
							list_item.append(L" : ");
							TCHAR str[20];
							::_itot_s(*it1,str,20,10);
							list_item.append(str);
							gtagFunctionList.AddItem(list_item);
							it3++;
				}
			}
		}							
		else return; //something wrong... lets find out later


	}//else{
		//goto decl
	//}

}

void FileListDialog::OpenFileAndGotoLine(const wchar_t * file_to_open,int GotoLine){
	::SendMessage(g_NppData._nppHandle, WM_DOOPEN, 0, (LPARAM)file_to_open);
	::SendMessage(g_NppData._scintillaMainHandle, SCI_GOTOLINE, (WPARAM)(GotoLine+10), 0);
	::SendMessage(g_NppData._scintillaMainHandle, SCI_GOTOLINE, (WPARAM)(GotoLine-1), 0);					
	::SendMessage(g_NppData._scintillaMainHandle, SCI_SETFOCUS, (WPARAM)1, 0);	
}

BOOL CALLBACK FileListDialog::run_dlgProc( HWND hWnd, UINT msg, WPARAM wp, LPARAM lp )
{
	switch ( msg )
	{
		case WM_INITDIALOG:
			InitialiseDialog();
			break;

		case WM_SIZE:
		case WM_MOVE:
		case WM_COMMAND:
			if ( (HWND)lp == gtagSearchResult.getHSelf() )
			{
				switch ( HIWORD(wp) )
				{
				case LBN_SELCHANGE:
					if(IsDefSearch)return 0;

				case LBN_DBLCLK:
					OnFileListDoubleClicked();
					return 0;
				}
			}
			if ( (HWND)lp == gtagFunctionList.getHSelf() )
			{
				switch ( HIWORD(wp) )
				{
				case LBN_DBLCLK:
					int array_index=gtagFunctionList.GetCurrentSelectionIndex();
					std::vector<int>::iterator it=linenum_list.begin()+array_index;
					OpenFileAndGotoLine(file_name.c_str(),*it);
					return 0;
				}
			}

			break;
		case WM_NOTIFY:
			{
				LPNMHDR nmhdr = (LPNMHDR) lp;
				if ( nmhdr->hwndFrom == _hParent )
				{
					switch ( LOWORD( nmhdr->code ) )
					{
					case DMN_FLOAT:
					case DMN_DOCK:
						{
							if ( LOWORD( nmhdr->code ) == DMN_FLOAT )
							{
								_isFloating = true;
							}
							else
							{
								_isFloating = false;
								_iDockedPos = HIWORD( nmhdr->code );
							}
							break;
						}
					default:
						// Parse all other notifications to docking dialog interface
						return DockingDlgInterface::run_dlgProc( _hSelf, msg, wp, lp );
					}
				}
				break;
			}
		default:
			return DockingDlgInterface::run_dlgProc( _hSelf, msg, wp, lp );
	}

	return FALSE;
}


void FileListDialog::InitialiseDialog()
{
}
