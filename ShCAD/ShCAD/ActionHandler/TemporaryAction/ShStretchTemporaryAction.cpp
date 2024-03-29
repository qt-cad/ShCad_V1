

#include "ShStretchTemporaryAction.h"
#include <QKeyEvent>
#include "ShNotifyEvent.h"
#include "Entity\Composite\ShPreview.h"
#include "Entity\Leaf\ShRubberBand.h"
#include "Interface\ShGraphicView.h"
#include "Visitor Pattern\ShStretchVisitor.h"

ShStretchTemporaryAction::ShStretchTemporaryAction(ShGraphicView *graphicView,
	const QList<ShEntity*>& entitiesToStretch, const QList<ShStretchData*>& stretchDataList, ShPoint3d vertex)
	:ShTemporaryAction(graphicView), entitiesToStretch(entitiesToStretch),
	stretchDataList(stretchDataList), vertex(vertex){


	ShUpdateListTextEvent event("_Stretch", ShUpdateListTextEvent::UpdateType::editTextAndNewLineHeadTitleWithText);
	this->graphicView->Notify(&event);

	ShUpdateCommandEditHeadTitle event2("Stretch >> Specify stretch point: ");
	this->graphicView->Notify(&event2);

	QList<ShEntity*>::iterator itr;

	for (itr = this->entitiesToStretch.begin(); itr != this->entitiesToStretch.end(); ++itr)
		this->graphicView->preview.Add((*itr)->Clone());


	if (this->graphicView->rubberBand != 0)
		delete this->graphicView->rubberBand;

	ShPoint3d point(vertex.x, vertex.y);
	this->graphicView->rubberBand = new ShRubberBand(ShLineData(point, point));

	this->graphicView->update((DrawType)(DrawType::DrawCaptureImage |
		DrawType::DrawPreviewEntities | DrawType::DrawActionHandler));

	qDebug("Changed to StretchTemporaryAction.");
}

ShStretchTemporaryAction::ShStretchTemporaryAction(ShGraphicView *graphicView, ShActionHandler *previousAction,
	const QList<ShEntity*>& entitiesToStretch, const QList<ShStretchData*>& stretchDataList, ShPoint3d vertex)
	:ShTemporaryAction(graphicView, previousAction), entitiesToStretch(entitiesToStretch),
	stretchDataList(stretchDataList), vertex(vertex){

	ShUpdateListTextEvent event("_Stretch", ShUpdateListTextEvent::UpdateType::editTextAndNewLineHeadTitleWithText);
	this->graphicView->Notify(&event);

	ShUpdateCommandEditHeadTitle event2("Stretch >> Specify stretch point: ");
	this->graphicView->Notify(&event2);

	QList<ShEntity*>::iterator itr;

	for (itr = this->entitiesToStretch.begin(); itr != this->entitiesToStretch.end(); ++itr)
		this->graphicView->preview.Add((*itr)->Clone());


	if (this->graphicView->rubberBand != 0)
		delete this->graphicView->rubberBand;

	ShPoint3d point(vertex.x, vertex.y);
	this->graphicView->rubberBand = new ShRubberBand(ShLineData(point, point));


	this->graphicView->update((DrawType)(DrawType::DrawCaptureImage |
		DrawType::DrawPreviewEntities | DrawType::DrawActionHandler));

	qDebug("Changed to StretchTemporaryAction.");
}

ShStretchTemporaryAction::~ShStretchTemporaryAction() {

	
	this->graphicView->preview.DeleteAll();

	if (this->graphicView->rubberBand != 0) {
		delete this->graphicView->rubberBand;
		this->graphicView->rubberBand = 0;
	}

	this->graphicView->update(DrawType::DrawCaptureImage);

	
	
	while (!this->stretchDataList.isEmpty())
		delete this->stretchDataList.takeFirst();
	
}


#include "Command Pattern\Entity Command\ShStretchEntityCommand.h"
void ShStretchTemporaryAction::LMousePressEvent(QMouseEvent *event, ShActionData& data) {

	ShPoint3d point = data.GetPoint();
	

	QList<ShEntity*> entitiesToStretch;
	QList<ShEntity*>::iterator itr = this->entitiesToStretch.begin();
	for (itr = this->entitiesToStretch.begin();
		itr != this->entitiesToStretch.end();
		++itr) {
	
		entitiesToStretch.append((*itr)->Clone());
	}

	ShStretchVisitor visitor(this->vertex, point);

	QList<ShEntity*>::iterator originalItr = this->entitiesToStretch.begin();
	QList<ShStretchData*>::iterator dataItr = this->stretchDataList.begin();
	for (itr = entitiesToStretch.begin();
		itr != entitiesToStretch.end();
		++itr) {
	
		this->graphicView->selectedEntityManager.Pop((*originalItr));
		this->graphicView->entityTable.Remove((*originalItr));

		this->graphicView->selectedEntityManager.Push((*itr));
		this->graphicView->entityTable.Add((*itr));

		visitor.SetOriginal((*originalItr));
		visitor.SetStretchData((*dataItr));
		++originalItr;
		++dataItr;
		(*itr)->Accept(&visitor);
	}

	

	this->graphicView->update(DrawType::DrawAll);
	this->graphicView->CaptureImage();

	ShStretchEntityCommand *command = new ShStretchEntityCommand(this->graphicView,
		this->entitiesToStretch, entitiesToStretch);

	this->graphicView->undoTaker.Push(command);

	if (!this->graphicView->redoTaker.IsEmpty())
		this->graphicView->redoTaker.DeleteAll();
	

	ShUpdateListTextEvent event2("");
	this->graphicView->Notify(&event2);

	ShUpdateCommandEditHeadTitle event3(this->previousAction->GetActionHeadTitle());
	this->graphicView->Notify(&event3);

	this->ReturnToPrevious();

}


