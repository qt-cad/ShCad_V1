

#include "ShHomeTab.h"
#include "ShDirectoryManager.h"
#include <qpixmap.h>
#include <qbitmap.h>
#include <qmenu.h>
#include "Interface\ShGraphicView.h"
#include "ShNotifyEvent.h"
#include "Singleton Pattern\ShWidgetManager.h"
#include "Singleton Pattern\ShChangeManager.h"
#include "Interface\Items\ShCustomizedWidget.h"
#include "Interface\Items\ShRibbonButton.h"
ShHomeTab::ShHomeTab(const QString &title, QWidget *parent)
	:ShRibbonTab(title, parent) {

	this->drawColumn = new ShDrawColumn(this, "Draw", 100);
	this->AddColumn(this->drawColumn);

	this->modifyColumn = new ShModifyColumn(this, "Modify", 150);
	this->AddColumn(this->modifyColumn);

	this->propertyColumn = new ShPropertyColumn(this, "Property", 250);
	this->AddColumn(this->propertyColumn);

	this->layerColumn = new ShLayerColumn(this, "Layer", 250);
	this->AddColumn(this->layerColumn);
}

ShHomeTab::~ShHomeTab() {

}



/////////////////////////////////////////////////////////////////////////

ShDrawColumn::ShDrawColumn(QWidget *parent, const QString &title, int width)
	:ShColumnInRibbonTab(parent, title, width) {



	this->InitLineButton();
	this->circleButton = new ShRibbonCircleButton(this->layoutWidget);
	this->arcButton = new ShRibbonArcButton(this->layoutWidget);
	this->polyButton = new ShRibbonPolyLineButton(this->layoutWidget);


}

ShDrawColumn::~ShDrawColumn() {


}

void ShDrawColumn::resizeEvent(QResizeEvent *event) {

	ShColumnInRibbonTab::resizeEvent(event);

	int width = this->layoutWidget->width()/2;
	int height = this->layoutWidget->height()/3;

	this->lineButton->setGeometry(0, 0, width, height);
	this->circleButton->setGeometry(width, 0, width, height);

	this->arcButton->setGeometry(0, height, width, height);
	this->polyButton->setGeometry(width, height, width, height);


}

#include "Strategy Pattern\ShChangeCurrentActionStrategy.h"
void ShDrawColumn::LineButtonClicked() {
	qDebug("LineButtonClicked");

	if (ShWidgetManager::GetInstance()->GetActivatedWidget() == 0) {
		qDebug("no activated widget");
		return;
	}

	ShChangeCurrentActionCancelCurrent strategy(ActionType::ActionDrawLine);
	ShWidgetManager::GetInstance()->GetActivatedWidget()->ChangeCurrentAction(strategy);
	
}


void ShDrawColumn::InitLineButton() {

	QString path = ShDirectoryManager::GetImageUiPath();

	QPixmap pix(path + "\\Line.png");
	QBitmap mask = pix.createMaskFromColor(QColor(255, 255, 255), Qt::MaskMode::MaskInColor);
	pix.setMask(mask);


	QIcon icon(pix);
	this->lineButton = new ShButtonWithMenuPopup(this->layoutWidget);
	this->lineButton->SetIcon(icon);
	this->lineButton->setToolTip("Line \nCreates straight line segments");

	QMenu *menu = new QMenu(this->lineButton);
	menu->addAction("Construction Line");
	menu->addAction("Ray");
	this->lineButton->SetMenu(menu);




	connect(this->lineButton, &ShButtonWithMenuPopup::pressed, this, &ShDrawColumn::LineButtonClicked);

}

/////////////////////////////////////////////////////////////////////////////////////////////////////////

