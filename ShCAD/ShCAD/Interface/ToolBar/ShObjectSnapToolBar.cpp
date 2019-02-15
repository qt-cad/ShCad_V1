

#include "ShObjectSnapToolBar.h"
#include "ShDirectoryManager.h"
#include <qbitmap.h>
ShObjectSnapToolBar::ShObjectSnapToolBar(QWidget *parent)
	:QToolBar(parent) {

	this->setIconSize(QSize(20, 20));

	QString path=ShDirectoryManager::GetImageUiPath();
	QIcon icon;

	this->GetIcon(path + "\\Temporary track point.png", icon);
	QAction *action=this->addAction(icon, "Temporary track point");
	connect(action, &QAction::triggered, this, &ShObjectSnapToolBar::TemporaryTrackPointClicked);

	this->addSeparator();

	this->GetIcon(path + "\\End point.png",icon);
	action = this->addAction(icon, "End point");
	connect(action, &QAction::triggered, this, &ShObjectSnapToolBar::EndPointClicked);

	this->GetIcon(path + "\\Mid point.png", icon);
	action = this->addAction(icon, "Mid point");
	connect(action, &QAction::triggered, this, &ShObjectSnapToolBar::MidPointClicked);

	this->GetIcon(path + "\\Intersection point.png", icon);
	action=this->addAction(icon, "Intersection point");
	connect(action, &QAction::triggered, this, &ShObjectSnapToolBar::IntersectionPointClicked);

	this->GetIcon(path + "\\Apparent intersection point.png", icon);
	action = this->addAction(icon, "Apparent intersection point");
	connect(action, &QAction::triggered, this, &ShObjectSnapToolBar::ApparentIntersectionPointClicked);

	this->GetIcon(path + "\\Extension point", icon);
	action = this->addAction(icon, "Extension point");
	connect(action, &QAction::triggered, this, &ShObjectSnapToolBar::ExtensionPointClicked);

	this->addSeparator();

	this->GetIcon(path + "\\Center point.png", icon);
	action = this->addAction(icon, "Center point");
	connect(action, &QAction::triggered, this, &ShObjectSnapToolBar::CenterPointClicked);

	this->GetIcon(path + "\\Quadrant point.png", icon);
	action = this->addAction(icon, "Quadrant point");
	connect(action, &QAction::triggered, this, &ShObjectSnapToolBar::QuadrantPointClicked);

	this->GetIcon(path + "\\Tangent point.png", icon);
	action = this->addAction(icon, "Tangent point");
	connect(action, &QAction::triggered, this, &ShObjectSnapToolBar::TangentPointClicked);

	this->addSeparator();


	this->GetIcon(path + "\\Perpendicular.png", icon);
	action = this->addAction(icon, "Perpendicular");
	connect(action, &QAction::triggered, this, &ShObjectSnapToolBar::PerpendicularClicked);

}

ShObjectSnapToolBar::~ShObjectSnapToolBar() {

}

void ShObjectSnapToolBar::GetIcon(const QString& filePath, QIcon& icon) {
	
	QPixmap pix(filePath);
	QBitmap mask = pix.createMaskFromColor(QColor(255, 255, 255), Qt::MaskMode::MaskInColor);
	pix.setMask(mask);

	icon = QIcon(pix);

}


#include "Singleton Pattern\ShWidgetManager.h"
#include "Interface\ShGraphicView.h"
#include "ActionHandler\ShActionHandler.h"
#include "ShVariable.h"
void ShObjectSnapToolBar::TemporaryTrackPointClicked() {
	

}


void ShObjectSnapToolBar::EndPointClicked() {

	ShWidgetManager *manager = ShWidgetManager::GetInstance();

	if (manager->GetActivatedWidget() != 0)
		manager->GetActivatedWidget()->currentAction->SetObjectSnap(ObjectSnap::ObjectSnapEndPoint);

}

void ShObjectSnapToolBar::MidPointClicked() {

	ShWidgetManager *manager = ShWidgetManager::GetInstance();

	if (manager->GetActivatedWidget() != 0)
		manager->GetActivatedWidget()->currentAction->SetObjectSnap(ObjectSnap::ObjectSnapMidPoint);
}


void ShObjectSnapToolBar::IntersectionPointClicked() {
	

}

void ShObjectSnapToolBar::ApparentIntersectionPointClicked() {

}

void ShObjectSnapToolBar::ExtensionPointClicked() {


}

void ShObjectSnapToolBar::CenterPointClicked() {


}

void ShObjectSnapToolBar::QuadrantPointClicked() {


}

void ShObjectSnapToolBar::TangentPointClicked() {


}

void ShObjectSnapToolBar::PerpendicularClicked() {

	ShWidgetManager *manager = ShWidgetManager::GetInstance();

	if (manager->GetActivatedWidget() != 0)
		manager->GetActivatedWidget()->currentAction->SetObjectSnap(ObjectSnap::ObjectSnapPerpendicular);
}