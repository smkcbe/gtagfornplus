///----------------------------------------------------------------------------
/// Copyright (c) 2008-2009 
/// Brandon Cannaday
/// Paranoid Ferret Productions (support@paranoidferret.com)
///
/// This program is free software; you can redistribute it and/or
/// modify it under the terms of the GNU General Public License
/// as published by the Free Software Foundation; either
/// version 2 of the License, or (at your option) any later version.
///
/// This program is distributed in the hope that it will be useful,
/// but WITHOUT ANY WARRANTY; without even the implied warranty of
/// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
/// GNU General Public License for more details.
///
/// You should have received a copy of the GNU General Public License
/// along with this program; if not, write to the Free Software
/// Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
///----------------------------------------------------------------------------

#include "stdafx.h"
#include "PluginInterface.h"
#include "FileListDialog.h"
#include <string>
#include <vector>


HINSTANCE			g_hInstance = NULL;
NppData				g_NppData;
FileListDialog	file_list_dialog;

const TCHAR PLUGIN_NAME[] = TEXT("Gtag Search");
const int NUM_FUNCS = 3;

FuncItem funcItems[NUM_FUNCS];

//
// Required plugin methods
//
extern "C" __declspec(dllexport) BOOL isUnicode()
{
	return TRUE;
}

extern "C" __declspec(dllexport) void setInfo(NppData notpadPlusData)
{
	g_NppData = notpadPlusData;
	file_list_dialog.Init();
}

extern "C" __declspec(dllexport) const TCHAR * getName()
{
	return PLUGIN_NAME;
}

extern "C" __declspec(dllexport) void beNotified(SCNotification *notifyCode)
{
	
}

extern "C" __declspec(dllexport) LRESULT messageProc(UINT Message, WPARAM wParam, LPARAM lParam)
{
	return TRUE;
}

extern "C" __declspec(dllexport) FuncItem * getFuncsArray(int *nbF)
{
	*nbF = NUM_FUNCS;
	return funcItems;
}

///
/// Gets the path to the current file.
///
/// @return Current file path.
///

void ToggleDialog(){
	HMENU hMenu = ::GetMenu(g_NppData._nppHandle);
	UINT state = ::GetMenuState(hMenu, funcItems[FILELIST_DOCKABLE_WINDOW_INDEX]._cmdID, MF_BYCOMMAND);
	file_list_dialog.ShowDialog( state & MF_CHECKED ? false : true );

}



void FindDecl(){
	file_list_dialog.IsSymbolSearch = FALSE;
	file_list_dialog.FullPathToExe = L".\\plugins\\gtagfornplus\\global.exe";
	file_list_dialog.getSearchString();
	file_list_dialog.Parameters = L" -a --result=grep ";
	file_list_dialog.Parameters.append(file_list_dialog.SearchString);
	file_list_dialog.getCurrentDir();
	file_list_dialog.IsDefSearch = TRUE;
	file_list_dialog.Search();
}

void FindRefr(){
	file_list_dialog.IsSymbolSearch = FALSE;
	file_list_dialog.FullPathToExe = L".\\plugins\\gtagfornplus\\global.exe";
	file_list_dialog.getSearchString();
	file_list_dialog.Parameters = L" -ar ";
	file_list_dialog.Parameters.append(file_list_dialog.SearchString);
	file_list_dialog.getCurrentDir();
	file_list_dialog.IsDefSearch = FALSE;
	file_list_dialog.Search();
	if(file_list_dialog.gtagSearchResult.GetItemCount()==0)
		file_list_dialog.IsSymbolSearch = TRUE;
	else
		return;
	file_list_dialog.symbol_linenum_list.clear();
	file_list_dialog.symbol_list.clear();
	file_list_dialog.symbol_blocks.clear();
	file_list_dialog.Parameters = L" -axs --result=grep ";
	file_list_dialog.Parameters.append(file_list_dialog.SearchString);
	file_list_dialog.Search();
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	g_hInstance = hModule;
	switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
			funcItems[FILELIST_DOCKABLE_WINDOW_INDEX]._pFunc = ToggleDialog;
			lstrcpy(funcItems[FILELIST_DOCKABLE_WINDOW_INDEX]._itemName, TEXT("View Search Window"));
			//funcItems[FILELIST_DOCKABLE_WINDOW_INDEX]._init2Check = false;
			funcItems[FILELIST_DOCKABLE_WINDOW_INDEX]._pShKey = new ShortcutKey;
			funcItems[FILELIST_DOCKABLE_WINDOW_INDEX]._pShKey->_isAlt = true;
			funcItems[FILELIST_DOCKABLE_WINDOW_INDEX]._pShKey->_isCtrl = true;
			funcItems[FILELIST_DOCKABLE_WINDOW_INDEX]._pShKey->_isShift = false;
			funcItems[FILELIST_DOCKABLE_WINDOW_INDEX]._pShKey->_key = 0x41; //A

			funcItems[FILELIST_FIND_DECL_INDEX]._pFunc = FindDecl;
			lstrcpy(funcItems[FILELIST_FIND_DECL_INDEX]._itemName, TEXT("Find Definition"));
			//funcItems[FILELIST_FIND_DECL_INDEX]._init2Check = false;
			funcItems[FILELIST_FIND_DECL_INDEX]._pShKey = new ShortcutKey;
			funcItems[FILELIST_FIND_DECL_INDEX]._pShKey->_isAlt = true;
			funcItems[FILELIST_FIND_DECL_INDEX]._pShKey->_isCtrl = true;
			funcItems[FILELIST_FIND_DECL_INDEX]._pShKey->_isShift = false;
			funcItems[FILELIST_FIND_DECL_INDEX]._pShKey->_key = 0x42; //A

			funcItems[FILELIST_FIND_REF_INDEX]._pFunc = FindRefr;
			lstrcpy(funcItems[FILELIST_FIND_REF_INDEX]._itemName, TEXT("Find References"));
			//funcItems[FILELIST_FIND_REF_INDEX]._init2Check = false;
			funcItems[FILELIST_FIND_REF_INDEX]._pShKey = new ShortcutKey;
			funcItems[FILELIST_FIND_REF_INDEX]._pShKey->_isAlt = true;
			funcItems[FILELIST_FIND_REF_INDEX]._pShKey->_isCtrl = true;
			funcItems[FILELIST_FIND_REF_INDEX]._pShKey->_isShift = false;
			funcItems[FILELIST_FIND_REF_INDEX]._pShKey->_key = 0x43; //A

		case DLL_THREAD_ATTACH:
		case DLL_THREAD_DETACH:
		case DLL_PROCESS_DETACH:
			break;
	}
	return TRUE;
}

