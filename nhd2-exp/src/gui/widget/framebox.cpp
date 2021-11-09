/*
	$Id: framebox.cpp 09.02.2019 mohousch Exp $


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

#include <global.h>
#include <neutrino.h>

#include <gui/widget/framebox.h>
#include <gui/widget/textbox.h>

#include <gui/pluginlist.h>

#include <system/settings.h>
#include <system/debug.h>

#include <video_cs.h>


extern CPlugins * g_PluginList;    // defined in neutrino.cpp
extern cVideo * videoDecoder;

// CFrame
CFrame::CFrame()
{
	captionFont = g_Font[SNeutrinoSettings::FONT_TYPE_EPG_INFO2];
	optionFont = g_Font[SNeutrinoSettings::FONT_TYPE_EPG_INFO1];

	caption = "";
	mode = FRAME_BOX;

	shadow = true;
	paintFrame = true;
	pluginOrigName = false;

	iconName.clear();
	option.clear();

	jumpTarget = NULL;
	actionKey.clear();
	
	//
	fcolor = COL_MENUCONTENT_PLUS_0;
	radius = RADIUS_MID;
	corner = NO_RADIUS;
	gradient = NOGRADIENT;

	// headFrame
	headColor = COL_MENUHEAD_PLUS_0;
	headRadius = RADIUS_MID;
	headCorner = CORNER_TOP;
	headGradient = g_settings.Head_gradient;
	paintDate = false;
	logo = false;
	hbutton_count	= 0;
	hbutton_labels.clear();

	// footFrame
	footColor = COL_MENUFOOT_PLUS_0;
	footRadius = RADIUS_MID;
	footCorner = CORNER_BOTTOM;
	footGradient = g_settings.Foot_gradient;
	fbutton_count	= 0;
	fbutton_labels.clear();
	//fbutton_width = cFrameBox.iWidth;

	// init
	window.setPosition(-1, -1, -1, -1);
	
	// 
	active = true;
	marked = false;
	
	//
	parent = NULL;	
}

void CFrame::setMode(int m)
{
	mode = m;
			
	if ( (mode == FRAME_HLINE) || (mode == FRAME_VLINE) ) 
	{
		shadow = false;
		paintFrame = false;
		
		setActive(false);
	}
	
	if ( (mode == FRAME_HEAD) || (mode == FRAME_FOOT) ) 
	{
		setActive(false);
	}
}

void CFrame::setActive(const bool Active)
{
	active = Active;
	
	//if (window.getWindowsPos().iX != -1)
	//	paint();
}

void CFrame::setMarked(const bool Marked)
{
	marked = Marked;
	
	//if (window.getWindowsPos().iX != -1)
	//	paint();
}

void CFrame::setPlugin(const char * const pluginName)
{
	if (mode == FRAME_PLUGIN)
	{
		if (g_PluginList->plugin_exists(pluginName))
		{
			unsigned int count = g_PluginList->find_plugin(pluginName);

			//iconName
			iconName = NEUTRINO_ICON_MENUITEM_PLUGIN;

			std::string icon("");
			icon = g_PluginList->getIcon(count);

			if(!icon.empty())
			{
					iconName = PLUGINDIR;
					iconName += "/";
					iconName += g_PluginList->getFileName(count);
					iconName += "/";
					iconName += g_PluginList->getIcon(count);
			}

			// caption
			if (caption.empty())
				caption = g_PluginList->getName(count);

			// option
			if (option.empty())
				option = g_PluginList->getDescription(count);

			// jumpTarget
			jumpTarget = CPluginsExec::getInstance();

			// actionKey
			actionKey = to_string(count).c_str();
		}
	}
}

void CFrame::setHeaderButtons(const struct button_label *_hbutton_labels, const int _hbutton_count)
{
	if (mode == FRAME_HEAD)
	{
		if (_hbutton_count)
		{
			for (unsigned int i = 0; i < _hbutton_count; i++)
			{
				hbutton_labels.push_back(_hbutton_labels[i]);
			}
		}

		hbutton_count = hbutton_labels.size();
	}
}

void CFrame::setFooterButtons(const struct button_label* _fbutton_labels, const int _fbutton_count)
{
	if (mode == FRAME_FOOT)
	{
		if (_fbutton_count)
		{
			for (unsigned int i = 0; i < _fbutton_count; i++)
			{
				fbutton_labels.push_back(_fbutton_labels[i]);
			}
		}

		fbutton_count = fbutton_labels.size();	
	}
}

int CFrame::paint(bool selected, bool /*AfterPulldown*/)
{
	dprintf(DEBUG_DEBUG, "CFrame::paint:\n");

	uint8_t color = COL_MENUCONTENT;
	fb_pixel_t bgcolor = marked? COL_MENUCONTENTSELECTED_PLUS_2 : fcolor;

	if (selected)
	{
		color = COL_MENUCONTENTSELECTED;
		
		if (parent)
		{
			bgcolor = parent->inFocus? COL_MENUCONTENTSELECTED_PLUS_0 : COL_MENUCONTENTSELECTED_PLUS_1;
		}
		else
			bgcolor = COL_MENUCONTENTSELECTED_PLUS_0;
	}
	else if (!active)
	{
		//color = COL_MENUCONTENTINACTIVE;
		//bgcolor = COL_MENUCONTENTINACTIVE_PLUS_0;
	}

	// paint frameBackground
	if (paintFrame)
	{
		if (shadow)
		{
			window.enableShadow();
		}
	
		window.setColor(bgcolor);
		//window.setCorner(radius, corner); //FIXME:
		window.setGradient(gradient);
		
		window.paint();
	}

	// icon
	int iw = 0;
	int ih = 0;
	int iconOffset = 0;

	if (mode == FRAME_BOX)
	{
		// icon
		if(!iconName.empty())
		{
			iconOffset = ICON_OFFSET;

			CFrameBuffer::getInstance()->getIconSize(iconName.c_str(), &iw, &ih);

			CFrameBuffer::getInstance()->paintIcon(iconName, window.getWindowsPos().iX + ICON_OFFSET, window.getWindowsPos().iY + (window.getWindowsPos().iHeight - ih)/2);
		}

		// caption
		if(!option.empty())
		{
			// caption
			if(!caption.empty())
			{
				int c_w = captionFont->getRenderWidth(caption);

				captionFont->RenderString(window.getWindowsPos().iX + BORDER_LEFT + iconOffset + iw + 2, window.getWindowsPos().iY + 3 + captionFont->getHeight(), window.getWindowsPos().iWidth - BORDER_LEFT - BORDER_RIGHT - iconOffset - iw, caption.c_str(), color, 0, true); //
			}

			// option
			if(!option.empty())
			{
				int o_w = optionFont->getRenderWidth(option);

				optionFont->RenderString(window.getWindowsPos().iX + BORDER_LEFT + iconOffset + iw + 2, window.getWindowsPos().iY + window.getWindowsPos().iHeight, window.getWindowsPos().iWidth - BORDER_LEFT - BORDER_RIGHT - iconOffset - iw, option.c_str(), color, 0, true);
			}
		}
		else
		{
			if(!caption.empty())
			{
				int c_w = captionFont->getRenderWidth(caption);

				captionFont->RenderString(window.getWindowsPos().iX + BORDER_LEFT + iconOffset + iw + 2, window.getWindowsPos().iY + (window.getWindowsPos().iHeight - g_Font[SNeutrinoSettings::FONT_TYPE_MENU]->getHeight())/2 + captionFont->getHeight(), window.getWindowsPos().iWidth - BORDER_LEFT - BORDER_RIGHT - iconOffset - iw, caption.c_str(), color);
			}
		}
	}
	else if (mode == FRAME_PICTURE)
	{
		int c_h = 0;
		int stringStartPosX = 0;
		int c_w = 0;

		if(!caption.empty())
			c_h = captionFont->getHeight() + 20;

		// refresh
		window.setColor(bgcolor);
		window.paint();

		if(!iconName.empty())
		{
			CFrameBuffer::getInstance()->displayImage(iconName, window.getWindowsPos().iX + 10, window.getWindowsPos().iY + 10, window.getWindowsPos().iWidth - 20, window.getWindowsPos().iHeight - c_h - 20);
			
			if (!active)
			{
				CFrameBuffer::getInstance()->displayImage(iconName, window.getWindowsPos().iX, window.getWindowsPos().iY, window.getWindowsPos().iWidth, window.getWindowsPos().iHeight - c_h);
			}
		}

		if(!caption.empty())
		{
			c_w = captionFont->getRenderWidth(caption);
			
			if (c_w > window.getWindowsPos().iWidth)
				c_w = window.getWindowsPos().iWidth;
				
			stringStartPosX = window.getWindowsPos().iX + (window.getWindowsPos().iWidth >> 1) - (c_w >> 1);

			captionFont->RenderString(stringStartPosX, window.getWindowsPos().iY + window.getWindowsPos().iHeight - 5, window.getWindowsPos().iWidth - 6, caption.c_str(), color);
		}

		if (selected)
		{
			// refresh
			window.setColor(bgcolor);
			window.paint();

			if(!iconName.empty())
			{
				CFrameBuffer::getInstance()->displayImage(iconName, window.getWindowsPos().iX + 2, window.getWindowsPos().iY + 2, window.getWindowsPos().iWidth - 4, window.getWindowsPos().iHeight - c_h - 4);
			}

			if(!caption.empty())
			{
				c_w = captionFont->getRenderWidth(caption);
				
				if (c_w > window.getWindowsPos().iWidth)
					c_w = window.getWindowsPos().iWidth;
					
				stringStartPosX = window.getWindowsPos().iX + (window.getWindowsPos().iWidth >> 1) - (c_w >> 1);

				captionFont->RenderString(stringStartPosX, window.getWindowsPos().iY + window.getWindowsPos().iHeight, window.getWindowsPos().iWidth - 6, caption.c_str(), color);
			}
		}
	}
	else if (mode == FRAME_ICON)
	{
		iconOffset = ICON_OFFSET;

		if (!active)
			iconOffset = 0;

		// iconName
		if(caption.empty())
		{
			if(!iconName.empty())
			{
				CFrameBuffer::getInstance()->getIconSize(iconName.c_str(), &iw, &ih);

				CFrameBuffer::getInstance()->paintIcon(iconName, window.getWindowsPos().iX + (window.getWindowsPos().iWidth - iw)/2, window.getWindowsPos().iY + (window.getWindowsPos().iHeight - ih)/2);
			}
		}
		else
		{
			// iconName
			if(!iconName.empty())
			{
				CFrameBuffer::getInstance()->getIconSize(iconName.c_str(), &iw, &ih);

				CFrameBuffer::getInstance()->paintIcon(iconName, window.getWindowsPos().iX + iconOffset, window.getWindowsPos().iY + (window.getWindowsPos().iHeight - ih)/2);
			}		

			// caption
			int c_w = captionFont->getRenderWidth(caption);

			captionFont->RenderString(window.getWindowsPos().iX + iconOffset + iw + iconOffset, window.getWindowsPos().iY + optionFont->getHeight() + (window.getWindowsPos().iHeight - optionFont->getHeight())/2, window.getWindowsPos().iWidth - iconOffset - iw - iconOffset, caption.c_str(), color, 0, true); //
		}
	}
	else if (mode == FRAME_TEXT)
	{
		CTextBox textBox;

		textBox.setPosition(window.getWindowsPos().iX + 1, window.getWindowsPos().iY + 1, window.getWindowsPos().iWidth - 2, window.getWindowsPos().iHeight - 2);

		textBox.disablePaintFrame();
		textBox.setMode(AUTO_WIDTH);
		textBox.setFontText(captionFont);

		// caption
		if(!caption.empty())
		{
			textBox.setText(caption.c_str());
		}
		
		textBox.paint();
	}
	else if (mode == FRAME_PLUGIN)
	{
		int c_h = 0;

		if(!caption.empty() && pluginOrigName)
			c_h = captionFont->getHeight() + 20;

		// icon
		if(!iconName.empty())
		{
			CFrameBuffer::getInstance()->displayImage(iconName, window.getWindowsPos().iX + 2, window.getWindowsPos().iY + 2, window.getWindowsPos().iWidth - 4, window.getWindowsPos().iHeight - 4 - c_h);
		}

		// caption
		if(!caption.empty() && pluginOrigName)
		{
			int c_w = captionFont->getRenderWidth(caption);

			captionFont->RenderString(window.getWindowsPos().iX + 2, window.getWindowsPos().iY + window.getWindowsPos().iHeight, window.getWindowsPos().iWidth - 4, caption.c_str(), color);
		}
	}
	else if (mode == FRAME_VLINE)
	{
		CFrameBuffer::getInstance()->paintVLineRel(window.getWindowsPos().iX, window.getWindowsPos().iY, window.getWindowsPos().iHeight, COL_MENUCONTENTDARK_PLUS_0);
	}
	else if (mode == FRAME_HLINE)
	{
		CFrameBuffer::getInstance()->paintHLineRel(window.getWindowsPos().iX, window.getWindowsPos().iWidth, window.getWindowsPos().iY, COL_MENUCONTENTDARK_PLUS_0);
	}
	else if (mode == FRAME_LABEL)
	{
		if(!caption.empty())
		{
			captionFont->RenderString(window.getWindowsPos().iX + 2, window.getWindowsPos().iY + window.getWindowsPos().iHeight, window.getWindowsPos().iWidth - 4, caption.c_str(), color);
		}
	}
	else if (mode == FRAME_HEAD)
	{
		CHeaders headers(window.getWindowsPos(), caption.c_str(), iconName.c_str());

		headers.setColor(headColor);
		headers.setCorner(headRadius, headCorner);
		headers.setGradient(headGradient);
		
		if(paintDate)
			headers.enablePaintDate();

		if(logo)
			headers.enableLogo();

		for (int i = 0; i < hbutton_count; i++)
		{			
			headers.setButtons(&hbutton_labels[i]);
		}

		headers.paint();
	}
	else if ( mode == FRAME_FOOT)
	{
		CFooters footers(window.getWindowsPos());

		footers.setColor(footColor);
		footers.setCorner(footRadius, footCorner);
		footers.setGradient(footGradient);

		for (int i = 0; i < fbutton_count; i++)
		{			
			footers.setButtons(&fbutton_labels[i]);
		}

		footers.paint();
	}	
	else if (mode == FRAME_PIG)
	{
		//window.paint();
		CFrameBuffer::getInstance()->paintBackgroundBoxRel(window.getWindowsPos().iX, window.getWindowsPos().iY, window.getWindowsPos().iWidth, window.getWindowsPos().iHeight);	
		

		if(videoDecoder)
			videoDecoder->Pig(window.getWindowsPos().iX, window.getWindowsPos().iY, window.getWindowsPos().iWidth, window.getWindowsPos().iHeight);
		
	}

	return 0;
}

