
#include "ShArc.h"
#include "Visitor Pattern\ShVisitor.h"


ShArc::ShArc() {


}

ShArc::ShArc(const ShArcData &data)
	:data(data) {


}

ShArc::ShArc(const ShPropertyData& propertyData, const ShArcData &data, ShLayer *layer)
	: ShLeaf(propertyData, layer), data(data) {

}

ShArc::ShArc(const ShArc& other)
	: ShLeaf(other), data(other.data) {


}

ShArc& ShArc::operator=(const ShArc& other) {

	ShLeaf::operator=(other);

	this->data = other.data;

	return *this;
}

ShArc::~ShArc() {


}

ShArc* ShArc::Clone() {

	return new ShArc(*this);
}

void ShArc::Accept(ShVisitor *shVisitor) {

	shVisitor->Visit(this);

}

void ShArc::Move(double cx, double cy) {

	this->data.center.x += cx;
	this->data.center.y += cy;
}


#include "ShMath.h"
ShPoint3d ShArc::GetStart() {

	ShPoint3d start;
	ShPoint3d center = this->data.center;
	double radius = this->data.radius;
	double startAngle = this->data.startAngle;

	Math::Rotate(startAngle, center.x, center.y, center.x + radius, center.y, start.x, start.y);

	return start;
}

ShPoint3d ShArc::GetEnd() {

	ShPoint3d end;
	ShPoint3d center = this->data.center;
	double radius = this->data.radius;
	double endAngle = this->data.endAngle;

	Math::Rotate(endAngle, center.x, center.y, center.x + radius, center.y, end.x, end.y);

	return end;
}

ShPoint3d ShArc::GetMid() {

	ShPoint3d mid;
	ShPoint3d center = this->data.center;
	double radius = this->data.radius;
	double difference = Math::GetAngleDifference(this->data.startAngle, this->data.endAngle);

	Math::Rotate(this->data.startAngle + (difference / 2.0), center.x, center.y, center.x + radius, center.y, mid.x, mid.y);

	return mid;
}
