
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

#ifndef _SHMOVEVIEWCOMMAND_H
#define _SHMOVEVIEWCOMMAND_H

#include "Command Pattern\ShCommand.h"

class ShGraphicView;
class ShMoveViewCommand : public ShCommand {

private:
	ShGraphicView *view;
	double ex;
	double ey;
	double zoomRate;
	int dx;
	int dy;

public:
	ShMoveViewCommand(ShGraphicView *view, double ex, double ey, double zoomRate, int dx, int dy);
	virtual void Execute();
	virtual void UnExecute();

protected:
	~ShMoveViewCommand();



};

#endif //_SHMOVEVIEWCOMMAND_H