int CFrame::exec(CMenuTarget *parent)
{
	dprintf(DEBUG_NORMAL, "CFrame::exec: actionKey:(%s)\n", actionKey.c_str());

	int ret = RETURN_EXIT;

	if(jumpTarget)
		ret = jumpTarget->exec(parent, actionKey);
	else
		ret = RETURN_EXIT;

	return ret;
}

// CFrameBox
CFrameBox::CFrameBox(const int x, int const y, const int dx, const int dy)
{
	dprintf(DEBUG_NORMAL, "CFrameBox::CFrameBox:\n");

	frameBuffer = CFrameBuffer::getInstance();

	itemBox.iX = x;
	itemBox.iY = y;
	itemBox.iWidth = dx;
	itemBox.iHeight = dy;

	selected = -1;
	pos = 0;

	inFocus = true;

	itemType = WIDGET_ITEM_FRAMEBOX;
	paintFrame = true;

	actionKey = "";
	
	//
	timeout = 0;
	exit_pressed = false;
	
	//
	bgcolor = COL_MENUCONTENT_PLUS_0;
	radius = RADIUS_MID;
	corner = NO_RADIUS;
	shadow = false;
	screen = false;
	
	// init
	//initFrames();
}

CFrameBox::CFrameBox(CBox* position)
{
	dprintf(DEBUG_NORMAL, "CFrameBox::CFrameBox:\n");

	frameBuffer = CFrameBuffer::getInstance();

	itemBox = *position;

	selected = -1;
	pos = 0;

	inFocus = true;

	itemType = WIDGET_ITEM_FRAMEBOX;
	paintFrame = true;

	actionKey = "";
	
	//
	timeout = 0;
	exit_pressed = false;
	
	//
	bgcolor = COL_MENUCONTENT_PLUS_0;
	radius = RADIUS_MID;
	corner = NO_RADIUS;
	shadow = false;
	screen = false;
	
	// init
	//initFrames();
}

