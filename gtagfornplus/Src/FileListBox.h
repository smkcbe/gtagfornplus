/*
This file is part of MultiClipboard Plugin for Notepad++
Copyright (C) 2009 LoonyChewy

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either
version 2 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

#ifndef FILE_LISBOX_H
#define FILE_LISBOX_H


#include "Window.h"
#include <string>
#include <resource.h>


class GtagFileListbox : public Window
{
public:
	virtual void init(HINSTANCE hInst, HWND parent);
	virtual void destroy();

	virtual void AddItem( std::wstring item );
	virtual void ClearAll();
	virtual INT GetItemCount();
	virtual INT GetCurrentSelectionIndex();
	int GetCurrentSelection(std::wstring &, BOOL Truncate=TRUE);
	void SetTTText();
	//bool IsSelChange();
private:
	HFONT hNewFont;
	//int selection;
};


#endif