#include "stdafx.h"
#include "FileListDialog.h"

// Splitter logic from http://www.codeproject.com/KB/winsdk/SplitterWindowProject.aspx
void FileListDialog::DrawXorBar(HDC hdc, int x1, int y1, int width, int height)
{
	static WORD _dotPatternBmp[8] = 
	{ 
		0x00aa, 0x0055, 0x00aa, 0x0055, 
		0x00aa, 0x0055, 0x00aa, 0x0055
	};

	HBITMAP hbm;
	HBRUSH  hbr, hbrushOld;

	hbm = CreateBitmap(8, 8, 1, 1, _dotPatternBmp);
	hbr = CreatePatternBrush(hbm);
	
	SetBrushOrgEx(hdc, x1, y1, 0);
	hbrushOld = (HBRUSH)SelectObject(hdc, hbr);
	
	PatBlt(hdc, x1, y1, width, height, PATINVERT);
	
	SelectObject(hdc, hbrushOld);
	
	DeleteObject(hbr);
	DeleteObject(hbm);
}

LRESULT FileListDialog::Splitter_OnLButtonDown(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	POINT pt;
	HDC hdc;
	RECT rect;

	pt.x = (short)LOWORD(lParam);  // horizontal position of cursor 
	pt.y = (short)HIWORD(lParam);

	GetWindowRect(hwnd, &rect);

	//convert the mouse coordinates relative to the top-left of
	//the window
	ClientToScreen(hwnd, &pt);
	pt.x -= rect.left;
	pt.y -= rect.top;
	
	//same for the window coordinates - make them relative to 0,0
	OffsetRect(&rect, -rect.left, -rect.top);
	
	if(pt.x < 0) pt.x = 0;
	if(pt.x > rect.right-4) 
	{
		pt.x = rect.right-4;
	}

	fDragMode = TRUE;

	SetCapture(hwnd);

	hdc = GetWindowDC(hwnd);
	DrawXorBar(hdc,pt.x - 2, 1,4, rect.right-2);
	ReleaseDC(hwnd, hdc);
	
	oldx = pt.x;
		
	return 0;
}

LRESULT FileListDialog::Splitter_OnLButtonUp(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	HDC hdc;
	RECT rect;

	POINT pt;
	pt.x = (short)LOWORD(lParam);  // horizontal position of cursor 
	pt.y = (short)HIWORD(lParam);

	if(fDragMode == FALSE)
		return 0;
	
	GetWindowRect(hwnd, &rect);

	ClientToScreen(hwnd, &pt);
	pt.x -= rect.left;
	pt.y -= rect.top;
	
	OffsetRect(&rect, -rect.left, -rect.top);

	if(pt.x < 0) pt.x = 0;
	if(pt.x > rect.right-4) 
	{
		pt.x = rect.right-4;
	}

	hdc = GetWindowDC(hwnd);
	DrawXorBar(hdc,oldx - 2, 1,4, rect.right-2);			
	ReleaseDC(hwnd, hdc);

	oldx = pt.x;

	fDragMode = FALSE;

	//convert the splitter position back to screen coords.
	GetWindowRect(hwnd, &rect);
	pt.x += rect.left;
	pt.y += rect.top;

	//now convert into CLIENT coordinates
	ScreenToClient(hwnd, &pt);
	GetClientRect(hwnd, &rect);
	WindowRatio=(float)pt.x/(float)rect.right;

	//position the child controls
	ResizeListBoxes();

	ReleaseCapture();

	return 0;
}