CFrameBox::~CFrameBox()
{
	frames.clear();
}

void CFrameBox::addFrame(CFrame *frame, const bool defaultselected)
{
	if (defaultselected)
		selected = frames.size();
	
	frames.push_back(frame);
	frame->setParent(this);
}

bool CFrameBox::hasItem()
{
	return !frames.empty();
}

void CFrameBox::initFrames()
{
	dprintf(DEBUG_NORMAL, "CFrameBox::initFrames:\n");
	
	cFrameWindow.setPosition(&itemBox);
}

void CFrameBox::paintFrames()
{
	dprintf(DEBUG_NORMAL, "CFrameBox::paintFrames:\n");

	for (unsigned int count = 0; count < (unsigned int)frames.size(); count++) 
	{
		CFrame *frame = frames[count];

		// init frame

		//
		if((frame->isSelectable()) && (selected == -1)) 
		{
			selected = count;
		} 

		//if(inFocus)
			frame->paint( selected == ((signed int) count));
		//else
		//	frame->paint(false);
	}
}

void CFrameBox::paint()
{
	dprintf(DEBUG_NORMAL, "CFrameBox::paint:\n");
	
	//
	initFrames();

	//
	if (paintFrame)
	{
		cFrameWindow.setColor(bgcolor);
		cFrameWindow.setCorner(radius, corner);
		
		if (shadow)
			cFrameWindow.enableShadow();
			
		if (screen)
			cFrameWindow.enableSaveScreen();

		cFrameWindow.paint();
	}

	paintFrames();
}

