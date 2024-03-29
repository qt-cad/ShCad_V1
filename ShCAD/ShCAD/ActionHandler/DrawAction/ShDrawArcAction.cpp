

#include "ShDrawArcAction.h"
#include "ShNotifyEvent.h"
#include <QKeyEvent>
#include "Entity\Leaf\ShArc.h"
#include "ShMath.h"
#include "Entity\Leaf\ShRubberBand.h"

ShDrawArcAction::ShDrawArcAction(ShGraphicView *graphicView, ShDrawArcAction::DrawMethod drawMethod)
	:ShDrawAction(graphicView), drawArcMethod(0), drawMethod(drawMethod) {

	this->status = PickedNothing;
	

	ShUpdateListTextEvent event("_Arc", ShUpdateListTextEvent::UpdateType::editTextAndNewLineHeadTitleWithText);
	this->graphicView->Notify(&event);

	this->SetDrawMethod(this->drawMethod);

	QString headTitle = this->drawArcMethod->GetActionHeadTitle();

	ShUpdateCommandEditHeadTitle event2(headTitle);
	this->graphicView->Notify(&event2);

}

ShDrawArcAction::~ShDrawArcAction() {

	if (this->drawArcMethod != 0)
		delete this->drawArcMethod;
}

void ShDrawArcAction::LMousePressEvent(QMouseEvent *event, ShActionData& data) {

	this->drawArcMethod->LMousePressEvent(event, data);

}

void ShDrawArcAction::MouseMoveEvent(QMouseEvent *event, ShActionData& data) {

	this->drawArcMethod->MouseMoveEvent(event, data);
}

#include "Strategy Pattern\ShChangeCurrentActionStrategy.h"
void ShDrawArcAction::KeyPressEvent(QKeyEvent *event, ShActionData& data) {

	if (event->key() == Qt::Key::Key_Escape) {

		ShChangeCurrentActionCancelCurrent strategy(ActionType::ActionDefault);
		this->graphicView->ChangeCurrentAction(strategy);
		
	}
	else {

		ShKeyPressedEvent event2(event);
		this->graphicView->Notify(&event2);
	}
}


ActionType ShDrawArcAction::GetType() {

	return ActionType::ActionDrawArcCenterStartEnd;
}

void ShDrawArcAction::ApplyOrthogonalShape(bool on) {

	this->drawArcMethod->ApplyOrthogonalShape(on);
}

QString ShDrawArcAction::GetActionHeadTitle() {

	return this->drawArcMethod->GetActionHeadTitle();
}

void ShDrawArcAction::IsAllowedDraftOperation(ShAllowedDraftData &data) {

	this->drawArcMethod->IsAllowedDraftOperation(data);
}


void ShDrawArcAction::SetDrawMethod(DrawMethod drawMethod) {

	if (this->drawArcMethod != 0)
		delete this->drawArcMethod;

	switch (drawMethod) {

	case DrawMethod::ThreePoint:
		this->drawArcMethod = new ShDrawArcMethod_ThreePoint(this, this->graphicView);
		break;
	case DrawMethod::StartCenterEnd:
		this->drawArcMethod = new ShDrawArcMethod_StartCentertEnd(this, this->graphicView);
		break;
	case DrawMethod::StartCenterAngle:
		this->drawArcMethod = new ShDrawArcMethod_StartCentertAngle(this, this->graphicView);
		break;
	case DrawMethod::StartCenterLength:
		this->drawArcMethod = new ShDrawArcMethod_StartCentertLength(this, this->graphicView);
		break;
	case DrawMethod::StartEndAngle:
		this->drawArcMethod = new ShDrawArcMethod_StartEndAngle(this, this->graphicView);
		break;
	case DrawMethod::StartEndDirection:
		this->drawArcMethod = new ShDrawArcMethod_StartEndDirection(this, this->graphicView);
		break;
	case DrawMethod::StartEndRadius:
		this->drawArcMethod = new ShDrawArcMethod_StartEndRadius(this, this->graphicView);
		break;
	case DrawMethod::CenterStartEnd:
		this->drawArcMethod = new ShDrawArcMethod_CenterStartEnd(this, this->graphicView);
		break;
	case DrawMethod::CenterStartAngle:
		this->drawArcMethod = new ShDrawArcMethod_CenterStartAngle(this, this->graphicView);
		break;
	case DrawMethod::CenterStartLength:
		this->drawArcMethod = new ShDrawArcMethod_CenterStartLength(this, this->graphicView);
		break;
	default:
		break;
	}



}

////////////////////////////////////////////////////////////////////////////////////////////

ShDrawArcMethod::ShDrawArcMethod(ShDrawArcAction *drawArcAction, ShGraphicView *view)
	:drawArcAction(drawArcAction), view(view) {

}


ShDrawArcMethod::~ShDrawArcMethod() {


}

void ShDrawArcMethod::UpdateNextListText() {

	ShUpdateListTextEvent event("");
	this->view->Notify(&event);

	ShUpdateCommandEditHeadTitle event2(this->drawArcAction->GetActionHeadTitle());
	this->view->Notify(&event2);
}

