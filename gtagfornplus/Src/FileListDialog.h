/*
This file is part of MultiClipboard Plugin for Notepad++
Copyright (C) 2009 LoonyChewy

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

#ifndef FILELIST_DIALOG_H
#define FILELIST_DIALOG_H

#include "DockingDlgInterface.h"
#include "FileListDialog.h"
#include "FileListBox.h"
#include <vector>
#include <tchar.h>
#include "PluginInterface.h"
#include "Window.h"


extern HINSTANCE g_hInstance;
extern NppData g_NppData;

class FileListDialog: public DockingDlgInterface
{
public:
	GtagFileListbox gtagSearchResult;
	GtagFileListbox gtagFunctionList;
	FileListDialog();
	~FileListDialog();
	// Not inherited from DockingDlgInterface
	virtual void Init( );
	// Inherited from DockingDlgInterface
	virtual void destroy() {}

	void ShowDialog( bool Show = TRUE );
	void DoDeclSearch();
	void DoRefSearch();

	void Search(int SearchType=GTAG_FILES);
protected:
	// Overload DockingDlgInterface's dialog procedure
	virtual BOOL CALLBACK run_dlgProc( UINT msg, WPARAM wp, LPARAM lp );
	void InitialiseDialog();
	void ReadFromPipe(HANDLE g_hChildStd_OUT_Rd,int SearchType=GTAG_FILES);

private:
	bool IsShown;
	tTbData TBData;
	int SearchStringSize;
	void OpenFileAndGotoLine(const wchar_t *,int);
	std::wstring fileName;
	float WindowRatio;
	std::wstring SearchString;
	std::wstring FullPathToExe;
	std::wstring Parameters;
	std::wstring Dir;
	BOOL IsDefSearch;
	BOOL IsSymbolSearch;
	std::vector<int> linenum_list;
	std::vector<int> final_linenum_list;
	std::vector<int> ctag_linenum_list;
	std::vector<std::wstring> ctag_func_list;
	std::vector<int> symbol_linenum_list;
	std::vector<int> symbol_blocks;
	std::vector<std::wstring> symbol_list;
	int file_name_length;
	void getCurrentDir();
	void getSearchString();
	int  oldx ;
	BOOL fMoved;
	BOOL fDragMode;	


	void DrawXorBar(HDC hdc, int x1, int y1, int width, int height);
	LRESULT Splitter_OnLButtonDown(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam);
	LRESULT Splitter_OnLButtonUp(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam);
	LRESULT Splitter_OnMouseMove(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam);
	void ResizeListBoxes();
	void OnFileListDoubleClicked();
};


#endif FILELIST