ShModifyColumn::ShModifyColumn(QWidget *parent, const QString &title, int width)
	:ShColumnInRibbonTab(parent, title, width) {

	QString path = ShDirectoryManager::GetImageUiPath();

	QPixmap pix(path + "\\Modify\\Move.png");
	QBitmap mask = pix.createMaskFromColor(QColor(255, 255, 255), Qt::MaskMode::MaskInColor);
	pix.setMask(mask);
	QIcon icon(pix);
	this->moveButton = new ShButton(this->layoutWidget);
	this->moveButton->setIcon(icon);



	pix = QPixmap(path + "\\Modify\\Copy.png");
	mask = pix.createMaskFromColor(QColor(255, 255, 255), Qt::MaskMode::MaskInColor);
	pix.setMask(mask);
	icon = QIcon(pix);
	this->copyButton = new ShButton(this->layoutWidget);
	this->copyButton->setIcon(icon);


	pix = QPixmap(path + "\\Modify\\Stretch.png");
	mask = pix.createMaskFromColor(QColor(255, 255, 255), Qt::MaskMode::MaskInColor);
	pix.setMask(mask);
	icon = QIcon(pix);
	this->stretchButton = new ShButton(this->layoutWidget);
	this->stretchButton->setIcon(icon);



	pix = QPixmap(path + "\\Modify\\Rotate.png");
	mask = pix.createMaskFromColor(QColor(255, 255, 255), Qt::MaskMode::MaskInColor);
	pix.setMask(mask);
	icon = QIcon(pix);
	this->rotateButton = new ShButton(this->layoutWidget);
	this->rotateButton->setIcon(icon);


	pix = QPixmap(path + "\\Modify\\Mirror.png");
	mask = pix.createMaskFromColor(QColor(255, 255, 255), Qt::MaskMode::MaskInColor);
	pix.setMask(mask);
	icon = QIcon(pix);
	this->mirrorButton = new ShButton(this->layoutWidget);
	this->mirrorButton->setIcon(icon);

	pix = QPixmap(path + "\\Modify\\Scale.png");
	mask = pix.createMaskFromColor(QColor(255, 255, 255), Qt::MaskMode::MaskInColor);
	pix.setMask(mask);
	icon = QIcon(pix);
	this->scaleButton = new ShButton(this->layoutWidget);
	this->scaleButton->setIcon(icon);


	pix = QPixmap(path + "\\Modify\\Erase.png");
	mask = pix.createMaskFromColor(QColor(255, 255, 255), Qt::MaskMode::MaskInColor);
	pix.setMask(mask);
	icon = QIcon(pix);
	this->eraseButton = new ShButton(this->layoutWidget);
	this->eraseButton->setIcon(icon);

	pix = QPixmap(path + "\\Modify\\Extend.png");
	mask = pix.createMaskFromColor(QColor(255, 255, 255), Qt::MaskMode::MaskInColor);
	pix.setMask(mask);
	icon = QIcon(pix);
	this->extendButton = new ShButton(this->layoutWidget);
	this->extendButton->setIcon(icon);

	pix = QPixmap(path + "\\Modify\\Trim.png");
	mask = pix.createMaskFromColor(QColor(255, 255, 255), Qt::MaskMode::MaskInColor);
	pix.setMask(mask);
	icon = QIcon(pix);
	this->trimButton = new ShButton(this->layoutWidget);
	this->trimButton->setIcon(icon);

	connect(this->moveButton, &ShButton::released, this, &ShModifyColumn::MoveButtonClicked);
	connect(this->copyButton, &ShButton::released, this, &ShModifyColumn::CopyButtonClicked);
	connect(this->stretchButton, &ShButton::released, this, &ShModifyColumn::StretchButtonClicked);
	connect(this->rotateButton, &ShButton::released, this, &ShModifyColumn::RotateButtonClicked);
	connect(this->mirrorButton, &ShButton::released, this, &ShModifyColumn::MirrorButtonClicked);
	connect(this->scaleButton, &ShButton::released, this, &ShModifyColumn::ScaleButtonClicked);
	connect(this->eraseButton, &ShButton::released, this, &ShModifyColumn::EraseButtonClicked);
	connect(this->extendButton, &ShButton::released, this, &ShModifyColumn::ExtendButtonClicked);
	connect(this->trimButton, &ShButton::released, this, &ShModifyColumn::TrimButtonClicked);

}

ShModifyColumn::~ShModifyColumn() {


}

void ShModifyColumn::resizeEvent(QResizeEvent *event) {

	ShColumnInRibbonTab::resizeEvent(event);

	//3row 3column
	int width = this->layoutWidget->width() / 3;
	int height = this->layoutWidget->height() / 3;



	this->moveButton->setGeometry(0, 0, width, height);
	this->copyButton->setGeometry(0, height, width, height);
	this->stretchButton->setGeometry(0, height * 2, width, height);

	this->rotateButton->setGeometry(width, 0, width, height);
	this->mirrorButton->setGeometry(width, height, width, height);
	this->scaleButton->setGeometry(width, height * 2, width, height);

	this->eraseButton->setGeometry(width * 2, 0, width, height);
	this->extendButton->setGeometry(width * 2, height, width, height);
	this->trimButton->setGeometry(width * 2, height * 2, width, height);

}

