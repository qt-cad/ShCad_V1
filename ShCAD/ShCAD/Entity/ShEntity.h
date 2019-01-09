


#ifndef _SHENTITY_H
#define _SHENTITY_H

/*Base class for all objects having a graphical representation.*/

class ShMemento;
class ShVisitor;
class ShEntity {

public:
	ShEntity();
	virtual ~ShEntity() = 0;
	ShEntity(const ShEntity& other);
	ShEntity& operator=(const ShEntity& other);

	virtual ShEntity* Clone() = 0;
	virtual void Accept(ShVisitor *shVisitor) = 0;

	//virtual ShMemento* CreateMemento() {}
	//virtual void SetMemento(const ShMemento* memento) {}

};


#endif //_SHENTITY_H