void CFrameBox::hide()
{
	dprintf(DEBUG_NORMAL, "CFrameBox::hide:\n");
	
	if (hasItem())
	{
		for (int i = 0; i < frames.size(); i++)
		{
			if (frames[i]->getMode() == FRAME_PIG)
			{
				if(videoDecoder)  
					videoDecoder->Pig(-1, -1, -1, -1);
					
				CFrameBuffer::getInstance()->paintBackgroundBoxRel(frames[i]->window.getWindowsPos().iX, frames[i]->window.getWindowsPos().iY, frames[i]->window.getWindowsPos().iWidth, frames[i]->window.getWindowsPos().iHeight);
			}
		}
	}

	cFrameWindow.hide();
	
	frameBuffer->blit();
}

bool CFrameBox::isSelectable(void)
{
	if (hasItem())
	{
		for (int i = 0; i < frames.size(); i++)
		{
			if (frames[i]->isSelectable())
				return true;
		}
	}

	return false;
}

void CFrameBox::swipRight()
{
	dprintf(DEBUG_NORMAL, "CFrameBox::swipRight:\n");

	for (unsigned int count = 1; count < frames.size(); count++) 
	{
		pos = (selected + count)%frames.size();

		CFrame * frame = frames[pos];

		if(frame->isSelectable())
		{
			frames[selected]->paint(false);
			frame->paint(true);

			selected = pos;
				
			break;
		}
	}
}