void ShModifyColumn::MoveButtonClicked() {

	if (ShWidgetManager::GetInstance()->GetActivatedWidget() == 0) {
		qDebug("no activated widget");
		return;
	}

	ShChangeCurrentActionCancelCurrent strategy(ActionType::ActionModifyMove);
	ShWidgetManager::GetInstance()->GetActivatedWidget()->ChangeCurrentAction(strategy);
}

void ShModifyColumn::CopyButtonClicked() {

	if (ShWidgetManager::GetInstance()->GetActivatedWidget() == 0) {
		qDebug("no activated widget");
		return;
	}

	ShChangeCurrentActionCancelCurrent strategy(ActionType::ActionModifyCopy);
	ShWidgetManager::GetInstance()->GetActivatedWidget()->ChangeCurrentAction(strategy);
}

void ShModifyColumn::StretchButtonClicked() {

	if (ShWidgetManager::GetInstance()->GetActivatedWidget() == 0) {
		qDebug("no activated widget");
		return;
	}

	ShChangeCurrentActionCancelCurrent strategy(ActionType::ActionModifyStretch);
	ShWidgetManager::GetInstance()->GetActivatedWidget()->ChangeCurrentAction(strategy);
}

void ShModifyColumn::RotateButtonClicked() {

	if (ShWidgetManager::GetInstance()->GetActivatedWidget() == 0) {
		qDebug("no activated widget");
		return;
	}

	ShChangeCurrentActionCancelCurrent strategy(ActionType::ActionModifyRotate);
	ShWidgetManager::GetInstance()->GetActivatedWidget()->ChangeCurrentAction(strategy);
}

void ShModifyColumn::MirrorButtonClicked() {

	if (ShWidgetManager::GetInstance()->GetActivatedWidget() == 0) {
		qDebug("no activated widget");
		return;
	}

	ShChangeCurrentActionCancelCurrent strategy(ActionType::ActionModifyMirror);
	ShWidgetManager::GetInstance()->GetActivatedWidget()->ChangeCurrentAction(strategy);
}

void ShModifyColumn::ScaleButtonClicked() {

	if (ShWidgetManager::GetInstance()->GetActivatedWidget() == 0) {
		qDebug("no activated widget");
		return;
	}

	ShChangeCurrentActionCancelCurrent strategy(ActionType::ActionModifyScale);
	ShWidgetManager::GetInstance()->GetActivatedWidget()->ChangeCurrentAction(strategy);
}

void ShModifyColumn::EraseButtonClicked() {

	if (ShWidgetManager::GetInstance()->GetActivatedWidget() == 0) {
		qDebug("no activated widget");
		return;
	}

	ShChangeCurrentActionCancelCurrent strategy(ActionType::ActionModifyErase);
	ShWidgetManager::GetInstance()->GetActivatedWidget()->ChangeCurrentAction(strategy);
}

void ShModifyColumn::ExtendButtonClicked() {

	if (ShWidgetManager::GetInstance()->GetActivatedWidget() == 0) {
		qDebug("no activated widget");
		return;
	}

	ShChangeCurrentActionCancelCurrent strategy(ActionType::ActionModifyExtend);
	ShWidgetManager::GetInstance()->GetActivatedWidget()->ChangeCurrentAction(strategy);
}

void ShModifyColumn::TrimButtonClicked() {

	if (ShWidgetManager::GetInstance()->GetActivatedWidget() == 0) {
		qDebug("no activated widget");
		return;
	}

	ShChangeCurrentActionCancelCurrent strategy(ActionType::ActionModifyTrim);
	ShWidgetManager::GetInstance()->GetActivatedWidget()->ChangeCurrentAction(strategy);

}

/////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "Interface\Items\ShColorComboBox.h"
#include "Singleton Pattern\ShColorComboList.h"
#include "Interface\Items\ShLineStyleComboBox.h"
#include "Singleton Pattern\ShLineStyleComboList.h"
ShPropertyColumn::ShPropertyColumn(QWidget *parent, const QString &title, int width)
	:ShColumnInRibbonTab(parent, title, width){

	ShChangeManager *manager = ShChangeManager::GetInstance();
	manager->Register(this);


	this->colorCustomButton = new ShButton(this);

	QString path = ShDirectoryManager::GetImageUiPath();
	QPixmap pix(path + "\\SelectColor.png");
	QBitmap mask = pix.createMaskFromColor(QColor(255, 255, 255), Qt::MaskMode::MaskInColor);
	pix.setMask(mask);

	this->colorCustomButton->setIcon(QIcon(pix));

	connect(this->colorCustomButton, &QPushButton::released, this, &ShPropertyColumn::ColorCustomButtonClicked);


	this->colorCombo = new ShColorComboBox(this);

	connect(this->colorCombo, SIGNAL(ColorChanged(const ShColor&)), this, SLOT(ColorSelChanged(const ShColor&)));


	this->lineStyleCombo = new ShLineStyleComboBox(this);

	connect(this->lineStyleCombo, SIGNAL(LineStyleChanged(const ShLineStyle&)), this, SLOT(LineStyleSelChanged(const ShLineStyle&)));

}

