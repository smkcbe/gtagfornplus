/*
This file is part of GTAGFORNPLUS Plugin for Notepad++
Copyright (C) 20011 Mohan Kumar S

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#include "stdafx.h"
#include "PluginInterface.h"
#include "FileListDialog.h"
#include <string>
#include <vector>


HINSTANCE			g_hInstance = NULL;
NppData				g_NppData;
FileListDialog	file_list_dialog;

const TCHAR PLUGIN_NAME[] = TEXT("Gtag Search");
const int NUM_FUNCS = 6;

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
	file_list_dialog.DoDeclSearch();
}

void FindRefr(){
	file_list_dialog.DoRefSearch();
}

void GenerateTags(){
	//file_list_dialog.gtagSearchResult.SetFirstItemSelected();
}

void IncreaseFont(){
	file_list_dialog.IncFont();
}

void DecreaseFont(){
	file_list_dialog.DecFont();
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

			funcItems[FILELIST_FIND_DECL_INDEX]._pFunc = FindDecl;
			lstrcpy(funcItems[FILELIST_FIND_DECL_INDEX]._itemName, TEXT("Find Definition"));

			funcItems[FILELIST_FIND_REF_INDEX]._pFunc = FindRefr;
			lstrcpy(funcItems[FILELIST_FIND_REF_INDEX]._itemName, TEXT("Find References"));

			funcItems[FILELIST_GEN_TAG_INDEX]._pFunc = GenerateTags;
			lstrcpy(funcItems[FILELIST_GEN_TAG_INDEX]._itemName, TEXT("Generate Tag DB"));
 
			funcItems[FILELIST_INC_FONT_INDEX]._pFunc = IncreaseFont;
			lstrcpy(funcItems[FILELIST_INC_FONT_INDEX]._itemName, TEXT("Increase Dialog Font Size"));

			funcItems[FILELIST_DEC_FONT_INDEX]._pFunc = DecreaseFont;
			lstrcpy(funcItems[FILELIST_DEC_FONT_INDEX]._itemName, TEXT("Decrease Dialog Font Size"));

		case DLL_THREAD_ATTACH:
		case DLL_THREAD_DETACH:
		case DLL_PROCESS_DETACH:
			break;
	}
	return TRUE;
}

