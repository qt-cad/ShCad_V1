

#ifndef _SHCADFACADE_H
#define _SHCADFACADE_H

/* All functions takes actions and follow-up on it such as updating list, saving undo,redo memento , ect */

class ShGraphicView;
class ShCadFacade{

private:
	ShCadFacade();
	~ShCadFacade();

public:
	//Ctrl + Z , Undo command or tool Button press.
	static void Undo(ShGraphicView *graphicView);

	//Ctrl + Y , Redo command or tool Button press.
	static void Redo(ShGraphicView *graphicView);

	//Ctrl + A , SelectAll or tool Button press.
	static void SelectAll(ShGraphicView *graphicView);

	//Delete or tool Button press.
	static void Delete(ShGraphicView *graphicView);

	




};

#endif //_SHCADFACADE_H