ShPropertyColumn::~ShPropertyColumn() {


}


void ShPropertyColumn::resizeEvent(QResizeEvent *event) {


	ShColumnInRibbonTab::resizeEvent(event);

	int height = this->layoutWidget->height();


	this->colorCustomButton->setGeometry(0, 0, height / 3 - 2, height / 3 - 2);

	this->colorCombo->setGeometry(height / 3 + 2, 3, 150, height / 3 - 8);


	this->lineStyleCombo->setGeometry(height / 3 + 2, height / 3, 150, height / 3 - 8);

}


void ShPropertyColumn::ColorCustomButtonClicked() {

	this->colorCombo->OpenColorPickDialog();
}

void ShPropertyColumn::SetColorComboInfo(const ShColor& blockColor, const ShColor& layerColor, const ShColor& current) {

	this->colorCombo->SetBlockColor(blockColor);
	this->colorCombo->SetLayerColor(layerColor);

	if (current.GetType() == ShColor::Type::ByBlock)
		this->colorCombo->Synchronize(0);
	else if (current.GetType() == ShColor::Type::ByLayer)
		this->colorCombo->Synchronize(1);
	else {
		ShColorComboList *list = ShColorComboList::GetInstance();

		int index = list->Search(current);
		this->colorCombo->Synchronize(index + 2);
	}

}

void ShPropertyColumn::SetLineStyleComboInfo(const ShLineStyle& blockLineStyle, const ShLineStyle& layerLineStyle,
	const ShLineStyle& current) {

	this->lineStyleCombo->SetBlockLineStyle(blockLineStyle);
	this->lineStyleCombo->SetLayerLineStyle(layerLineStyle);

	if (current.GetType() == ShLineStyle::Type::ByBlock)
		this->lineStyleCombo->Synchronize(0);
	else if (current.GetType() == ShLineStyle::Type::ByLayer)
		this->lineStyleCombo->Synchronize(1);
	else {
		ShLineStyleComboList *list = ShLineStyleComboList::GetInstance();
		int index = list->Search(current);
		this->lineStyleCombo->Synchronize(index + 2);
	}

}



void ShPropertyColumn::SynchronizeColorCombo(int colorComboIndex) {

	this->colorCombo->Synchronize(colorComboIndex);
}

void ShPropertyColumn::SynchronizeColorCombo(const ShColor& color) {

	if (color.GetType() == ShColor::Type::ByBlock)
		this->colorCombo->Synchronize(0);
	else if (color.GetType() == ShColor::Type::ByLayer)
		this->colorCombo->Synchronize(1);
	else {
		ShColorComboList *list = ShColorComboList::GetInstance();

		int index = list->Search(color);
		this->colorCombo->Synchronize(index + 2);
	}
}


void ShPropertyColumn::Update(ShActivatedWidgetChangedEvent *event) {

	ShGraphicView *newWidget = event->GetNewWidget();

	this->SetColorComboInfo(newWidget->GetData()->GetBlockData()->GetColor(),
		newWidget->GetData()->GetLayerData()->GetColor(),
		newWidget->GetData()->GetPropertyData()->GetColor());

	this->SetLineStyleComboInfo(newWidget->GetData()->GetBlockData()->GetLineStyle(),
		newWidget->GetData()->GetLayerData()->GetLineStyle(),
		newWidget->GetData()->GetPropertyData()->GetLineStyle());

	////////////////////////////////////////////////////////////////////////////

	if (newWidget->GetCurrentActionType() == ActionType::ActionDefault) {
		this->colorCombo->setDisabled(false);
		this->lineStyleCombo->setDisabled(false);
	}
	else {
		this->colorCombo->setDisabled(true);
		this->lineStyleCombo->setDisabled(true);
	}

}

