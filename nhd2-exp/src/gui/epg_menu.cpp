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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <global.h>
#include <neutrino.h>
#include <gui/widget/icons.h>

#include <gui/epg_menu.h>

#include <gui/eventlist.h>
#include <gui/infoviewer.h>
#include <gui/epgplus.h>
#include <gui/streaminfo2.h>



//
//  -- EPG Menue Handler Class
//  -- to be used for calls from Menue
//  -- (2004-03-06 rasc)
// 

int CEPGMenuHandler::exec(CMenuTarget* parent, const std::string &)
{
	int res = RETURN_EXIT_ALL;

	if (parent)
		parent->hide();

	doMenu();
	
	return res;
}




int CEPGMenuHandler::doMenu()
{
	//if ( !g_settings.use_default_skin && (CNeutrinoApp::getInstance()->skin_exists("mainmenu")))
	//	CNeutrinoApp::getInstance()->startSkin("red");
	//else	
	{
		CMenuWidget redMenu(LOCALE_EPGMENU_HEAD, NEUTRINO_ICON_BUTTON_EPG);

		redMenu.setWidgetMode(MODE_MENU);
		redMenu.setWidgetType(WIDGET_TYPE_CLASSIC);
		redMenu.setMenuPosition(MENU_POSITION_LEFT);
		redMenu.enableShrinkMenu();
		redMenu.enableSaveScreen();

		// intros
		//redMenu.addItem(new CMenuForwarder(LOCALE_MENU_BACK, true, NULL, NULL, NULL, RC_nokey, NEUTRINO_ICON_BUTTON_LEFT));
		//redMenu.addItem( new CMenuSeparator(LINE) );

		// eventlist
		redMenu.addItem(new CMenuForwarder(LOCALE_EPGMENU_EVENTLIST, true, NULL, new CEventListHandler(), "", RC_red, NEUTRINO_ICON_BUTTON_RED, NEUTRINO_ICON_MENUITEM_SLEEPTIMER));

		// epg view
		redMenu.addItem(new CMenuForwarder(LOCALE_EPGMENU_EVENTINFO, true, NULL, new CEPGDataHandler(), "", RC_green, NEUTRINO_ICON_BUTTON_GREEN, NEUTRINO_ICON_MENUITEM_RESTART));
			
		// epgplus
		redMenu.addItem(new CMenuForwarder(LOCALE_EPGMENU_EPGPLUS, true, NULL, new CEPGplusHandler(), "", RC_yellow, NEUTRINO_ICON_BUTTON_YELLOW, NEUTRINO_ICON_MENUITEM_STANDBY));

		//tech info
		redMenu.addItem(new CMenuForwarder(LOCALE_EPGMENU_STREAMINFO, true, NULL, new CStreamInfo2Handler(), "", RC_blue, NEUTRINO_ICON_BUTTON_BLUE, NEUTRINO_ICON_MENUITEM_TIMERLIST));
					
		return redMenu.exec(NULL, "");
	}
}