void ShStretchTemporaryAction::MouseMoveEvent(QMouseEvent *event, ShActionData& data) {

	ShPoint3d end = data.GetPoint();
	this->graphicView->rubberBand->SetEnd(end);

	ShStretchVisitor visitor(this->vertex, end);

	QLinkedList<ShEntity*>::iterator itr;
	QList<ShEntity*>::iterator originalItr = this->entitiesToStretch.begin();
	QList<ShStretchData*>::iterator dataItr = this->stretchDataList.begin();
	for (itr = this->graphicView->preview.Begin();
		itr != this->graphicView->preview.End();
		++itr) {

		visitor.SetOriginal((*originalItr));
		visitor.SetStretchData((*dataItr));
		++originalItr;
		++dataItr;
		(*itr)->Accept(&visitor);
	}

	

	data.AppendDrawType((DrawType)(DrawType::DrawCaptureImage |
			DrawType::DrawPreviewEntities | DrawType::DrawActionHandler));
}

void ShStretchTemporaryAction::KeyPressEvent(QKeyEvent *event, ShActionData& data) {
	
	if (event->key() == Qt::Key::Key_Escape) {

		this->ReturnToPrevious();
		
		ShUpdateListTextEvent event("<Cancel>");
		this->graphicView->Notify(&event);
	}
	else {

		ShKeyPressedEvent event2(event);
		this->graphicView->Notify(&event2);
	}
}

QCursor ShStretchTemporaryAction::GetCursorShape() {
	
	return QCursor(Qt::CursorShape::CrossCursor);
}

ActionType ShStretchTemporaryAction::GetType() {

	return ActionType::ActionStretch;
}

#include <qpainter.h>
void ShStretchTemporaryAction::DrawVertex() {

	int dx, dy;
	this->graphicView->ConvertEntityToDevice(this->vertex.x, this->vertex.y, dx, dy);

	QPainter painter(this->graphicView);
	painter.fillRect(dx - 3, dy - 3, 6, 6, QColor(255, 000, 000));
}

void ShStretchTemporaryAction::EraseVertex() {

	int dx, dy;
	this->graphicView->ConvertEntityToDevice(this->vertex.x, this->vertex.y, dx, dy);

	QPainter painter(this->graphicView);
	painter.fillRect(dx - 3, dy - 3, 6, 6, QColor(000, 153, 255));

}

void ShStretchTemporaryAction::Draw(QPainter *painter) {

	if (painter->isActive() == false)
		painter->begin(this->graphicView);


	int dx, dy;
	this->graphicView->ConvertEntityToDevice(this->vertex.x, this->vertex.y, dx, dy);

	painter->fillRect(dx - 3, dy - 3, 6, 6, QColor(255, 000, 000));

}


void ShStretchTemporaryAction::ApplyOrthogonalShape(bool on) {

	ShPoint3d mouse, point;
	mouse = this->graphicView->GetCursorPoint();
	
	if (on == true)
		this->GetOrthogonal(this->vertex.x, this->vertex.y, mouse.x, mouse.y, point.x, point.y);
	else
		point = mouse;


	this->graphicView->rubberBand->SetEnd(point);


	ShStretchVisitor visitor(this->vertex, point);

	QLinkedList<ShEntity*>::iterator itr;
	QList<ShEntity*>::iterator originalItr = this->entitiesToStretch.begin();
	QList<ShStretchData*>::iterator dataItr = this->stretchDataList.begin();
	for (itr = this->graphicView->preview.Begin();
		itr != this->graphicView->preview.End();
		++itr) {

		visitor.SetOriginal((*originalItr));
		visitor.SetStretchData((*dataItr));
		++originalItr;
		++dataItr;
		(*itr)->Accept(&visitor);
	}

	this->graphicView->update((DrawType)(DrawType::DrawCaptureImage |
		DrawType::DrawPreviewEntities | DrawType::DrawActionHandler));

}


QString ShStretchTemporaryAction::GetActionHeadTitle() {

	return QString("Stretch >> Specify stretch point: ");
}

void ShStretchTemporaryAction::IsAllowedDraftOperation(ShAllowedDraftData &data) {

	data.SetAllowOrthogonal(true);
	data.SetAllowtSnap(true);

	data.SetOrthogonalBasePoint(this->vertex);
	data.SetSnapBasePoint(this->vertex);
}