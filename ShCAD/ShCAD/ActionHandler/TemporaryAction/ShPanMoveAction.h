
/*--
**
**   This file is part of the ShCAD project, a 2D CAD Program
**
**    Copyright (C) 2019, Seungho Ha  (sh.ha1018@gmail.com)
**
**
**   This program is free software; you can redistribute it and/or modify it
**   under the terms of the GNU Lesser General Public License as published by
**   the Free Software Foundation; either version 2.1 of the License, or (at
**   your option) any later version.
**
**   This program is distributed in the hope that it will be useful, but
**   WITHOUT ANY WARRANTY; without even the implied warranty of
**   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser
**   General Public License for more details.
**
**   You should have received a copy of the GNU Lesser General Public License
**   along with this program; if not, write to the Free Software Foundation,
**   Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
**
**
--*/


#ifndef _SHPANMOVEACTION_H
#define _SHPANMOVEACTION_H

#include "ShTemporaryAction.h"

class ShPanMoveAction : public ShTemporaryAction {

private:
	double prevX;
	double prevY;

public:
	ShPanMoveAction(ShGraphicView *graphicView);
	ShPanMoveAction(ShGraphicView *graphicView, ShActionHandler *previousAction);
	~ShPanMoveAction();
	
	virtual void LMousePressEvent(QMouseEvent *event, ShActionData &data);
	virtual void MMousePressEvent(QMouseEvent *event, ShActionData& data);
	virtual void MouseMoveEvent(QMouseEvent *event, ShActionData& data);
	virtual void KeyPressEvent(QKeyEvent *event, ShActionData& data);
	virtual void MouseReleaseEvent(QMouseEvent *event, ShActionData& data);

	virtual ActionType GetType();

	virtual QCursor GetCursorShape();


};

#endif //_SHPANMOVEACTION_H