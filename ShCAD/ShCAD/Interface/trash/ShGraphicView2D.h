
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


#ifndef _SHGRAPHICVIEW2D_H
#define _SHGRAPHICVIEW2D_H

#include "ShGraphicView.h"


class ShGraphicView2D : public ShGraphicView {


public:
	ShGraphicView2D(QWidget *parent = 0);
	~ShGraphicView2D();

	virtual ActionType ChangeCurrentAction(ActionType actionType);

	virtual void update(DrawType drawType = DrawType::DrawAll);

protected:
	virtual void initializeGL();
	virtual void paintGL();
	virtual void resizeGL(int width, int height);

	virtual void mousePressEvent(QMouseEvent *event);
	virtual void mouseMoveEvent(QMouseEvent *event);
	virtual void keyPressEvent(QKeyEvent *event);
	virtual void wheelEvent(QWheelEvent *event);
	virtual void mouseReleaseEvent(QMouseEvent *event);
};

#endif //_SHGRAPHICVIEW2D_H