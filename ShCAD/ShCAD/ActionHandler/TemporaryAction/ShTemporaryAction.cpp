
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

#include "ShTemporaryAction.h"
ShTemporaryAction::ShTemporaryAction(ShGraphicView *graphicView)
	:ShActionHandler(graphicView), previousAction(0){

}

ShTemporaryAction::ShTemporaryAction(ShGraphicView *graphicView, ShActionHandler *previousAction)
	: ShActionHandler(graphicView), previousAction(previousAction) {


}

ShTemporaryAction::~ShTemporaryAction() {

	if (this->previousAction != NULL)
		delete this->previousAction;

}

#include "ShNotifyEvent.h"
#include "ActionHandler\ShActionHandlerManager.h"
void ShTemporaryAction::ReturnToPrevious() {
	qDebug("ShTemporaryAction->ReturnToPrevious");
	
	this->graphicView->actionHandlerManager->ReplaceAction(this->previousAction);
	this->graphicView->setCursor(this->graphicView->actionHandlerManager->GetCursorShape());

	ShCurrentActionChangedEvent event2(this->graphicView->actionHandlerManager->GetType());
	this->graphicView->Notify(&event2);

	this->previousAction = NULL;

	delete this;
}