bool ShDrawArcMethod::CheckValidDataAndUpdateListText(const ShArcData& data) {

	if (Math::Compare(data.radius, 0) == 0) {
		ShUpdateListTextEvent event("");
		this->view->Notify(&event);
		ShUpdateListTextEvent event2("Radius must be nonzero.",
			ShUpdateListTextEvent::UpdateType::TextWithoutAnything);
		this->view->Notify(&event2);
		ShUpdateCommandEditHeadTitle event3(this->drawArcAction->GetActionHeadTitle());
		this->view->Notify(&event3);

		return false;
	}
	if (Math::Compare(data.startAngle, data.endAngle) == 0) {
		ShUpdateListTextEvent event("");
		this->view->Notify(&event);
		ShUpdateListTextEvent event2("Invalid point.",
			ShUpdateListTextEvent::UpdateType::TextWithoutAnything);
		this->view->Notify(&event2);
		ShUpdateCommandEditHeadTitle event3(this->drawArcAction->GetActionHeadTitle());
		this->view->Notify(&event3);

		return false;
	}

	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////


ShDrawArcMethod_ThreePoint::ShDrawArcMethod_ThreePoint(ShDrawArcAction *drawArcAction, ShGraphicView *view)
	:ShDrawArcMethod(drawArcAction, view) {


}

ShDrawArcMethod_ThreePoint::~ShDrawArcMethod_ThreePoint() {

}

void ShDrawArcMethod_ThreePoint::LMousePressEvent(QMouseEvent *event, ShActionData &data) {
	
	ShDrawArcAction::Status &status = this->GetStatus();
	ShPoint3d point = data.GetPoint();
	ShPoint3d nextPoint = data.GetNextPoint();

	if (status == ShDrawArcAction::Status::PickedNothing) {
		
		this->first = point;

		status = ShDrawArcAction::Status::PickedFirstPoint;

		this->view->rubberBand = new ShRubberBand(ShLineData(point, nextPoint));

		this->view->update((DrawType)(DrawType::DrawCaptureImage | DrawType::DrawPreviewEntities));

		this->UpdateNextListText();
	}
	else if (status == ShDrawArcAction::Status::PickedFirstPoint) {
	
		this->second = point;

		status = ShDrawArcAction::Status::PickedSecondPoint;

		this->view->rubberBand->SetStart(this->second);
		this->view->rubberBand->SetEnd(nextPoint);

		ShArcData arcData;
		this->GetArcDataWithThreePoint(this->first, this->second, nextPoint, arcData);

		this->view->preview.Add(new ShArc(ShPropertyData(*this->view->GetData()->GetPropertyData()),
			arcData, this->view->entityTable.GetLayerTable()->GetCurrentLayer()));

		this->view->update((DrawType)(DrawType::DrawCaptureImage | DrawType::DrawPreviewEntities));

		this->UpdateNextListText();

	}
	else if (status == ShDrawArcAction::Status::PickedSecondPoint) {
	
		ShArcData arcData;
		if (this->GetArcDataWithThreePoint(this->first, this->second, point, arcData) == false) {
			ShUpdateListTextEvent event2("");
			this->view->Notify(&event2);
			ShUpdateListTextEvent event3("Arc does not exist.",
				ShUpdateListTextEvent::UpdateType::TextWithoutAnything);
			this->view->Notify(&event3);
			ShUpdateCommandEditHeadTitle event4(this->drawArcAction->GetActionHeadTitle());
			this->view->Notify(&event4);
			return;
		}
		
		ShArc *previewArc = dynamic_cast<ShArc*>((*this->view->preview.Begin()));
		previewArc->SetData(arcData);

		ShUpdateListTextEvent event2("");
		this->view->Notify(&event2);

		this->AddEntityAndFinish(previewArc->Clone(), "Arc");

	}

}

void ShDrawArcMethod_ThreePoint::MouseMoveEvent(QMouseEvent *event, ShActionData &data) {

	ShDrawArcAction::Status &status = this->GetStatus();
	ShPoint3d point = data.GetPoint();

	if (status == ShDrawArcAction::Status::PickedFirstPoint) {
		this->view->rubberBand->SetEnd(point);

		data.AppendDrawType((DrawType)(DrawType::DrawCaptureImage | DrawType::DrawPreviewEntities));
	}
	else if (status == ShDrawArcAction::Status::PickedSecondPoint) {
	
		this->view->rubberBand->SetEnd(point);

		ShArcData arcData;
		this->GetArcDataWithThreePoint(this->first, this->second, point, arcData);

		ShArc *previewArc = dynamic_cast<ShArc*>((*this->view->preview.Begin()));
		previewArc->SetData(arcData);

		data.AppendDrawType((DrawType)(DrawType::DrawCaptureImage | DrawType::DrawPreviewEntities));
	}

}

void ShDrawArcMethod_ThreePoint::ApplyOrthogonalShape(bool on) {

	if (this->GetStatus() == ShDrawArcAction::Status::PickedNothing)
		return;

	ShPoint3d mouse = this->view->GetCursorPoint();

	if (this->GetStatus() == ShDrawArcAction::Status::PickedFirstPoint) {

		if (on == true) {
			ShPoint3d orth;
			this->GetOrthogonal(this->first.x, this->first.y, mouse.x, mouse.y, orth.x, orth.y);
			this->view->rubberBand->SetEnd(orth);
		}
		else {
			this->view->rubberBand->SetEnd(mouse);
		}
	}
	else if (this->GetStatus() == ShDrawArcAction::Status::PickedSecondPoint) {

		if (on == true) {
			ShPoint3d orth;
			this->GetOrthogonal(this->second.x, this->second.y, mouse.x, mouse.y, orth.x, orth.y);
			this->view->rubberBand->SetEnd(orth);
		}
		else {
			this->view->rubberBand->SetEnd(mouse);
		}
		ShPoint3d third = this->view->rubberBand->GetEnd();

		ShArcData arcData;
		this->GetArcDataWithThreePoint(this->first, this->second, third, arcData);

		ShArc *previewArc = dynamic_cast<ShArc*>((*this->view->preview.Begin()));
		previewArc->SetData(arcData);
	}


	this->view->update((DrawType)(DrawCaptureImage | DrawPreviewEntities));
}

QString ShDrawArcMethod_ThreePoint::GetActionHeadTitle() {

	ShDrawArcAction::Status status = this->GetStatus();
	QString str;

	if (status == ShDrawArcAction::Status::PickedNothing)
		str = "Arc >> Specify first point of arc: ";
	else if (status == ShDrawArcAction::Status::PickedFirstPoint)
		str = "Arc >> Specify second point of arc: ";
	else if (status == ShDrawArcAction::Status::PickedSecondPoint)
		str = "Arc >> Specify third point of arc: ";

	return str;
}

void ShDrawArcMethod_ThreePoint::IsAllowedDraftOperation(ShAllowedDraftData &data) {

	ShDrawArcAction::Status status = this->GetStatus();

	if (status == ShDrawArcAction::Status::PickedNothing) {

		data.SetAllowOrthogonal(true);
		data.SetAllowtSnap(true);

		ShPoint3d mouse = this->view->GetCursorPoint();
		data.SetOrthogonalBasePoint(mouse);
		data.SetSnapBasePoint(mouse);
	}
	else if (status == ShDrawArcAction::Status::PickedFirstPoint ||
		status == ShDrawArcAction::Status::PickedSecondPoint) {
		data.SetAllowOrthogonal(true);
		data.SetAllowtSnap(true);

		ShPoint3d first = this->view->rubberBand->GetStart();

		data.SetOrthogonalBasePoint(first);
		data.SetSnapBasePoint(first);

	}

}

bool ShDrawArcMethod_ThreePoint::GetArcDataWithThreePoint(const ShPoint3d& first, const ShPoint3d& second,
	const ShPoint3d& third, ShArcData &data) {

	if (Math::GetCenterWithThreePoint(first, second, third, data.center) == false) {
		data.center.x = 0;
		data.center.y = 0;
		data.radius = 0;
		data.startAngle = 0;
		data.endAngle = 0;
		return false;
	}
	
	data.radius = Math::GetDistance(data.center.x, data.center.y, this->first.x, this->first.y);

	double startAngle = Math::GetAbsAngle(data.center.x, data.center.y, third.x, third.y);
	double endAngle = Math::GetAbsAngle(data.center.x, data.center.y, first.x, first.y);
	double angle = Math::GetAbsAngle(data.center.x, data.center.y, second.x, second.y);

	if (Math::CheckAngleLiesOnAngleBetween(startAngle, endAngle, angle) == true) {
		data.startAngle = startAngle;
		data.endAngle = endAngle;
	}
	else {
		data.startAngle = endAngle;
		data.endAngle = startAngle;
	}

	return true;
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////

ShDrawArcMethod_StartCentertEnd::ShDrawArcMethod_StartCentertEnd(ShDrawArcAction *drawArcAction, ShGraphicView *view)
	:ShDrawArcMethod(drawArcAction, view) {

}

ShDrawArcMethod_StartCentertEnd::~ShDrawArcMethod_StartCentertEnd() {

}

void ShDrawArcMethod_StartCentertEnd::LMousePressEvent(QMouseEvent *event, ShActionData& data) {

	ShDrawArcAction::Status &status = this->GetStatus();
	ShPoint3d point = data.GetPoint();
	ShPoint3d nextPoint = data.GetNextPoint();

	if (status == ShDrawArcAction::Status::PickedNothing) {

		this->start = point;

		status = ShDrawArcAction::Status::PickedStart;

		this->view->rubberBand = new ShRubberBand(ShLineData(point, nextPoint));

		this->view->update((DrawType)(DrawType::DrawCaptureImage | DrawType::DrawPreviewEntities));

		this->UpdateNextListText();

	}
	else if (status == ShDrawArcAction::Status::PickedStart) {

		ShArcData arcData;
		arcData.center = point;
		arcData.radius = Math::GetDistance(point.x, point.y, this->start.x, this->start.y);
		arcData.startAngle = Math::GetAbsAngle(point.x, point.y, this->start.x, this->start.y);
		arcData.endAngle = Math::GetAbsAngle(point.x, point.y, nextPoint.x, nextPoint.y);

		if (this->CheckValidDataAndUpdateListText(arcData) == false)
			return;

		status = ShDrawArcAction::Status::PickedCenter;

		this->center = point;
	
		this->view->rubberBand->SetStart(this->center);
		this->view->rubberBand->SetEnd(nextPoint);
		
		this->view->preview.Add(new ShArc(ShPropertyData(*this->view->GetData()->GetPropertyData()),
			arcData, this->view->entityTable.GetLayerTable()->GetCurrentLayer()));

		this->view->update((DrawType)(DrawType::DrawCaptureImage | DrawType::DrawPreviewEntities));

		this->UpdateNextListText();

	}
	else if (status == ShDrawArcAction::Status::PickedCenter) {

		ShArc *previewArc = dynamic_cast<ShArc*>((*this->view->preview.Begin()));
		ShArcData arcData = previewArc->GetData();

		arcData.endAngle = Math::GetAbsAngle(arcData.center.x, arcData.center.y, point.x, point.y);

		if (this->CheckValidDataAndUpdateListText(arcData) == true) {
			previewArc->SetData(arcData);

			ShUpdateListTextEvent event2("");
			this->view->Notify(&event2);

			this->AddEntityAndFinish(previewArc->Clone(), "Arc");
		}
	}
}

void ShDrawArcMethod_StartCentertEnd::MouseMoveEvent(QMouseEvent *event, ShActionData& data) {

	ShDrawArcAction::Status status = this->GetStatus();

	if (status == ShDrawArcAction::Status::PickedStart) {

		ShPoint3d point = data.GetPoint();

		this->view->rubberBand->SetEnd(point);

		data.AppendDrawType((DrawType)(DrawType::DrawCaptureImage | DrawType::DrawPreviewEntities));
	}
	else if (status == ShDrawArcAction::Status::PickedCenter) {

		ShPoint3d point = data.GetPoint();

		ShArc *previewArc = dynamic_cast<ShArc*>((*this->view->preview.Begin()));
		ShPoint3d center = previewArc->GetCenter();
		double endAngle = Math::GetAbsAngle(center.x, center.y, point.x, point.y);

		previewArc->SetEndAngle(endAngle);

		this->view->rubberBand->SetEnd(point);

		data.AppendDrawType((DrawType)(DrawType::DrawCaptureImage | DrawType::DrawPreviewEntities));
	}
}

void ShDrawArcMethod_StartCentertEnd::ApplyOrthogonalShape(bool on) {

	ShDrawArcAction::Status status = this->GetStatus();

	if (status == ShDrawArcAction::Status::PickedNothing)
		return;

	ShPoint3d mouse = this->view->GetCursorPoint();

	if (status == ShDrawArcAction::Status::PickedStart) {

		if (on == true) {
			ShPoint3d center = this->view->rubberBand->GetStart();
			ShPoint3d orth;
			this->GetOrthogonal(center.x, center.y, mouse.x, mouse.y, orth.x, orth.y);
			this->view->rubberBand->SetEnd(orth);
		}
		else {
			this->view->rubberBand->SetEnd(mouse);
		}

	}
	else if (status == ShDrawArcAction::Status::PickedCenter) {
		ShArc *arc = dynamic_cast<ShArc*>((*this->view->preview.Begin()));
		ShPoint3d center = arc->GetCenter();
		double endAngle;

		if (on == true) {

			ShPoint3d orth;
			this->GetOrthogonal(center.x, center.y, mouse.x, mouse.y, orth.x, orth.y);
			this->view->rubberBand->SetEnd(orth);
			endAngle = Math::GetAbsAngle(center.x, center.y, orth.x, orth.y);

		}
		else {
			this->view->rubberBand->SetEnd(mouse);
			endAngle = Math::GetAbsAngle(center.x, center.y, mouse.x, mouse.y);
		}

		arc->SetEndAngle(endAngle);
	}

	this->view->update((DrawType)(DrawCaptureImage | DrawPreviewEntities));
}

QString ShDrawArcMethod_StartCentertEnd::GetActionHeadTitle() {

	ShDrawArcAction::Status status = this->GetStatus();
	QString str = "";

	if (status == ShDrawArcAction::Status::PickedNothing)
		str = "Arc >> Specify start point of arc: ";
	else if (status == ShDrawArcAction::Status::PickedStart)
		str = "Arc >> Specify center point of arc: ";
	else if (status == ShDrawArcAction::Status::PickedCenter)
		str = "Arc >> Specify end point of arc: ";

	return str;
}

void ShDrawArcMethod_StartCentertEnd::IsAllowedDraftOperation(ShAllowedDraftData &data) {

	ShDrawArcAction::Status status = this->GetStatus();

	if (status == ShDrawArcAction::Status::PickedNothing) {

		data.SetAllowOrthogonal(true);
		data.SetAllowtSnap(true);

		ShPoint3d mouse = this->view->GetCursorPoint();
		data.SetOrthogonalBasePoint(mouse);
		data.SetSnapBasePoint(mouse);
	}
	else if (status == ShDrawArcAction::Status::PickedStart) {
		data.SetAllowOrthogonal(true);
		data.SetAllowtSnap(true);

		ShPoint3d point = this->view->rubberBand->GetStart();

		data.SetOrthogonalBasePoint(point);
		data.SetSnapBasePoint(point);

	}
	else if (status == ShDrawArcAction::Status::PickedCenter) {
		data.SetAllowOrthogonal(true);
		data.SetAllowtSnap(true);

		ShArc *previewArc = dynamic_cast<ShArc*>((*this->view->preview.Begin()));

		data.SetOrthogonalBasePoint(previewArc->GetCenter());
		data.SetSnapBasePoint(previewArc->GetCenter());
	}

}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////


ShDrawArcMethod_StartCentertAngle::ShDrawArcMethod_StartCentertAngle(ShDrawArcAction *drawArcAction, ShGraphicView *view)
	:ShDrawArcMethod(drawArcAction, view) {

}

ShDrawArcMethod_StartCentertAngle::~ShDrawArcMethod_StartCentertAngle() {

}

void ShDrawArcMethod_StartCentertAngle::LMousePressEvent(QMouseEvent *event, ShActionData& data) {

	ShDrawArcAction::Status &status = this->GetStatus();
	ShPoint3d point = data.GetPoint();
	ShPoint3d nextPoint = data.GetNextPoint();

	if (status == ShDrawArcAction::Status::PickedNothing) {

		this->start = point;

		status = ShDrawArcAction::Status::PickedStart;

		this->view->rubberBand = new ShRubberBand(ShLineData(point, nextPoint));

		this->view->update((DrawType)(DrawType::DrawCaptureImage | DrawType::DrawPreviewEntities));

		this->UpdateNextListText();

	}
	else if (status == ShDrawArcAction::Status::PickedStart) {

		ShArcData arcData;
		arcData.center = point;
		arcData.radius = Math::GetDistance(point.x, point.y, this->start.x, this->start.y);
		arcData.startAngle = Math::GetAbsAngle(point.x, point.y, this->start.x, this->start.y);
		double angleCenterToNext = Math::GetAbsAngle(point.x, point.y, nextPoint.x, nextPoint.y);
		arcData.endAngle = Math::AddAngle(arcData.startAngle, angleCenterToNext);


		status = ShDrawArcAction::Status::PickedCenter;

		this->center = point;

		this->view->rubberBand->SetStart(this->center);
		this->view->rubberBand->SetEnd(nextPoint);

		this->view->preview.Add(new ShArc(ShPropertyData(*this->view->GetData()->GetPropertyData()),
			arcData, this->view->entityTable.GetLayerTable()->GetCurrentLayer()));

		this->view->update((DrawType)(DrawType::DrawCaptureImage | DrawType::DrawPreviewEntities));

		this->UpdateNextListText();

	}
	else if (status == ShDrawArcAction::Status::PickedCenter) {

		ShArc *previewArc = dynamic_cast<ShArc*>((*this->view->preview.Begin()));
		ShArcData arcData = previewArc->GetData();

		double angleCenterToPoint = Math::GetAbsAngle(this->center.x, this->center.y, point.x, point.y);
		arcData.endAngle = Math::AddAngle(arcData.startAngle, angleCenterToPoint);

		if (this->CheckValidDataAndUpdateListText(arcData) == true) {

			previewArc->SetData(arcData);

			ShUpdateListTextEvent event2("");
			this->view->Notify(&event2);

			this->AddEntityAndFinish(previewArc->Clone(), "Arc");

		}
	}
}

void ShDrawArcMethod_StartCentertAngle::MouseMoveEvent(QMouseEvent *event, ShActionData& data) {

	ShDrawArcAction::Status status = this->GetStatus();

	if (status == ShDrawArcAction::Status::PickedStart) {

		ShPoint3d point = data.GetPoint();

		this->view->rubberBand->SetEnd(point);

		data.AppendDrawType((DrawType)(DrawType::DrawCaptureImage | DrawType::DrawPreviewEntities));
	}
	else if (status == ShDrawArcAction::Status::PickedCenter) {

		ShPoint3d point = data.GetPoint();

		ShArc *previewArc = dynamic_cast<ShArc*>((*this->view->preview.Begin()));

		double angleCenterToPoint = Math::GetAbsAngle(this->center.x, this->center.y, point.x, point.y);
		double endAngle = Math::AddAngle(previewArc->GetStartAngle(), angleCenterToPoint);
		previewArc->SetEndAngle(endAngle);

		this->view->rubberBand->SetEnd(point);

		data.AppendDrawType((DrawType)(DrawType::DrawCaptureImage | DrawType::DrawPreviewEntities));
	}
}

void ShDrawArcMethod_StartCentertAngle::ApplyOrthogonalShape(bool on) {

	ShDrawArcAction::Status status = this->GetStatus();

	if (status == ShDrawArcAction::Status::PickedNothing)
		return;

	ShPoint3d mouse = this->view->GetCursorPoint();

	if (status == ShDrawArcAction::Status::PickedStart) {

		if (on == true) {
			ShPoint3d center = this->view->rubberBand->GetStart();
			ShPoint3d orth;
			this->GetOrthogonal(center.x, center.y, mouse.x, mouse.y, orth.x, orth.y);
			this->view->rubberBand->SetEnd(orth);
		}
		else {
			this->view->rubberBand->SetEnd(mouse);
		}

	}
	else if (status == ShDrawArcAction::Status::PickedCenter) {
		ShArc *arc = dynamic_cast<ShArc*>((*this->view->preview.Begin()));
		ShPoint3d center = arc->GetCenter();
		double angleCenterToPoint;

		if (on == true) {

			ShPoint3d orth;
			this->GetOrthogonal(center.x, center.y, mouse.x, mouse.y, orth.x, orth.y);
			this->view->rubberBand->SetEnd(orth);
			angleCenterToPoint = Math::GetAbsAngle(center.x, center.y, orth.x, orth.y);

		}
		else {
			this->view->rubberBand->SetEnd(mouse);
			angleCenterToPoint = Math::GetAbsAngle(center.x, center.y, mouse.x, mouse.y);
		}


		arc->SetEndAngle(Math::AddAngle(arc->GetStartAngle(), angleCenterToPoint));
	}

	this->view->update((DrawType)(DrawCaptureImage | DrawPreviewEntities));
}

QString ShDrawArcMethod_StartCentertAngle::GetActionHeadTitle() {

	ShDrawArcAction::Status status = this->GetStatus();
	QString str = "";

	if (status == ShDrawArcAction::Status::PickedNothing)
		str = "Arc >> Specify start point of arc: ";
	else if (status == ShDrawArcAction::Status::PickedStart)
		str = "Arc >> Specify center point of arc: ";
	else if (status == ShDrawArcAction::Status::PickedCenter)
		str = "Arc >> Specify included angle: ";

	return str;
}

void ShDrawArcMethod_StartCentertAngle::IsAllowedDraftOperation(ShAllowedDraftData &data) {

	ShDrawArcAction::Status status = this->GetStatus();

	if (status == ShDrawArcAction::Status::PickedNothing) {

		data.SetAllowOrthogonal(true);
		data.SetAllowtSnap(true);

		ShPoint3d mouse = this->view->GetCursorPoint();
		data.SetOrthogonalBasePoint(mouse);
		data.SetSnapBasePoint(mouse);
	}
	else if (status == ShDrawArcAction::Status::PickedStart) {
		data.SetAllowOrthogonal(true);
		data.SetAllowtSnap(true);

		ShPoint3d point = this->view->rubberBand->GetStart();

		data.SetOrthogonalBasePoint(point);
		data.SetSnapBasePoint(point);

	}
	else if (status == ShDrawArcAction::Status::PickedCenter) {
		data.SetAllowOrthogonal(true);
		data.SetAllowtSnap(true);

		ShPoint3d point = this->view->rubberBand->GetStart();

		data.SetOrthogonalBasePoint(point);
		data.SetSnapBasePoint(point);
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////

ShDrawArcMethod_StartCentertLength::ShDrawArcMethod_StartCentertLength(ShDrawArcAction *drawArcAction, ShGraphicView *view)
	:ShDrawArcMethod(drawArcAction, view) {

}

ShDrawArcMethod_StartCentertLength::~ShDrawArcMethod_StartCentertLength() {

}

void ShDrawArcMethod_StartCentertLength::LMousePressEvent(QMouseEvent *event, ShActionData& data) {

	ShDrawArcAction::Status &status = this->GetStatus();
	ShPoint3d point = data.GetPoint();
	ShPoint3d nextPoint = data.GetNextPoint();

	if (status == ShDrawArcAction::Status::PickedNothing) {

		this->start = point;

		status = ShDrawArcAction::Status::PickedStart;

		this->view->rubberBand = new ShRubberBand(ShLineData(point, nextPoint));

		this->view->update((DrawType)(DrawType::DrawCaptureImage | DrawType::DrawPreviewEntities));

		this->UpdateNextListText();

	}
	else if (status == ShDrawArcAction::Status::PickedStart) {

		ShArcData arcData;
		this->GetArcDataWithStartCenterLength(this->start, point,
			Math::GetDistance(this->start.x, this->start.y, nextPoint.x, nextPoint.y),
			arcData);

		status = ShDrawArcAction::Status::PickedCenter;

		this->center = point;

		this->view->preview.Add(new ShArc(ShPropertyData(*this->view->GetData()->GetPropertyData()),
			arcData, this->view->entityTable.GetLayerTable()->GetCurrentLayer()));

		this->view->update((DrawType)(DrawType::DrawCaptureImage | DrawType::DrawPreviewEntities));

		this->UpdateNextListText();

	}
	else if (status == ShDrawArcAction::Status::PickedCenter) {

		ShArcData arcData;
		this->GetArcDataWithStartCenterLength(this->start,this->center,
			Math::GetDistance(this->start.x, this->start.y, point.x, point.y),
			arcData);

		if (this->CheckValidDataAndUpdateListText(arcData) == false)
			return;


		ShArc *previewArc = dynamic_cast<ShArc*>((*this->view->preview.Begin()));
		previewArc->SetData(arcData);

		ShUpdateListTextEvent event2("");
		this->view->Notify(&event2);

		this->AddEntityAndFinish(previewArc->Clone(), "Arc");

	}
}

void ShDrawArcMethod_StartCentertLength::MouseMoveEvent(QMouseEvent *event, ShActionData& data) {

	ShDrawArcAction::Status status = this->GetStatus();

	if (status == ShDrawArcAction::Status::PickedStart) {

		ShPoint3d point = data.GetPoint();

		this->view->rubberBand->SetEnd(point);

		data.AppendDrawType((DrawType)(DrawType::DrawCaptureImage | DrawType::DrawPreviewEntities));
	}
	else if (status == ShDrawArcAction::Status::PickedCenter) {

		ShPoint3d point = data.GetPoint();

		ShArcData arcData;
		this->GetArcDataWithStartCenterLength(this->start, this->center,
			Math::GetDistance(this->start.x, this->start.y, point.x, point.y),
			arcData);

		ShArc *previewArc = dynamic_cast<ShArc*>((*this->view->preview.Begin()));
		previewArc->SetData(arcData);


		this->view->rubberBand->SetEnd(point);

		data.AppendDrawType((DrawType)(DrawType::DrawCaptureImage | DrawType::DrawPreviewEntities));
	}
}

void ShDrawArcMethod_StartCentertLength::ApplyOrthogonalShape(bool on) {

	ShDrawArcAction::Status status = this->GetStatus();

	if (status == ShDrawArcAction::Status::PickedNothing)
		return;

	ShPoint3d mouse = this->view->GetCursorPoint();

	if (status == ShDrawArcAction::Status::PickedStart) {

		if (on == true) {

			ShPoint3d orth;
			this->GetOrthogonal(this->start.x, this->start.y, mouse.x, mouse.y, orth.x, orth.y);
			this->view->rubberBand->SetEnd(orth);
		}
		else {
			this->view->rubberBand->SetEnd(mouse);
		}

	}
	else if (status == ShDrawArcAction::Status::PickedCenter) {

		if (on == true) {

			ShPoint3d orth;
			this->GetOrthogonal(this->start.x, this->start.y, mouse.x, mouse.y, orth.x, orth.y);
			this->view->rubberBand->SetEnd(orth);
		}
		else
			this->view->rubberBand->SetEnd(mouse);

		ShPoint3d end = this->view->rubberBand->GetEnd();

		ShArcData arcData;
		this->GetArcDataWithStartCenterLength(this->start, this->center,
			Math::GetDistance(this->start.x, this->start.y, end.x, end.y),
			arcData);

		ShArc *arc = dynamic_cast<ShArc*>((*this->view->preview.Begin()));
		arc->SetData(arcData);
	}

	this->view->update((DrawType)(DrawCaptureImage | DrawPreviewEntities));
}

QString ShDrawArcMethod_StartCentertLength::GetActionHeadTitle() {

	ShDrawArcAction::Status status = this->GetStatus();
	QString str = "";

	if (status == ShDrawArcAction::Status::PickedNothing)
		str = "Arc >> Specify start point of arc: ";
	else if (status == ShDrawArcAction::Status::PickedStart)
		str = "Arc >> Specify center point of arc: ";
	else if (status == ShDrawArcAction::Status::PickedCenter)
		str = "Arc >> Specify length of chord: ";

	return str;
}

void ShDrawArcMethod_StartCentertLength::IsAllowedDraftOperation(ShAllowedDraftData &data) {

	ShDrawArcAction::Status status = this->GetStatus();

	if (status == ShDrawArcAction::Status::PickedNothing) {

		data.SetAllowOrthogonal(true);
		data.SetAllowtSnap(true);

		ShPoint3d mouse = this->view->GetCursorPoint();
		data.SetOrthogonalBasePoint(mouse);
		data.SetSnapBasePoint(mouse);
	}
	else if (status == ShDrawArcAction::Status::PickedStart ||
		status == ShDrawArcAction::Status::PickedCenter) {
		data.SetAllowOrthogonal(true);
		data.SetAllowtSnap(true);

		ShPoint3d point = this->view->rubberBand->GetStart();

		data.SetOrthogonalBasePoint(point);
		data.SetSnapBasePoint(point);

	}
}

bool ShDrawArcMethod_StartCentertLength::GetArcDataWithStartCenterLength(const ShPoint3d& start, const ShPoint3d& center,
	double length, ShArcData &arcData) {

	double radius = Math::GetDistance(center.x, center.y, start.x, start.y);
	double height = length / 2;
	double angle = asin(height / radius) * 180 / 3.1415926535897;

	arcData.radius = radius;
	arcData.center = center;
	arcData.startAngle = Math::GetAbsAngle(center.x, center.y, start.x, start.y);

	if (Math::Compare(radius, length) == 1 || Math::Compare(radius, length) == 0) {
		arcData.endAngle = Math::AddAngle(arcData.startAngle, angle * 2);
		return true;
	}

	arcData.endAngle = arcData.startAngle;
	return false;

}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ShDrawArcMethod_StartEndAngle::ShDrawArcMethod_StartEndAngle(ShDrawArcAction *drawArcAction, ShGraphicView *view)
	:ShDrawArcMethod(drawArcAction, view) {

}

ShDrawArcMethod_StartEndAngle::~ShDrawArcMethod_StartEndAngle() {

}

void ShDrawArcMethod_StartEndAngle::LMousePressEvent(QMouseEvent *event, ShActionData& data) {

	ShDrawArcAction::Status &status = this->GetStatus();
	ShPoint3d point = data.GetPoint();
	ShPoint3d nextPoint = data.GetNextPoint();

	if (status == ShDrawArcAction::Status::PickedNothing) {

		this->start = point;

		status = ShDrawArcAction::Status::PickedStart;

		this->view->rubberBand = new ShRubberBand(ShLineData(point, nextPoint));

		this->view->update((DrawType)(DrawType::DrawCaptureImage | DrawType::DrawPreviewEntities));

		this->UpdateNextListText();

	}
	else if (status == ShDrawArcAction::Status::PickedStart) {

		double dis = Math::GetDistance(this->start.x, this->start.y, point.x, point.y);
		if (Math::Compare(dis, 0) == 0) {
			ShUpdateListTextEvent event2("");
			this->view->Notify(&event2);
			ShUpdateListTextEvent event3("Invalid point.",
				ShUpdateListTextEvent::UpdateType::TextWithoutAnything);
			this->view->Notify(&event3);
			ShUpdateCommandEditHeadTitle event4(this->drawArcAction->GetActionHeadTitle());
			this->view->Notify(&event4);
			return;
		}

		status = ShDrawArcAction::Status::PickedEnd;

		this->end = point;

		ShArcData arcData;
		this->GetArcDataWithStartEndAnother(this->start, this->end, nextPoint, arcData);

		this->view->preview.Add(new ShArc(ShPropertyData(*this->view->GetData()->GetPropertyData()),
			arcData, this->view->entityTable.GetLayerTable()->GetCurrentLayer()));

		this->view->update((DrawType)(DrawType::DrawCaptureImage | DrawType::DrawPreviewEntities));

		this->UpdateNextListText();

	}
	else if (status == ShDrawArcAction::Status::PickedEnd) {

		ShArcData arcData;
		this->GetArcDataWithStartEndAnother(this->start, this->end, point, arcData);

		if (this->CheckValidDataAndUpdateListText(arcData) == false)
			return;

		ShArc *previewArc = dynamic_cast<ShArc*>((*this->view->preview.Begin()));
		previewArc->SetData(arcData);

		ShUpdateListTextEvent event2("");
		this->view->Notify(&event2);

		this->AddEntityAndFinish(previewArc->Clone(), "Arc");
	}
}

void ShDrawArcMethod_StartEndAngle::MouseMoveEvent(QMouseEvent *event, ShActionData& data) {

	ShDrawArcAction::Status status = this->GetStatus();

	if (status == ShDrawArcAction::PickedStart) {

		ShPoint3d point = data.GetPoint();

		this->view->rubberBand->SetEnd(point);
		data.AppendDrawType((DrawType)(DrawType::DrawCaptureImage | DrawType::DrawPreviewEntities));

	}
	else if (status == ShDrawArcAction::PickedEnd) {

		ShPoint3d point = data.GetPoint();

		ShArcData arcData;
		this->GetArcDataWithStartEndAnother(this->start, this->end, point, arcData);

		ShArc *previewArc = dynamic_cast<ShArc*>((*this->view->preview.Begin()));
		previewArc->SetData(arcData);

		this->view->rubberBand->SetEnd(point);

		data.AppendDrawType((DrawType)(DrawType::DrawCaptureImage | DrawType::DrawPreviewEntities));

	}
}

void ShDrawArcMethod_StartEndAngle::ApplyOrthogonalShape(bool on) {

	if (this->GetStatus() == ShDrawArcAction::Status::PickedNothing)
		return;

	ShPoint3d mouse = this->view->GetCursorPoint();

	if (this->GetStatus() == ShDrawArcAction::Status::PickedStart) {

		if (on == true) {
			ShPoint3d orth;
			this->GetOrthogonal(this->start.x, this->start.y, mouse.x, mouse.y, orth.x, orth.y);
			this->view->rubberBand->SetEnd(orth);
		}
		else {
			this->view->rubberBand->SetEnd(mouse);
		}
	}
	else if (this->GetStatus() == ShDrawArcAction::Status::PickedEnd) {

		if (on == true) {
			ShPoint3d orth;
			this->GetOrthogonal(this->start.x, this->start.y, mouse.x, mouse.y, orth.x, orth.y);
			this->view->rubberBand->SetEnd(orth);
		}
		else {
			this->view->rubberBand->SetEnd(mouse);
		}
		ShPoint3d third = this->view->rubberBand->GetEnd();

		ShArcData arcData;
		this->GetArcDataWithStartEndAnother(this->start, this->end, third, arcData);

		ShArc *previewArc = dynamic_cast<ShArc*>((*this->view->preview.Begin()));
		previewArc->SetData(arcData);
	}


	this->view->update((DrawType)(DrawCaptureImage | DrawPreviewEntities));
}

QString ShDrawArcMethod_StartEndAngle::GetActionHeadTitle() {

	ShDrawArcAction::Status status = this->GetStatus();
	QString str;

	if (status == ShDrawArcAction::Status::PickedNothing)
		str = "Arc >> Specify start point of arc: ";
	else if (status == ShDrawArcAction::Status::PickedStart)
		str = "Arc >> Specify end point of arc: ";
	else if (status == ShDrawArcAction::Status::PickedEnd)
		str = "Arc >> Specify included angle: ";

	return str;
}

void ShDrawArcMethod_StartEndAngle::IsAllowedDraftOperation(ShAllowedDraftData &data) {

	ShDrawArcAction::Status status = this->GetStatus();

	if (status == ShDrawArcAction::Status::PickedNothing) {

		data.SetAllowOrthogonal(true);
		data.SetAllowtSnap(true);

		ShPoint3d mouse = this->view->GetCursorPoint();
		data.SetOrthogonalBasePoint(mouse);
		data.SetSnapBasePoint(mouse);
	}
	else if (status == ShDrawArcAction::Status::PickedStart ||
		status == ShDrawArcAction::Status::PickedEnd) {
		data.SetAllowOrthogonal(true);
		data.SetAllowtSnap(true);

		ShPoint3d first = this->view->rubberBand->GetStart();

		data.SetOrthogonalBasePoint(first);
		data.SetSnapBasePoint(first);

	}
}

bool ShDrawArcMethod_StartEndAngle::GetArcDataWithStartEndAnother(const ShPoint3d& start, const ShPoint3d& end,
	const ShPoint3d& another, ShArcData& data) {

	double angle = Math::GetAbsAngle(start.x, start.y, another.x, another.y);

	double disStartToEnd = Math::GetDistance(start.x, start.y, end.x, end.y);
	double baseSide = disStartToEnd / 2;

	double angleBetweenBaseSideAndHypotenuse = 90 - angle / 2;

	double height = tan((angleBetweenBaseSideAndHypotenuse*3.1415926535897) / 180);

	double angleStartToEnd = Math::GetAbsAngle(start.x, start.y, end.x, end.y);
	
	Math::Rotate(angleStartToEnd, start.x, start.y, start.x + baseSide, start.y + height, data.center.x, data.center.y);

	data.startAngle = Math::GetAbsAngle(data.center.x, data.center.y, start.x, start.y);
	data.endAngle = Math::GetAbsAngle(data.center.x, data.center.y, end.x, end.y);
	data.radius = Math::GetDistance(data.center.x, data.center.y, start.x, start.y);

	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ShDrawArcMethod_StartEndDirection::ShDrawArcMethod_StartEndDirection(ShDrawArcAction *drawArcAction, ShGraphicView *view)
	:ShDrawArcMethod(drawArcAction, view) {

}

ShDrawArcMethod_StartEndDirection::~ShDrawArcMethod_StartEndDirection() {

}

void ShDrawArcMethod_StartEndDirection::LMousePressEvent(QMouseEvent *event, ShActionData& data) {

	ShDrawArcAction::Status &status = this->GetStatus();
	ShPoint3d point = data.GetPoint();
	ShPoint3d nextPoint = data.GetNextPoint();

	if (status == ShDrawArcAction::Status::PickedNothing) {

		this->start = point;

		status = ShDrawArcAction::Status::PickedStart;

		this->view->rubberBand = new ShRubberBand(ShLineData(point, nextPoint));

		this->view->update((DrawType)(DrawType::DrawCaptureImage | DrawType::DrawPreviewEntities));

		this->UpdateNextListText();

	}
	else if (status == ShDrawArcAction::Status::PickedStart) {

		double dis = Math::GetDistance(this->start.x, this->start.y, point.x, point.y);
		if (Math::Compare(dis, 0) == 0) {
			ShUpdateListTextEvent event2("");
			this->view->Notify(&event2);
			ShUpdateListTextEvent event3("Invalid point.",
				ShUpdateListTextEvent::UpdateType::TextWithoutAnything);
			this->view->Notify(&event3);
			ShUpdateCommandEditHeadTitle event4(this->drawArcAction->GetActionHeadTitle());
			this->view->Notify(&event4);
			return;
		}

		status = ShDrawArcAction::Status::PickedEnd;

		this->end = point;

		ShArcData arcData = this->GetArcDataWithTwoPointTangentLine(this->start, this->end,
			this->view->rubberBand->GetStart(), this->view->rubberBand->GetEnd());

		this->view->preview.Add(new ShArc(ShPropertyData(*this->view->GetData()->GetPropertyData()),
			arcData, this->view->entityTable.GetLayerTable()->GetCurrentLayer()));

		this->view->update((DrawType)(DrawType::DrawCaptureImage | DrawType::DrawPreviewEntities));

		this->UpdateNextListText();

	}
	else if (status == ShDrawArcAction::Status::PickedEnd) {

		ShArcData arcData= this->GetArcDataWithTwoPointTangentLine(this->start, this->end,
			this->view->rubberBand->GetStart(), this->view->rubberBand->GetEnd());

		if (this->CheckValidDataAndUpdateListText(arcData) == false)
			return;

		ShArc *previewArc = dynamic_cast<ShArc*>((*this->view->preview.Begin()));
		previewArc->SetData(arcData);

		ShUpdateListTextEvent event2("");
		this->view->Notify(&event2);

		this->AddEntityAndFinish(previewArc->Clone(), "Arc");
	}
}

void ShDrawArcMethod_StartEndDirection::MouseMoveEvent(QMouseEvent *event, ShActionData& data) {

	ShDrawArcAction::Status status = this->GetStatus();

	if (status == ShDrawArcAction::PickedStart) {

		ShPoint3d point = data.GetPoint();

		this->view->rubberBand->SetEnd(point);
		data.AppendDrawType((DrawType)(DrawType::DrawCaptureImage | DrawType::DrawPreviewEntities));

	}
	else if (status == ShDrawArcAction::PickedEnd) {

		ShPoint3d point = data.GetPoint();

		ShArcData arcData = this->GetArcDataWithTwoPointTangentLine(this->start, this->end,
			this->view->rubberBand->GetStart(), point);
		

		ShArc *previewArc = dynamic_cast<ShArc*>((*this->view->preview.Begin()));
		previewArc->SetData(arcData);

		this->view->rubberBand->SetEnd(point);

		data.AppendDrawType((DrawType)(DrawType::DrawCaptureImage | DrawType::DrawPreviewEntities));

	}
}

void ShDrawArcMethod_StartEndDirection::ApplyOrthogonalShape(bool on) {

	if (this->GetStatus() == ShDrawArcAction::Status::PickedNothing)
		return;

	ShPoint3d mouse = this->view->GetCursorPoint();

	if (this->GetStatus() == ShDrawArcAction::Status::PickedStart) {

		if (on == true) {
			ShPoint3d orth;
			this->GetOrthogonal(this->start.x, this->start.y, mouse.x, mouse.y, orth.x, orth.y);
			this->view->rubberBand->SetEnd(orth);
		}
		else {
			this->view->rubberBand->SetEnd(mouse);
		}
	}
	else if (this->GetStatus() == ShDrawArcAction::Status::PickedEnd) {

		if (on == true) {
			ShPoint3d orth;
			this->GetOrthogonal(this->start.x, this->start.y, mouse.x, mouse.y, orth.x, orth.y);
			this->view->rubberBand->SetEnd(orth);
		}
		else {
			this->view->rubberBand->SetEnd(mouse);
		}
		ShPoint3d third = this->view->rubberBand->GetEnd();

		ShArcData arcData= this->GetArcDataWithTwoPointTangentLine(this->start, this->end,
			this->view->rubberBand->GetStart(), this->view->rubberBand->GetEnd());
		

		ShArc *previewArc = dynamic_cast<ShArc*>((*this->view->preview.Begin()));
		previewArc->SetData(arcData);
	}


	this->view->update((DrawType)(DrawCaptureImage | DrawPreviewEntities));
}

QString ShDrawArcMethod_StartEndDirection::GetActionHeadTitle() {

	ShDrawArcAction::Status status = this->GetStatus();
	QString str;

	if (status == ShDrawArcAction::Status::PickedNothing)
		str = "Arc >> Specify start point of arc: ";
	else if (status == ShDrawArcAction::Status::PickedStart)
		str = "Arc >> Specify end point of arc: ";
	else if (status == ShDrawArcAction::Status::PickedEnd)
		str = "Arc >> Specify tangent direction for the start point of arc: ";

	return str;
}

void ShDrawArcMethod_StartEndDirection::IsAllowedDraftOperation(ShAllowedDraftData &data) {

	ShDrawArcAction::Status status = this->GetStatus();

	if (status == ShDrawArcAction::Status::PickedNothing) {

		data.SetAllowOrthogonal(true);
		data.SetAllowtSnap(true);

		ShPoint3d mouse = this->view->GetCursorPoint();
		data.SetOrthogonalBasePoint(mouse);
		data.SetSnapBasePoint(mouse);
	}
	else if (status == ShDrawArcAction::Status::PickedStart ||
		status == ShDrawArcAction::Status::PickedEnd) {
		data.SetAllowOrthogonal(true);
		data.SetAllowtSnap(true);

		ShPoint3d first = this->view->rubberBand->GetStart();

		data.SetOrthogonalBasePoint(first);
		data.SetSnapBasePoint(first);

	}

}

ShArcData ShDrawArcMethod_StartEndDirection::GetArcDataWithTwoPointTangentLine(const ShPoint3d& first, const ShPoint3d& second,
	const ShPoint3d& start, const ShPoint3d& end) {

	double angleStartToEnd = Math::GetAbsAngle(start.x, start.y, end.x, end.y);
	ShPoint3d tempEnd;
	Math::Rotate(angleStartToEnd + 90, first.x, first.y, first.x + 10, first.y, tempEnd.x, tempEnd.y);


	double angleFirstToSecond = Math::GetAbsAngle(first.x, first.y, second.x, second.y);

	double difference = Math::GetAngleDifference(angleStartToEnd, angleFirstToSecond);
	ShPoint3d tempEnd2;
	Math::Rotate(angleFirstToSecond + 90 + difference, second.x, second.y, second.x + 10, second.y, tempEnd2.x, tempEnd2.y);

	ShPoint3d center;
	Math::CheckLineLineIntersect(first, tempEnd, second, tempEnd2, center);

	ShArcData data;

	data.center = center;
	data.radius = Math::GetDistance(data.center.x, data.center.y, first.x, first.y);

	double angleCenterToFirst = Math::GetAbsAngle(data.center.x, data.center.y, first.x, first.y);
	double angleCenterToSecond = Math::GetAbsAngle(data.center.x, data.center.y, second.x, second.y);

	if (Math::CheckAngleLiesOnAngleBetween(angleFirstToSecond,Math::AddAngle(angleFirstToSecond,180), angleStartToEnd) == true) {
		data.startAngle = angleCenterToSecond;
		data.endAngle = angleCenterToFirst;
	}
	else {
		data.startAngle = angleCenterToFirst;
		data.endAngle = angleCenterToSecond;
	}

	
	return data;

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ShDrawArcMethod_StartEndRadius::ShDrawArcMethod_StartEndRadius(ShDrawArcAction *drawArcAction, ShGraphicView *view)
	:ShDrawArcMethod(drawArcAction, view) {


}


ShDrawArcMethod_StartEndRadius::~ShDrawArcMethod_StartEndRadius() {

}

void ShDrawArcMethod_StartEndRadius::LMousePressEvent(QMouseEvent *event, ShActionData &data) {

	ShDrawArcAction::Status &status = this->GetStatus();
	ShPoint3d point = data.GetPoint();
	ShPoint3d nextPoint = data.GetNextPoint();

	if (status == ShDrawArcAction::Status::PickedNothing) {
	
		this->start = point;

		status = ShDrawArcAction::Status::PickedStart;

		this->view->rubberBand = new ShRubberBand(ShLineData(point, nextPoint));

		this->view->update((DrawType)(DrawType::DrawCaptureImage | DrawType::DrawPreviewEntities));

		this->UpdateNextListText();

	}
	else if (status == ShDrawArcAction::Status::PickedStart) {
	
		double dis = Math::GetDistance(this->start.x, this->start.y, point.x, point.y);
		if (Math::Compare(dis, 0) == 0) {
			ShUpdateListTextEvent event2("");
			this->view->Notify(&event2);
			ShUpdateListTextEvent event3("Invalid point.",
				ShUpdateListTextEvent::UpdateType::TextWithoutAnything);
			this->view->Notify(&event3);
			ShUpdateCommandEditHeadTitle event4(this->drawArcAction->GetActionHeadTitle());
			this->view->Notify(&event4);
			return;
		}

		status = ShDrawArcAction::Status::PickedEnd;

		this->end = point;

		this->view->rubberBand->SetStart(this->end);
		this->view->rubberBand->SetEnd(nextPoint);

		ShArcData arcData;
		this->GetArcDataWithStartEndAnother(this->start, this->end, nextPoint, arcData);

		this->view->preview.Add(new ShArc(ShPropertyData(*this->view->GetData()->GetPropertyData()),
			arcData, this->view->entityTable.GetLayerTable()->GetCurrentLayer()));

		this->view->update((DrawType)(DrawType::DrawCaptureImage | DrawType::DrawPreviewEntities));

		this->UpdateNextListText();

	}
	else if (status == ShDrawArcAction::Status::PickedEnd) {
	
		ShArcData arcData;
		this->GetArcDataWithStartEndAnother(this->start, this->end, point, arcData);

		if (this->CheckValidDataAndUpdateListText(arcData) == false)
			return;

		ShArc *previewArc = dynamic_cast<ShArc*>((*this->view->preview.Begin()));
		previewArc->SetData(arcData);

		ShUpdateListTextEvent event2("");
		this->view->Notify(&event2);

		this->AddEntityAndFinish(previewArc->Clone(), "Arc");
	}

}

void ShDrawArcMethod_StartEndRadius::MouseMoveEvent(QMouseEvent *event, ShActionData &data) {

	ShDrawArcAction::Status status = this->GetStatus();

	if (status == ShDrawArcAction::PickedStart) {
	
		ShPoint3d point = data.GetPoint();

		this->view->rubberBand->SetEnd(point);
		data.AppendDrawType((DrawType)(DrawType::DrawCaptureImage | DrawType::DrawPreviewEntities));

	}
	else if (status == ShDrawArcAction::PickedEnd) {
	
		ShPoint3d point = data.GetPoint();

		ShArcData arcData;
		this->GetArcDataWithStartEndAnother(this->start, this->end, point, arcData);

		ShArc *previewArc = dynamic_cast<ShArc*>((*this->view->preview.Begin()));
		previewArc->SetData(arcData);

		this->view->rubberBand->SetEnd(point);

		data.AppendDrawType((DrawType)(DrawType::DrawCaptureImage | DrawType::DrawPreviewEntities));

	}

}

void ShDrawArcMethod_StartEndRadius::ApplyOrthogonalShape(bool on) {

	if (this->GetStatus() == ShDrawArcAction::Status::PickedNothing)
		return;

	ShPoint3d mouse = this->view->GetCursorPoint();

	if (this->GetStatus() == ShDrawArcAction::Status::PickedStart) {

		if (on == true) {
			ShPoint3d orth;
			this->GetOrthogonal(this->start.x, this->start.y, mouse.x, mouse.y, orth.x, orth.y);
			this->view->rubberBand->SetEnd(orth);
		}
		else {
			this->view->rubberBand->SetEnd(mouse);
		}
	}
	else if (this->GetStatus() == ShDrawArcAction::Status::PickedEnd) {

		if (on == true) {
			ShPoint3d orth;
			this->GetOrthogonal(this->end.x, this->end.y, mouse.x, mouse.y, orth.x, orth.y);
			this->view->rubberBand->SetEnd(orth);
		}
		else {
			this->view->rubberBand->SetEnd(mouse);
		}
		ShPoint3d third = this->view->rubberBand->GetEnd();

		ShArcData arcData;
		this->GetArcDataWithStartEndAnother(this->start, this->end, third, arcData);

		ShArc *previewArc = dynamic_cast<ShArc*>((*this->view->preview.Begin()));
		previewArc->SetData(arcData);
	}


	this->view->update((DrawType)(DrawCaptureImage | DrawPreviewEntities));
}

QString ShDrawArcMethod_StartEndRadius::GetActionHeadTitle() {

	ShDrawArcAction::Status status = this->GetStatus();
	QString str;

	if (status == ShDrawArcAction::Status::PickedNothing)
		str = "Arc >> Specify start point of arc: ";
	else if (status == ShDrawArcAction::Status::PickedStart)
		str = "Arc >> Specify end point of arc: ";
	else if (status == ShDrawArcAction::Status::PickedEnd)
		str = "Arc >> Specify radius of arc: ";

	return str;
}

void ShDrawArcMethod_StartEndRadius::IsAllowedDraftOperation(ShAllowedDraftData &data) {

	ShDrawArcAction::Status status = this->GetStatus();

	if (status == ShDrawArcAction::Status::PickedNothing) {

		data.SetAllowOrthogonal(true);
		data.SetAllowtSnap(true);

		ShPoint3d mouse = this->view->GetCursorPoint();
		data.SetOrthogonalBasePoint(mouse);
		data.SetSnapBasePoint(mouse);
	}
	else if (status == ShDrawArcAction::Status::PickedStart ||
		status == ShDrawArcAction::Status::PickedEnd) {
		data.SetAllowOrthogonal(true);
		data.SetAllowtSnap(true);

		ShPoint3d first = this->view->rubberBand->GetStart();

		data.SetOrthogonalBasePoint(first);
		data.SetSnapBasePoint(first);

	}
}


bool ShDrawArcMethod_StartEndRadius::GetArcDataWithStartEndAnother(const ShPoint3d& start, const ShPoint3d& end,
	const ShPoint3d& another, ShArcData& data) {

	double radius = Math::GetDistance(end.x, end.y, another.x, another.y);

	double angleStartToEnd = Math::GetAbsAngle(start.x, start.y, end.x, end.y);
	double disStartToEnd = Math::GetDistance(start.x, start.y, end.x, end.y);

	double baseSide = disStartToEnd / 2;
	double height = sqrt((radius*radius) - (baseSide*baseSide));

	if (Math::Compare(radius, baseSide) == 1) {
	
		Math::Rotate(angleStartToEnd, start.x, start.y, start.x + baseSide, start.y + height, data.center.x, data.center.y);
		data.radius = radius;
	}
	else {
	
		Math::Rotate(angleStartToEnd, start.x, start.y, start.x + baseSide, start.y, data.center.x, data.center.y);
		data.radius = baseSide;
	}

	data.startAngle = Math::GetAbsAngle(data.center.x, data.center.y, start.x, start.y);
	data.endAngle = Math::GetAbsAngle(data.center.x, data.center.y, end.x, end.y);
	
	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////



ShDrawArcMethod_CenterStartEnd::ShDrawArcMethod_CenterStartEnd(ShDrawArcAction *drawArcAction, ShGraphicView *view)
	:ShDrawArcMethod(drawArcAction, view) {

	
}


ShDrawArcMethod_CenterStartEnd::~ShDrawArcMethod_CenterStartEnd() {


}

void ShDrawArcMethod_CenterStartEnd::LMousePressEvent(QMouseEvent *event, ShActionData &data) {

	ShDrawArcAction::Status &status = this->GetStatus();
	ShPoint3d point = data.GetPoint();
	ShPoint3d nextPoint = data.GetNextPoint();

	if (status == ShDrawArcAction::Status::PickedNothing) {

		status = ShDrawArcAction::Status::PickedCenter;

		this->view->rubberBand = new ShRubberBand(ShLineData(point, nextPoint));

		this->view->update((DrawType)(DrawType::DrawCaptureImage | DrawType::DrawPreviewEntities));

		this->UpdateNextListText();

	}
	else if (status == ShDrawArcAction::Status::PickedCenter) {

		ShArcData arcData;
		arcData.center = this->view->rubberBand->GetStart();
		arcData.radius = Math::GetDistance(arcData.center.x, arcData.center.y, point.x, point.y);
		arcData.startAngle = Math::GetAbsAngle(arcData.center.x, arcData.center.y, point.x, point.y);
		arcData.endAngle = Math::GetAbsAngle(arcData.center.x, arcData.center.y, nextPoint.x, nextPoint.y);

		

		status = ShDrawArcAction::Status::PickedStart;

		this->view->preview.Add(new ShArc(ShPropertyData(*this->view->GetData()->GetPropertyData()),
			arcData, this->view->entityTable.GetLayerTable()->GetCurrentLayer()));

		this->view->update((DrawType)(DrawType::DrawCaptureImage | DrawType::DrawPreviewEntities));

		this->UpdateNextListText();

	}
	else if (status == ShDrawArcAction::Status::PickedStart) {
		
		ShArc *previewArc = dynamic_cast<ShArc*>((*this->view->preview.Begin()));
		ShArcData arcData = previewArc->GetData();
		
		arcData.endAngle = Math::GetAbsAngle(arcData.center.x, arcData.center.y, point.x, point.y);

		if (this->CheckValidDataAndUpdateListText(arcData) == false)
			return;

		previewArc->SetData(arcData);

		ShUpdateListTextEvent event2("");
		this->view->Notify(&event2);

		this->AddEntityAndFinish(previewArc->Clone(), "Arc");

	}

}

void ShDrawArcMethod_CenterStartEnd::MouseMoveEvent(QMouseEvent *event, ShActionData &data) {

	ShDrawArcAction::Status status = this->GetStatus();
	
	if (status == ShDrawArcAction::Status::PickedCenter) {
		
		ShPoint3d point = data.GetPoint();
		
		this->view->rubberBand->SetEnd(point);

		data.AppendDrawType((DrawType)(DrawType::DrawCaptureImage | DrawType::DrawPreviewEntities));
	}
	else if (status == ShDrawArcAction::Status::PickedStart) {
	
		ShPoint3d point = data.GetPoint();

		ShArc *previewArc = dynamic_cast<ShArc*>((*this->view->preview.Begin()));
		ShPoint3d center = previewArc->GetCenter();
		double endAngle = Math::GetAbsAngle(center.x, center.y, point.x, point.y);

		previewArc->SetEndAngle(endAngle);

		this->view->rubberBand->SetEnd(point);

		data.AppendDrawType((DrawType)(DrawType::DrawCaptureImage | DrawType::DrawPreviewEntities));
	}

}

void ShDrawArcMethod_CenterStartEnd::ApplyOrthogonalShape(bool on) {
	
	ShDrawArcAction::Status status = this->GetStatus();

	if (status == ShDrawArcAction::Status::PickedNothing)
		return;

	ShPoint3d mouse = this->view->GetCursorPoint();

	if (status == ShDrawArcAction::Status::PickedCenter) {
		
		if (on == true) {
			ShPoint3d center = this->view->rubberBand->GetStart();
			ShPoint3d orth;
			this->GetOrthogonal(center.x, center.y, mouse.x, mouse.y, orth.x, orth.y);
			this->view->rubberBand->SetEnd(orth);
		}
		else {
			this->view->rubberBand->SetEnd(mouse);
		}

	}
	else if (status == ShDrawArcAction::Status::PickedStart) {
		ShArc *arc = dynamic_cast<ShArc*>((*this->view->preview.Begin()));
		ShPoint3d center = arc->GetCenter();
		double endAngle;

		if (on == true) {
			
			ShPoint3d orth;
			this->GetOrthogonal(center.x, center.y, mouse.x, mouse.y, orth.x, orth.y);
			this->view->rubberBand->SetEnd(orth);
			endAngle = Math::GetAbsAngle(center.x, center.y, orth.x, orth.y);

		}
		else {
			this->view->rubberBand->SetEnd(mouse);
			endAngle = Math::GetAbsAngle(center.x, center.y, mouse.x, mouse.y);
		}

		arc->SetEndAngle(endAngle);
	}

	this->view->update((DrawType)(DrawCaptureImage | DrawPreviewEntities));

}

QString ShDrawArcMethod_CenterStartEnd::GetActionHeadTitle() {

	ShDrawArcAction::Status status = this->GetStatus();
	QString str = "";

	if (status == ShDrawArcAction::Status::PickedNothing)
		str = "Arc >> Specify center point of arc: ";
	else if (status == ShDrawArcAction::Status::PickedCenter)
		str = "Arc >> Specify start point of arc: ";
	else if (status == ShDrawArcAction::Status::PickedStart)
		str = "Arc >> Specify end point of arc: ";

	return str;
	
}

void ShDrawArcMethod_CenterStartEnd::IsAllowedDraftOperation(ShAllowedDraftData &data) {

	ShDrawArcAction::Status status = this->GetStatus();

	if (status == ShDrawArcAction::Status::PickedNothing) {

		data.SetAllowOrthogonal(true);
		data.SetAllowtSnap(true);

		ShPoint3d mouse = this->view->GetCursorPoint();
		data.SetOrthogonalBasePoint(mouse);
		data.SetSnapBasePoint(mouse);
	}
	else if (status == ShDrawArcAction::Status::PickedCenter) {
		data.SetAllowOrthogonal(true);
		data.SetAllowtSnap(true);

		ShPoint3d point = this->view->rubberBand->GetStart();

		data.SetOrthogonalBasePoint(point);
		data.SetSnapBasePoint(point);

	}
	else if (status == ShDrawArcAction::Status::PickedStart) {
		data.SetAllowOrthogonal(true);
		data.SetAllowtSnap(true);

		ShArc *previewArc = dynamic_cast<ShArc*>((*this->view->preview.Begin()));

		data.SetOrthogonalBasePoint(previewArc->GetCenter());
		data.SetSnapBasePoint(previewArc->GetCenter());

	}

}


//////////////////////////////////////////////////////////////////////////////////////////////////////

ShDrawArcMethod_CenterStartAngle::ShDrawArcMethod_CenterStartAngle(ShDrawArcAction *drawArcAction, ShGraphicView *view)
	:ShDrawArcMethod(drawArcAction, view) {

}

ShDrawArcMethod_CenterStartAngle::~ShDrawArcMethod_CenterStartAngle() {

}

void ShDrawArcMethod_CenterStartAngle::LMousePressEvent(QMouseEvent *event, ShActionData& data) {

	ShDrawArcAction::Status &status = this->GetStatus();
	ShPoint3d point = data.GetPoint();
	ShPoint3d nextPoint = data.GetNextPoint();

	if (status == ShDrawArcAction::Status::PickedNothing) {

		this->center = point;

		status = ShDrawArcAction::Status::PickedCenter;

		this->view->rubberBand = new ShRubberBand(ShLineData(point, nextPoint));

		this->view->update((DrawType)(DrawType::DrawCaptureImage | DrawType::DrawPreviewEntities));

		this->UpdateNextListText();

	}
	else if (status == ShDrawArcAction::Status::PickedCenter) {

		ShArcData arcData;
		arcData.center = this->center;
		arcData.radius = Math::GetDistance(this->center.x, this->center.y, point.x, point.y);
		arcData.startAngle = Math::GetAbsAngle(this->center.x, this->center.y, point.x, point.y);
		double angleCenterToNext = Math::GetAbsAngle(this->center.x, this->center.y, nextPoint.x, nextPoint.y);
		arcData.endAngle = Math::AddAngle(arcData.startAngle, angleCenterToNext);

		if (this->CheckValidDataAndUpdateListText(arcData) == false)
			return;

		status = ShDrawArcAction::Status::PickedStart;
		
		this->start = point;

		this->view->preview.Add(new ShArc(ShPropertyData(*this->view->GetData()->GetPropertyData()),
			arcData, this->view->entityTable.GetLayerTable()->GetCurrentLayer()));

		this->view->update((DrawType)(DrawType::DrawCaptureImage | DrawType::DrawPreviewEntities));

		this->UpdateNextListText();

	}
	else if (status == ShDrawArcAction::Status::PickedStart) {

		ShArc *previewArc = dynamic_cast<ShArc*>((*this->view->preview.Begin()));
		ShArcData arcData = previewArc->GetData();

		double angleCenterToPoint = Math::GetAbsAngle(this->center.x, this->center.y, point.x, point.y);
		arcData.endAngle = Math::AddAngle(arcData.startAngle, angleCenterToPoint);	
		
		if (this->CheckValidDataAndUpdateListText(arcData) == false)
			return;

		previewArc->SetData(arcData);

		ShUpdateListTextEvent event2("");
		this->view->Notify(&event2);

		this->AddEntityAndFinish(previewArc->Clone(), "Arc");

	}
}

void ShDrawArcMethod_CenterStartAngle::MouseMoveEvent(QMouseEvent *event, ShActionData& data) {

	ShDrawArcAction::Status status = this->GetStatus();

	if (status == ShDrawArcAction::Status::PickedCenter) {

		ShPoint3d point = data.GetPoint();

		this->view->rubberBand->SetEnd(point);

		data.AppendDrawType((DrawType)(DrawType::DrawCaptureImage | DrawType::DrawPreviewEntities));
	}
	else if (status == ShDrawArcAction::Status::PickedStart) {

		ShPoint3d point = data.GetPoint();

		ShArc *previewArc = dynamic_cast<ShArc*>((*this->view->preview.Begin()));

		double angleCenterToPoint = Math::GetAbsAngle(this->center.x, this->center.y, point.x, point.y);
		double endAngle = Math::AddAngle(previewArc->GetStartAngle(), angleCenterToPoint);
		previewArc->SetEndAngle(endAngle);

		this->view->rubberBand->SetEnd(point);

		data.AppendDrawType((DrawType)(DrawType::DrawCaptureImage | DrawType::DrawPreviewEntities));
	}
}

void ShDrawArcMethod_CenterStartAngle::ApplyOrthogonalShape(bool on) {

	ShDrawArcAction::Status status = this->GetStatus();

	if (status == ShDrawArcAction::Status::PickedNothing)
		return;

	ShPoint3d mouse = this->view->GetCursorPoint();

	if (status == ShDrawArcAction::Status::PickedCenter) {

		if (on == true) {
			ShPoint3d center = this->view->rubberBand->GetStart();
			ShPoint3d orth;
			this->GetOrthogonal(center.x, center.y, mouse.x, mouse.y, orth.x, orth.y);
			this->view->rubberBand->SetEnd(orth);
		}
		else {
			this->view->rubberBand->SetEnd(mouse);
		}

	}
	else if (status == ShDrawArcAction::Status::PickedStart) {
		ShArc *arc = dynamic_cast<ShArc*>((*this->view->preview.Begin()));
		ShPoint3d center = arc->GetCenter();
		double angleCenterToPoint;

		if (on == true) {

			ShPoint3d orth;
			this->GetOrthogonal(center.x, center.y, mouse.x, mouse.y, orth.x, orth.y);
			this->view->rubberBand->SetEnd(orth);
			angleCenterToPoint = Math::GetAbsAngle(center.x, center.y, orth.x, orth.y);

		}
		else {
			this->view->rubberBand->SetEnd(mouse);
			angleCenterToPoint = Math::GetAbsAngle(center.x, center.y, mouse.x, mouse.y);
		}


		arc->SetEndAngle(Math::AddAngle(arc->GetStartAngle(), angleCenterToPoint));
	}

	this->view->update((DrawType)(DrawCaptureImage | DrawPreviewEntities));
}

QString ShDrawArcMethod_CenterStartAngle::GetActionHeadTitle() {

	ShDrawArcAction::Status status = this->GetStatus();
	QString str = "";

	if (status == ShDrawArcAction::Status::PickedNothing)
		str = "Arc >> Specify center point of arc: ";
	else if (status == ShDrawArcAction::Status::PickedStart)
		str = "Arc >> Specify start point of arc: ";
	else if (status == ShDrawArcAction::Status::PickedCenter)
		str = "Arc >> Specify included angle: ";

	return str;
}

void ShDrawArcMethod_CenterStartAngle::IsAllowedDraftOperation(ShAllowedDraftData &data) {

	ShDrawArcAction::Status status = this->GetStatus();

	if (status == ShDrawArcAction::Status::PickedNothing) {

		data.SetAllowOrthogonal(true);
		data.SetAllowtSnap(true);

		ShPoint3d mouse = this->view->GetCursorPoint();
		data.SetOrthogonalBasePoint(mouse);
		data.SetSnapBasePoint(mouse);
	}
	else if (status == ShDrawArcAction::Status::PickedCenter) {
		data.SetAllowOrthogonal(true);
		data.SetAllowtSnap(true);

		ShPoint3d point = this->view->rubberBand->GetStart();

		data.SetOrthogonalBasePoint(point);
		data.SetSnapBasePoint(point);

	}
	else if (status == ShDrawArcAction::Status::PickedStart) {
		data.SetAllowOrthogonal(true);
		data.SetAllowtSnap(true);

		ShArc *previewArc = dynamic_cast<ShArc*>((*this->view->preview.Begin()));

		data.SetOrthogonalBasePoint(previewArc->GetCenter());
		data.SetSnapBasePoint(previewArc->GetCenter());
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////


ShDrawArcMethod_CenterStartLength::ShDrawArcMethod_CenterStartLength(ShDrawArcAction *drawArcAction, ShGraphicView *view)
	:ShDrawArcMethod(drawArcAction, view) {

}

ShDrawArcMethod_CenterStartLength::~ShDrawArcMethod_CenterStartLength() {

}

void ShDrawArcMethod_CenterStartLength::LMousePressEvent(QMouseEvent *event, ShActionData& data) {

	ShDrawArcAction::Status &status = this->GetStatus();
	ShPoint3d point = data.GetPoint();
	ShPoint3d nextPoint = data.GetNextPoint();

	if (status == ShDrawArcAction::Status::PickedNothing) {

		this->center = point;

		status = ShDrawArcAction::Status::PickedCenter;

		this->view->rubberBand = new ShRubberBand(ShLineData(point, nextPoint));

		this->view->update((DrawType)(DrawType::DrawCaptureImage | DrawType::DrawPreviewEntities));

		this->UpdateNextListText();

	}
	else if (status == ShDrawArcAction::Status::PickedCenter) {

		this->start = point;

		status = ShDrawArcAction::Status::PickedStart;

		this->view->rubberBand->SetStart(this->start);
		this->view->rubberBand->SetEnd(nextPoint);

		ShArcData arcData;
		this->GetArcDataWithCenterStartLength(this->center, this->start,
			Math::GetDistance(this->start.x, this->start.y, nextPoint.x, nextPoint.y),
			arcData);


		this->view->preview.Add(new ShArc(ShPropertyData(*this->view->GetData()->GetPropertyData()),
			arcData, this->view->entityTable.GetLayerTable()->GetCurrentLayer()));

		this->view->update((DrawType)(DrawType::DrawCaptureImage | DrawType::DrawPreviewEntities));

		this->UpdateNextListText();

	}
	else if (status == ShDrawArcAction::Status::PickedStart) {

		ShArcData arcData;
		this->GetArcDataWithCenterStartLength(this->center, this->start,
			Math::GetDistance(this->start.x, this->start.y, point.x, point.y),
			arcData);


		if (this->CheckValidDataAndUpdateListText(arcData) == false)
			return;

		ShArc *previewArc = dynamic_cast<ShArc*>((*this->view->preview.Begin()));
		previewArc->SetData(arcData);

		ShUpdateListTextEvent event2("");
		this->view->Notify(&event2);

		this->AddEntityAndFinish(previewArc->Clone(), "Arc");

	}
}

void ShDrawArcMethod_CenterStartLength::MouseMoveEvent(QMouseEvent *event, ShActionData& data) {

	ShDrawArcAction::Status status = this->GetStatus();

	if (status == ShDrawArcAction::Status::PickedCenter) {

		ShPoint3d point = data.GetPoint();

		this->view->rubberBand->SetEnd(point);

		data.AppendDrawType((DrawType)(DrawType::DrawCaptureImage | DrawType::DrawPreviewEntities));
	}
	else if (status == ShDrawArcAction::Status::PickedStart) {

		ShPoint3d point = data.GetPoint();

		ShArcData arcData;
		this->GetArcDataWithCenterStartLength(this->center, this->start,
			Math::GetDistance(this->start.x, this->start.y, point.x, point.y),
			arcData);

		ShArc *previewArc = dynamic_cast<ShArc*>((*this->view->preview.Begin()));
		previewArc->SetData(arcData);

	
		this->view->rubberBand->SetEnd(point);

		data.AppendDrawType((DrawType)(DrawType::DrawCaptureImage | DrawType::DrawPreviewEntities));
	}
}

void ShDrawArcMethod_CenterStartLength::ApplyOrthogonalShape(bool on) {

	ShDrawArcAction::Status status = this->GetStatus();

	if (status == ShDrawArcAction::Status::PickedNothing)
		return;

	ShPoint3d mouse = this->view->GetCursorPoint();

	if (status == ShDrawArcAction::Status::PickedCenter) {

		if (on == true) {
	
			ShPoint3d orth;
			this->GetOrthogonal(this->center.x, this->center.y, mouse.x, mouse.y, orth.x, orth.y);
			this->view->rubberBand->SetEnd(orth);
		}
		else {
			this->view->rubberBand->SetEnd(mouse);
		}

	}
	else if (status == ShDrawArcAction::Status::PickedStart) {
		
		if (on == true) {

			ShPoint3d orth;
			this->GetOrthogonal(this->start.x, this->start.y, mouse.x, mouse.y, orth.x, orth.y);
			this->view->rubberBand->SetEnd(orth);
		}
		else 
			this->view->rubberBand->SetEnd(mouse);

		ShPoint3d end = this->view->rubberBand->GetEnd();

		ShArcData arcData;
		this->GetArcDataWithCenterStartLength(this->center, this->start,
			Math::GetDistance(this->start.x, this->start.y, end.x, end.y),
			arcData);

		ShArc *arc = dynamic_cast<ShArc*>((*this->view->preview.Begin()));
		arc->SetData(arcData);
	}

	this->view->update((DrawType)(DrawCaptureImage | DrawPreviewEntities));
}

QString ShDrawArcMethod_CenterStartLength::GetActionHeadTitle() {

	ShDrawArcAction::Status status = this->GetStatus();
	QString str = "";

	if (status == ShDrawArcAction::Status::PickedNothing)
		str = "Arc >> Specify center point of arc: ";
	else if (status == ShDrawArcAction::Status::PickedCenter)
		str = "Arc >> Specify start point of arc: ";
	else if (status == ShDrawArcAction::Status::PickedStart)
		str = "Arc >> Specify length of chord: ";

	return str;
}

void ShDrawArcMethod_CenterStartLength::IsAllowedDraftOperation(ShAllowedDraftData &data) {

	ShDrawArcAction::Status status = this->GetStatus();

	if (status == ShDrawArcAction::Status::PickedNothing) {

		data.SetAllowOrthogonal(true);
		data.SetAllowtSnap(true);

		ShPoint3d mouse = this->view->GetCursorPoint();
		data.SetOrthogonalBasePoint(mouse);
		data.SetSnapBasePoint(mouse);
	}
	else if (status == ShDrawArcAction::Status::PickedCenter ||
		status == ShDrawArcAction::Status::PickedStart) {
		data.SetAllowOrthogonal(true);
		data.SetAllowtSnap(true);

		ShPoint3d point = this->view->rubberBand->GetStart();

		data.SetOrthogonalBasePoint(point);
		data.SetSnapBasePoint(point);

	}
	
}

bool ShDrawArcMethod_CenterStartLength::GetArcDataWithCenterStartLength(const ShPoint3d& center, const ShPoint3d& start,
	double length, ShArcData &arcData) {

	double radius = Math::GetDistance(center.x, center.y, start.x, start.y);
	double height = length / 2;
	double angle = asin(height / radius) * 180 / 3.1415926535897;

	
	arcData.radius = radius;
	arcData.center = center;
	arcData.startAngle= Math::GetAbsAngle(center.x, center.y, start.x, start.y);

	if (Math::Compare(radius, length) == 1 || Math::Compare(radius, length) == 0) {
		arcData.endAngle = Math::AddAngle(arcData.startAngle, angle * 2);
		return true;
	}

	arcData.endAngle = arcData.startAngle;
	return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////