void CFrameBox::swipLeft()
{
	dprintf(DEBUG_NORMAL, "CFrameBox::swipLeft:\n");

	for (unsigned int count = 1; count < frames.size(); count++) 
	{
		pos = selected - count;
		if ( pos < 0 )
			pos += frames.size();

		CFrame * frame = frames[pos];

		if(frame->isSelectable())
		{
			frames[selected]->paint(false);
			frame->paint(true);

			selected = pos;

			break;
		}
	}
}

void CFrameBox::scrollLineDown(const int lines)
{
	dprintf(DEBUG_NORMAL, "CFrameBox::scrollLineDown:\n");

	for (unsigned int count = 1; count < frames.size(); count++) 
	{
		pos = (selected + count)%frames.size();

		CFrame * frame = frames[pos];

		if(frame->isSelectable())
		{
			frames[selected]->paint(false);
			frame->paint(true);

			selected = pos;

			break;
		}
	}
}

void CFrameBox::scrollLineUp(const int lines)
{
	dprintf(DEBUG_NORMAL, "CFrameBox::scrollLineUp:\n");

	for (unsigned int count = 1; count < frames.size(); count++) 
	{
		pos = selected - count;
		if ( pos < 0 )
			pos += frames.size();

		CFrame * frame = frames[pos];

		if(frame->isSelectable())
		{
			frames[selected]->paint(false);
			frame->paint(true);

			selected = pos;

			break;
		}
	}
}

