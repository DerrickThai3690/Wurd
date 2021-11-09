#ifndef STUDENTUNDO_H_
#define STUDENTUNDO_H_

#include "Undo.h"
#include <stack>
class StudentUndo : public Undo {
public:

	void submit(Action action, int row, int col, char ch = 0);
	Action get(int& row, int& col, int& count, std::string& text);
	void clear();

private:
	struct info {
	public:
		info(Action a, int r, int c, char ch) {
			m_action = a;
			m_row = r;
			m_col = c;
			m_ch = ch;
		}

		Action m_action;
		int m_row;
		int m_col;
		char m_ch;
	};
	std::stack<info> undoStack;
};

#endif // STUDENTUNDO_H_