void ShPropertyColumn::Update(ShCurrentLayerChangedEvent *event) {

	this->colorCombo->SetLayerColor(event->GetCurrentLayer()->GetData().GetPropertyData().GetColor());
	int index = this->colorCombo->GetColorComboIndex();
	this->colorCombo->Synchronize(index);



	this->lineStyleCombo->SetLayerLineStyle(event->GetCurrentLayer()->GetData().GetPropertyData().GetLineStyle());
	index = this->lineStyleCombo->GetLineStyleComboIndex();
	this->lineStyleCombo->Synchronize(index);

}

void ShPropertyColumn::Update(ShLayerComboSelChangedEvent *event) {

	this->colorCombo->SetLayerColor(event->GetCurrentLayer()->GetData().GetPropertyData().GetColor());
	int index = this->colorCombo->GetColorComboIndex();
	this->colorCombo->Synchronize(index);



	this->lineStyleCombo->SetLayerLineStyle(event->GetCurrentLayer()->GetData().GetPropertyData().GetLineStyle());
	index = this->lineStyleCombo->GetLineStyleComboIndex();
	this->lineStyleCombo->Synchronize(index);

}

void ShPropertyColumn::Update(ShLayerDataChangedEvent *event) {
	
	this->colorCombo->SetLayerColor(event->GetCurrentLayer()->GetData().GetPropertyData().GetColor());
	int index = this->colorCombo->GetColorComboIndex();
	this->colorCombo->Synchronize(index);



	this->lineStyleCombo->SetLayerLineStyle(event->GetCurrentLayer()->GetData().GetPropertyData().GetLineStyle());
	index = this->lineStyleCombo->GetLineStyleComboIndex();
	this->lineStyleCombo->Synchronize(index);

}

void ShPropertyColumn::Update(ShCurrentActionChangedEvent *event) {

	if (event->GetActionType() == ActionType::ActionDefault) {
		this->colorCombo->setDisabled(false);
		this->lineStyleCombo->setDisabled(false);
	}
	else {
		this->colorCombo->setDisabled(true);
		this->lineStyleCombo->setDisabled(true);
	}

}

void ShPropertyColumn::Update(ShSelectedEntityCountChangedEvent *event) {
	
	ShGraphicView *view = event->GetView();
	
	if (event->GetCount() == 0) {

		this->SetColorComboInfo(view->GetData()->GetBlockData()->GetColor(),
			view->GetData()->GetLayerData()->GetColor(),
			view->GetData()->GetPropertyData()->GetColor());

		this->SetLineStyleComboInfo(view->GetData()->GetBlockData()->GetLineStyle(),
			view->GetData()->GetLayerData()->GetLineStyle(),
			view->GetData()->GetPropertyData()->GetLineStyle());

	}
	else {

		if (event->IsAllSameColor() == true) {

			this->SetColorComboInfo(view->GetData()->GetBlockData()->GetColor(),
				event->GetLayerData().GetColor(), event->GetData().GetColor());

		}
		else {
			this->colorCombo->Synchronize(-1);
		}

		if (event->IsAllSameLineStyle() == true) {

			this->SetLineStyleComboInfo(view->GetData()->GetBlockData()->GetLineStyle(),
				event->GetLayerData().GetLineStyle(), event->GetData().GetLineStyle());

		}
		else {
			this->lineStyleCombo->Synchronize(-1);
		}

	}


}

void ShPropertyColumn::Notify(ShNotifyEvent *event) {

	ShChangeManager *manager = ShChangeManager::GetInstance();

	manager->Notify(this, event);

}

void ShPropertyColumn::ColorSelChanged(const ShColor& color) {

	ShPropertyColorComboSelChangedEvent event(color);
	this->Notify(&event);
}

int ShPropertyColumn::GetColorComboIndex() {

	return this->colorCombo->GetColorComboIndex();
}


void ShPropertyColumn::SynchronizeLineStyleCombo(int lineStyleComboIndex) {

	this->lineStyleCombo->Synchronize(lineStyleComboIndex);
}

void ShPropertyColumn::SynchronizeLineStyleCombo(const ShLineStyle& lineStyle) {

	if (lineStyle.GetType() == ShLineStyle::Type::ByBlock)
		this->lineStyleCombo->Synchronize(0);
	else if (lineStyle.GetType() == ShLineStyle::Type::ByLayer)
		this->lineStyleCombo->Synchronize(1);
	else {
		ShLineStyleComboList *list = ShLineStyleComboList::GetInstance();
		int index = list->Search(lineStyle);
		this->lineStyleCombo->Synchronize(index + 2);

	}
}