int CFrameBox::oKKeyPressed(CMenuTarget *parent)
{
	dprintf(DEBUG_NORMAL, "CFrameBox::okKeyPressed:\n");

	if (hasItem() && selected >= 0 && frames[selected]->isSelectable())
		actionKey = frames[selected]->actionKey;

	if(parent)
	{
		if (hasItem() && selected >= 0 && frames[selected]->isSelectable())
		{
			return frames[selected]->exec(parent);
		}
		else
			return RETURN_EXIT;
	}
	else
		return RETURN_EXIT;
}

void CFrameBox::onHomeKeyPressed()
{
	dprintf(DEBUG_NORMAL, "CFrameBox::HomeKeyPressed:\n");

	selected = -1;
}

void CFrameBox::onUpKeyPressed()
{
	dprintf(DEBUG_DEBUG, "CFrameBox::UpKeyPressed:\n");

	scrollLineUp();
}

void CFrameBox::onDownKeyPressed()
{
	dprintf(DEBUG_DEBUG, "CFrameBox::DownKeyPressed:\n");

	scrollLineDown();
}

void CFrameBox::onRightKeyPressed()
{
	dprintf(DEBUG_DEBUG, "CFrameBox::RightKeyPressed:\n");

	swipRight();
}

void CFrameBox::onLeftKeyPressed()
{
	dprintf(DEBUG_DEBUG, "CFrameBox::LeftKeyPressed:\n");

	swipLeft();
}

void CFrameBox::onPageUpKeyPressed()
{
	dprintf(DEBUG_DEBUG, "CFrameBox::PageUpKeyPressed:\n");

	//scrollPageUp();
}

void CFrameBox::onPageDownKeyPressed()
{
	dprintf(DEBUG_DEBUG, "CFrameBox::PageDownKeyPressed:\n");

	//scrollPageDown();
}

//
void CFrameBox::addKey(neutrino_msg_t key, CMenuTarget *menue, const std::string & action)
{
	keyActionMap[key].menue = menue;
	keyActionMap[key].action = action;
}

