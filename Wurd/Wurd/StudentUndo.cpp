#include "StudentUndo.h"

Undo* createUndo()
{
	return new StudentUndo;
}

void StudentUndo::submit(const Action action, int row, int col, char ch) {
	//push all inputted information onto a stack
	info curr(action, row, col, ch);
	undoStack.push(curr);
	
}

StudentUndo::Action StudentUndo::get(int& row, int& col, int& count, std::string& text) {
	//if stack is not empty then read the information of top of stack
	if (!undoStack.empty()) {
		Action returnAction;
		//if action is insert then delete will occur
		if (undoStack.top().m_action == Undo::Action::INSERT) {
			returnAction = Undo::Action::DELETE;
			row = undoStack.top().m_row;
			col = undoStack.top().m_col;
			count = 1;
			text = "";
		}
		//if action is delete, then insertion will occur
		else if(undoStack.top().m_action == Undo::Action::DELETE){
			returnAction = Undo::Action::INSERT;
			row = undoStack.top().m_row;
			col = undoStack.top().m_col;
			count = text.length();
			text = undoStack.top().m_ch;
		}
		//if action is split, then join will occur
		else if (undoStack.top().m_action == Undo::Action::SPLIT) {
			returnAction = Undo::Action::JOIN;
			row = undoStack.top().m_row;
			col = undoStack.top().m_col;
			count = 1;
			text = "";
		}
		//if action is join, then split will occur
		else if (undoStack.top().m_action == Undo::Action::JOIN) {
			returnAction = Undo::Action::SPLIT;
			row = undoStack.top().m_row;
			col = undoStack.top().m_col;
			count = 1;
			text = "";
		}
		undoStack.pop();
		return returnAction;
	}
	
	return Action::ERROR;
}

void StudentUndo::clear() {
	while (!undoStack.empty()) {
		undoStack.pop();
	}
}