LRESULT FileListDialog::Splitter_OnMouseMove(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	HDC hdc;
	RECT rect;

	POINT pt;

	if(fDragMode == FALSE) return 0;

	pt.x = (short)LOWORD(lParam);  // horizontal position of cursor 
	pt.y = (short)HIWORD(lParam);

	GetWindowRect(hwnd, &rect);

	ClientToScreen(hwnd, &pt);
	pt.x -= rect.left;
	pt.y -= rect.top;

	OffsetRect(&rect, -rect.left, -rect.top);

	if(pt.x < 0) pt.x = 0;
	if(pt.x > rect.right-4) 
	{
		pt.x = rect.right-4;
	}

	if(pt.x != oldx && wParam & MK_LBUTTON)
	{
		hdc = GetWindowDC(hwnd);
		DrawXorBar(hdc, oldx - 2,1, 4,rect.right-2);
		DrawXorBar(hdc, pt.x - 2,1, 4,rect.right-2);
			
		ReleaseDC(hwnd, hdc);

		oldx = pt.x;
	}

	return 0;
}
void FileListDialog::ResizeListBoxes(){
	RECT coOrd;
	unsigned int OldRight;
	getClientRect(coOrd);
	OldRight = coOrd.right;
	coOrd.right*=WindowRatio;
	MoveWindow(gtagSearchResult.getHSelf(),coOrd.left,coOrd.top,coOrd.right-2,coOrd.bottom,TRUE);
	coOrd.left=coOrd.right;
	coOrd.right = OldRight-coOrd.right;
	MoveWindow(gtagFunctionList.getHSelf(),coOrd.left+2,coOrd.top,coOrd.right,coOrd.bottom,TRUE);
}
FileListDialog::FileListDialog()
: DockingDlgInterface(IDD_DOCK_DLG)
, IsShown( false ){
	WindowRatio=(float)0.5;
	file_name_length = 0;
	oldx = -4;
	fMoved = FALSE;
	fDragMode = FALSE;
	IsSymbolSearch = FALSE;
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
		create(&TBData);
		//TCHAR *name=new TCHAR(20);
		//lstrcpy( (LPWSTR)name , TEXT("Gtag Search Results") );
		//TBData.pszName = name;
		lstrcpy( TBData.pszName , TEXT("Gtag Search") );
		TBData.uMask			= DWS_DF_CONT_LEFT | DWS_ICONTAB;
		TBData.pszModuleName	= (LPCTSTR)getPluginFileName();
		TBData.dlgID			= FILELIST_DOCKABLE_WINDOW_INDEX ;
		// define the default docking behaviour
		::SendMessage( _hParent, NPPM_DMMREGASDCKDLG, 0,  (LPARAM)&TBData);
		gtagSearchResult.init(g_hInstance,getHSelf());
		gtagFunctionList.init(g_hInstance,getHSelf());

	}

	display( Show );
	gtagSearchResult.display(Show);	
	gtagFunctionList.display(Show);
	ResizeListBoxes();
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
   std::wstring LastFile,NewFile;
   int Blocks=0;

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
				fileName=tchBuf;
				int colonPos=0,numberPos=0;
				if(IsSymbolSearch){
					if((colonPos=fileName.find(L":",2))==std::wstring::npos)continue;
					NewFile = fileName.substr(0,colonPos);
					if(LastFile.size()==0)LastFile=NewFile;
					if(LastFile!=NewFile){
						gtagSearchResult.AddItem(LastFile);	
						symbol_blocks.push_back(Blocks);
						LastFile=NewFile;
					}
					colonPos++;
					if((numberPos=fileName.find(L":",colonPos))==std::wstring::npos)continue;
					NewFile=fileName.substr(colonPos,numberPos-colonPos);
					symbol_linenum_list.push_back(StrToIntW(NewFile.c_str()));
					NewFile = fileName.substr(numberPos+1);
					NewFile.erase (0, NewFile.find_first_not_of (L" \t\r\n")) ; 
					symbol_list.push_back(NewFile);
					Blocks++;
				}
				else{
				//for now dont add h files
				if(fileName.find(L".h",fileName.size()-5)==std::wstring::npos)
					gtagSearchResult.AddItem(tchBuf);
				}
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
					ctag_func_list.push_back(tchBuf+looper+1+file_name_length);
				}
			}
			buf = strtok_s( NULL, "\n",&context );
		}
		else
			break;
	  }

	}
	if(IsSymbolSearch){
		gtagSearchResult.AddItem(LastFile);	
		symbol_blocks.push_back(Blocks);
	}
	CloseHandle(g_hChildStd_OUT_Rd);
} 