int CFrameBox::exec(CMenuTarget* parent, const std::string&)
{
	dprintf(DEBUG_NORMAL, "CFrameBox::exec:\n");

	int retval = RETURN_REPAINT;
	
	if (parent)
		parent->hide();
	
	paint();
	CFrameBuffer::getInstance()->blit();
	
	// control loop
	uint64_t timeoutEnd = CRCInput::calcTimeoutEnd(timeout == 0 ? 0xFFFF : timeout);

	//control loop
	do {
		g_RCInput->getMsgAbsoluteTimeout(&msg, &data, &timeoutEnd);
		
		int handled = false;

		dprintf(DEBUG_DEBUG, "CFrameBox::exec: msg:%s\n", CRCInput::getSpecialKeyName(msg));
		
		if ( msg <= RC_MaxRC ) 
		{
			timeoutEnd = CRCInput::calcTimeoutEnd(timeout == 0 ? 0xFFFF : timeout);
			
			// keymap
			std::map<neutrino_msg_t, keyAction>::iterator it = keyActionMap.find(msg);
			
			if (it != keyActionMap.end()) 
			{
				actionKey = it->second.action;

				if (it->second.menue != NULL)
				{
					int rv = it->second.menue->exec(this, it->second.action);

					//FIXME:review this
					switch ( rv ) 
					{
						case RETURN_EXIT_ALL:
							retval = RETURN_EXIT_ALL; //fall through
						case RETURN_EXIT:
							msg = RC_timeout;
							break;
						case RETURN_REPAINT:
							hide();
							paint();
							break;
					}
				}
				else
				{
					selected = -1;
					handled = true;

					break;
				}

				continue;
			}
			
			// direktKey
			for (unsigned int i = 0; i < frames.size(); i++) 
			{
				CFrame * titem = frames[i];
			
				if ((titem->directKey != RC_nokey) && (titem->directKey == msg)) 
				{
					if (titem->isSelectable()) 
					{
						frames[selected]->paint(false);
						selected = i;

						/*
						if (selected > (int)page_start[current_page + 1] || selected < (int)page_start[current_page]) 
						{
							// different page
							paintFrames();
						}
						*/
						
						pos = selected;
						msg = RC_ok;
						actionKey = titem->actionKey;
					} 
					else 
					{
						// swallow-key...
						handled = true;
					}
					break;
				}
			}
		}
		
		if (!handled)
		{
			switch (msg)
			{
				//
				case (RC_up):
					onUpKeyPressed();
					break;

				case (RC_down):
					onDownKeyPressed();
					break;

				case (RC_right):
					onRightKeyPressed();
					break;

				case (RC_left):
					onLeftKeyPressed();
					break;

				case (RC_page_up):
					onPageUpKeyPressed();
					break;

				case (RC_page_down):
					onPageDownKeyPressed();
					break;

				case (RC_home):
					exit_pressed = true;
					dprintf(DEBUG_NORMAL, "CFrameBox::exec: exit_pressed\n");
					msg = RC_timeout;
					selected = -1; 
					break;

				case (RC_ok):
					{
						if (hasItem())
						{
							CFrame* item = frames[selected];
							item->msg = msg;
							actionKey = item->actionKey;
							
							int rv = item->exec(this);
							
							//FIXME:review this
							switch ( rv ) 
							{
								case RETURN_EXIT_ALL:
									retval = RETURN_EXIT_ALL;
								case RETURN_EXIT:
									msg = RC_timeout;
									break;
								case RETURN_REPAINT:
									hide();
									paint();
									break;
							}
						}
						else
							msg = RC_timeout;
					}
					break;
					
				case (RC_timeout):
					exit_pressed = true;
					selected = -1;
					break;

				default:
					if ( CNeutrinoApp::getInstance()->handleMsg( msg, data ) & messages_return::cancel_all ) 
					{
						retval = RETURN_EXIT_ALL;
						msg = RC_timeout;
					}
			}
			
			if ( msg <= RC_MaxRC )
			{
				// recalculate timeout for RC-Tasten
				timeoutEnd = CRCInput::calcTimeoutEnd(timeout == 0 ? 0xFFFF : timeout);
			}
		}
		
		frameBuffer->blit();
	}while ( msg != RC_timeout );	
	
	hide();
	
	// vfd
	if(!parent)
	{
		if(CNeutrinoApp::getInstance()->getMode() == NeutrinoMessages::mode_webtv)
			CVFD::getInstance()->setMode(CVFD::MODE_WEBTV);
		else
			CVFD::getInstance()->setMode(CVFD::MODE_TVRADIO);
	}
	
	return retval;
}


