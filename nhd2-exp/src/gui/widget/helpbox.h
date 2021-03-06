/*
	Neutrino-GUI  -   DBoxII-Project

	Copyright (C) 2001 Steffen Hehn 'McClean'
	Homepage: http://dbox.cyberphoria.org/

	Kommentar:

	Diese GUI wurde von Grund auf neu programmiert und sollte nun vom
	Aufbau und auch den Ausbaumoeglichkeiten gut aussehen. Neutrino basiert
	auf der Client-Server Idee, diese GUI ist also von der direkten DBox-
	Steuerung getrennt. Diese wird dann von Daemons uebernommen.


	License: GPL

	This program is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation; either version 2 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/


#ifndef __helpbox__
#define __helpbox__

#include <string>
#include <vector>

#include <system/localize.h>
#include <system/settings.h>

#include <gui/widget/drawable.h>
#include <gui/widget/messagebox.h>


#define HELPBOX_WIDTH			650

class CHelpBox
{
	private:
		ContentLines m_lines;
		
	public:
		CHelpBox();
		~CHelpBox();

		//
		void addLine(std::string& text, CFont* font = g_Font[SNeutrinoSettings::FONT_TYPE_MENU], uint8_t col = COL_MENUCONTENT, const bool bg = false);
		void addLine(const char* const text, CFont* font = g_Font[SNeutrinoSettings::FONT_TYPE_MENU], uint8_t col = COL_MENUCONTENT, const bool bg = false);
		void addLine(std::string& icon, std::string& text, CFont* font = g_Font[SNeutrinoSettings::FONT_TYPE_MENU], uint8_t col = COL_MENUCONTENT, const bool bg = false);
		void addLine(const char* const icon, const char* const text, CFont* font = g_Font[SNeutrinoSettings::FONT_TYPE_MENU], uint8_t col = COL_MENUCONTENT, const bool bg = false);
		void add2Line(const char* const text1, const char* const text2, CFont* font1 = g_Font[SNeutrinoSettings::FONT_TYPE_MENU], uint8_t col1 = COL_MENUCONTENT, const bool bg1 = false, CFont* font2 = g_Font[SNeutrinoSettings::FONT_TYPE_MENU], uint8_t col2 = COL_MENUCONTENT, const bool bg2 = false);
		void addSeparator();
		void addPagebreak();
		
		//
		void show(const char* const Caption, const int Width = HELPBOX_WIDTH, int timeout = -1, const result_ Default = mbrBack, const uint32_t ShowButtons = mbBack);
};

#endif