void FileListDialog::OnFileListDoubleClicked(){
	
	if (IsSymbolSearch){
		gtagSearchResult.GetCurrentSelection(fileName,FALSE);
		linenum_list.clear();
		gtagFunctionList.ClearAll();
		int FileSel=gtagSearchResult.GetCurrentSelectionIndex();
		int first,last;
		std::vector<int>::iterator it1=symbol_blocks.begin();
		std::vector<int>::iterator it2=symbol_linenum_list.begin();
		std::vector<std::wstring>::iterator it3 = symbol_list.begin();
		if(FileSel==0){
			first = 0;
			last = *(it1);
		}
		else{
			first=it1[FileSel-1];
			last=it1[FileSel];
		}
		for (first;first<last;first++){
			linenum_list.push_back(it2[first]);
			gtagFunctionList.AddItem(it3[first]);
		}
	}
	else if(IsDefSearch){
		int start=0;
		int end=0;

		gtagSearchResult.GetCurrentSelection(fileName);
		file_name_length = fileName.size();

		if((start=fileName.find(L":",2))==std::wstring::npos)
			return;
		else
			if((end=fileName.find(L":",start+1))==std::wstring::npos)
				return;
			else{
				std::wstring num_char = fileName.substr(start+1,end-start);
				std::wstring file_name_part = fileName.substr(0,start);
				int num=StrToIntW(num_char.c_str());
				OpenFileAndGotoLine(file_name_part.c_str(),num);
			}
			return;
	}
	else{
		TCHAR str[20];
		std::wstring list_item;
		linenum_list.clear();
		gtagSearchResult.GetCurrentSelection(fileName);
		file_name_length = fileName.size();
		ctag_func_list.clear();
		ctag_linenum_list.clear();
		FullPathToExe=L".\\plugins\\gtagfornplus\\global.exe";
		Parameters = L" -arf ";
		Parameters.append(SearchString);
		Parameters.append(L" ");
		Parameters.append(fileName);
		Search(GTAG_FUNCTION);
		FullPathToExe=L".\\plugins\\gtagfornplus\\ctags.exe";
		Parameters = L" -x --sort=no ";
		Parameters.append(fileName);
		Search(CTAG_FUNCTION);
		if((ctag_func_list.size()==ctag_linenum_list.size())&& ctag_func_list.size()!=0){
			int list_size=ctag_linenum_list.size();
			int looper=0;
			
			std::vector<int>::iterator it1;
			std::vector<int>::iterator it2 = ctag_linenum_list.begin();
			std::vector<std::wstring>::iterator it3 = ctag_func_list.begin();

			if(list_size==1){
				for ( it1=linenum_list.begin() ; it1 != linenum_list.end(); it1++ ){
					if(*it2>=*it1){
						list_item.clear();
						list_item.append(*it3);
						list_item.append(L" : ");
					}
					else
						list_item.append(L"Global Declaration : ");

					::_itot_s(*it1,str,20,10);
					list_item.append(str);
					gtagFunctionList.AddItem(list_item);				
				}
			}
			else {
				looper=0;
				for ( it1=linenum_list.begin() ; it1 != linenum_list.end(); it1++ ){
					if(looper==0 && *it1<it2[looper]){
							list_item.clear();
							list_item.append(L"Global Declaration : ");
							list_item.append(L" : ");
							::_itot_s(*it1,str,20,10);
							list_item.append(str);
							gtagFunctionList.AddItem(list_item);
							continue;
					}
					for(;looper<(list_size-2);looper++){
						if(*it1>it2[looper] && *it1<it2[looper+1]){
							list_item.clear();
							list_item.append(it3[looper]);
							list_item.append(L" : ");
							::_itot_s(*it1,str,20,10);
							list_item.append(str);
							gtagFunctionList.AddItem(list_item);
							break;
						}
					}
					if(looper==(list_size-2)){
						//The items may fall between last and 'last but one' functions.
						if(*it1>it2[looper] && *it1<it2[looper+1]){
							list_item.clear();
							list_item.append(it3[looper]);
							list_item.append(L" : ");
							::_itot_s(*it1,str,20,10);
							list_item.append(str);
							gtagFunctionList.AddItem(list_item);
							continue;
						}else 
							looper++;
					}
					if(looper==(list_size-1)){
						list_item.clear();
						list_item.append(it3[looper]);
						list_item.append(L" : ");
						::_itot_s(*it1,str,20,10);
						list_item.append(str);
						gtagFunctionList.AddItem(list_item);
						continue;
					}
				}
			}
		}							
		else return; //something wrong... lets find out later
	}
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
			ResizeListBoxes();
			break;
		case WM_LBUTTONDOWN:
			Splitter_OnLButtonDown(hWnd, msg, wp, lp);
			return 0;

		case WM_LBUTTONUP:
			Splitter_OnLButtonUp(hWnd, msg, wp, lp);
			return 0;

		case WM_MOUSEMOVE:
			Splitter_OnMouseMove(hWnd, msg, wp, lp);
			return 0;
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
					OpenFileAndGotoLine(fileName.c_str(),*it);
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
						return DockingDlgInterface::run_dlgProc(hWnd, msg, wp, lp );
					}
				}
				break;
			}
		default:
			return DockingDlgInterface::run_dlgProc( hWnd, msg, wp, lp );
	}

	return FALSE;
}


void FileListDialog::InitialiseDialog()
{
}

void FileListDialog::DoDeclSearch(){
	IsSymbolSearch = FALSE;
	FullPathToExe = L".\\plugins\\gtagfornplus\\global.exe";
	getSearchString();
	Parameters = L" -a --result=grep ";
	Parameters.append(SearchString);
	getCurrentDir();
	IsDefSearch = TRUE;
	Search();
}

void FileListDialog::DoRefSearch(){
	IsSymbolSearch = FALSE;
	FullPathToExe = L".\\plugins\\gtagfornplus\\global.exe";
	getSearchString();
	Parameters = L" -ar ";
	Parameters.append(SearchString);
	getCurrentDir();
	IsDefSearch = FALSE;
	Search();
	if(gtagSearchResult.GetItemCount()==0)
		IsSymbolSearch = TRUE;
	else
		return;
	symbol_linenum_list.clear();
	symbol_list.clear();
	symbol_blocks.clear();
	Parameters = L" -axs --result=grep ";
	Parameters.append(SearchString);
	Search();
}
