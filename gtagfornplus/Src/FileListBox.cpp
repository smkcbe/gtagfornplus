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

#include "stdafx.h"
#include "FileListbox.h"

void GtagFileListbox::init(HINSTANCE hInst, HWND parent)
{
	hNewFont = 0;

	Window::init( hInst, parent );

	_hSelf = CreateWindowEx(0, TEXT("listbox"), NULL,
		WS_CHILD | WS_VISIBLE | WS_BORDER |WS_HSCROLL| LBS_STANDARD ^ LBS_SORT ,
		0,0,0,0, parent, 0, hInst, NULL );

	if ( !_hSelf )
	{
		return;
	}

	hNewFont = (HFONT)::SendMessage( _hSelf, WM_GETFONT, 0, 0 );
	if ( hNewFont == NULL )
	{
		hNewFont = (HFONT)::GetStockObject( SYSTEM_FONT );
	}
	LOGFONT lf;
	::GetObject( hNewFont, sizeof( lf ), &lf );
	lf.lfHeight = 14;
	lf.lfWidth = 0;
	lf.lfWeight = FW_NORMAL;
	lstrcpy( lf.lfFaceName, TEXT("Courier New") );
	hNewFont = ::CreateFontIndirect( &lf );
	::SendMessage( _hSelf, WM_SETFONT, (WPARAM)hNewFont, 1 );
}



void GtagFileListbox::destroy()
{
	::DeleteObject( hNewFont );
}


void GtagFileListbox::AddItem( std::wstring item )
{
	::SendMessage( _hSelf, LB_ADDSTRING, 0, (LPARAM)item.c_str() );
}


void GtagFileListbox::ClearAll()
{
	::SendMessage( _hSelf, LB_RESETCONTENT, 0, 0 );
}


INT GtagFileListbox::GetItemCount()
{
	return (INT)::SendMessage( _hSelf, LB_GETCOUNT, 0, 0 );
}


INT GtagFileListbox::GetCurrentSelectionIndex()
{
	return (INT)::SendMessage( _hSelf, LB_GETCURSEL, 0, 0 );
}

int GtagFileListbox::GetCurrentSelection(std::wstring &content)
{
	int sel = (INT)::SendMessage( _hSelf, LB_GETCURSEL, 0, 0 );
	TCHAR data[1024];
	sel = (int)::SendMessage(_hSelf,LB_GETTEXT ,(WPARAM)sel,(LPARAM)(LPTSTR)data);
	content.clear();
	data[sel-1]=0x0;
	content.append(data,sel);
	return sel;
}