int ShPropertyColumn::GetLineStyleComboIndex() {

	return this->lineStyleCombo->GetLineStyleComboIndex();
}

void ShPropertyColumn::LineStyleSelChanged(const ShLineStyle& lineStyle) {

	ShPropertyLineStyleComboSelChangedEvent event(lineStyle);
	this->Notify(&event);
}


////////////////////////////////////////////////////////////////////////////////////////////

#include "Interface\Items\ShLayerComboBox.h"
ShLayerColumn::ShLayerColumn(QWidget *parent, const QString &title, int width)
	:ShColumnInRibbonTab(parent, title, width) {

	ShChangeManager *manager = ShChangeManager::GetInstance();
	manager->Register(this);

	this->layerCombo = new ShLayerComboBox(this);

	connect(this->layerCombo, SIGNAL(CurrentIndexChanged(int)), this, SLOT(CurrentIndexChanged(int)));
	connect(this->layerCombo, SIGNAL(LayerTurnChanged(ShLayer*, bool)), this, SLOT(LayerTurnChanged(ShLayer*, bool)));
	connect(this->layerCombo, SIGNAL(LayerColorChanged(ShLayer*, const ShColor&)), this, SLOT(LayerColorChanged(ShLayer*, const ShColor&)));

}

ShLayerColumn::~ShLayerColumn() {

}

void ShLayerColumn::resizeEvent(QResizeEvent *event) {
	
	ShColumnInRibbonTab::resizeEvent(event);

	int height = this->layoutWidget->height();

	this->layerCombo->setGeometry(height / 3 + 2, height / 3, 150, height / 3 - 8);
}

void ShLayerColumn::Update(ShActivatedWidgetChangedEvent *event) {

	this->layerCombo->SetLayerTable(event->GetNewWidget()->entityTable.GetLayerTable());
	this->layerCombo->Synchronize();

	//////////////////////////////////////////////////////////////////////

	if (event->GetNewWidget()->GetCurrentActionType() == ActionType::ActionDefault)
		this->layerCombo->setDisabled(false);
	else
		this->layerCombo->setDisabled(true);

}

void ShLayerColumn::Update(ShCurrentActionChangedEvent *event) {

	if (event->GetActionType() == ActionType::ActionDefault)
		this->layerCombo->setDisabled(false);
	else
		this->layerCombo->setDisabled(true);

}

void ShLayerColumn::Update(ShSelectedEntityCountChangedEvent *event) {

	this->layerCombo->SetLayerTable(event->GetView()->entityTable.GetLayerTable());

	if (event->GetCount() == 0)
		this->layerCombo->Synchronize();
	else {

		if (event->IsAllSameLayer() == true) {
			int index = event->GetView()->entityTable.GetLayerTable()->GetIndex(event->GetLayer());
			this->layerCombo->Synchronize(index);

		}
		else {
			this->layerCombo->Synchronize(-1);
		}

	}
}

void ShLayerColumn::Notify(ShNotifyEvent *event) {

	ShChangeManager *manager = ShChangeManager::GetInstance();

	manager->Notify(this, event);
}

void ShLayerColumn::CurrentIndexChanged(int index) {

	ShLayerComboSelChangedEvent event(index);
	this->Notify(&event);
}


void ShLayerColumn::LayerTurnChanged(ShLayer *layer,bool previous) {

	ShLayerData data = layer->GetData();
	data.SetTurn(previous);


	ShLayerDataChangedEvent event(layer, data, ShLayerDataChangedEvent::ChangedType::TurnOnOff);
	this->Notify(&event);
}

void ShLayerColumn::LayerColorChanged(ShLayer *layer, const ShColor& previous) {

	ShLayerData data = layer->GetData();
	ShPropertyData propertyData = data.GetPropertyData();
	propertyData.SetColor(previous);
	data.SetPropertyData(propertyData);

	ShLayerDataChangedEvent event(layer, data, ShLayerDataChangedEvent::ChangedType::Color);
	this->Notify(&event);
}

void ShLayerColumn::SynchronizeLayerCombo() {

	this->layerCombo->Synchronize();
}

void ShLayerColumn::SynchronizeLayerCombo(int index) {

	this->layerCombo->Synchronize(index);
}

int ShLayerColumn::GetCurrentComboIndex() {

	return this->layerCombo->GetCurrentComboIndex();
}