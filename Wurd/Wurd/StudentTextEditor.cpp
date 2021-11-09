#include "StudentTextEditor.h"
#include "Undo.h"
#include <string>
#include <vector>
#include <iostream>
#include <fstream>

using namespace std;

TextEditor* createTextEditor(Undo* un)
{
	return new StudentTextEditor(un);
}

StudentTextEditor::StudentTextEditor(Undo* undo)
 : TextEditor(undo) {
	// cursor starts at top left of screen
	m_lines.push_back("");
	cursor = m_lines.begin();
	m_rows = 0;
	m_cols = 0;
}

StudentTextEditor::~StudentTextEditor()
{
	reset();
}

bool StudentTextEditor::load(std::string file) {
	ifstream infile(file);
	//if invalid file then return an error
	if (!infile) {
		cerr << "Error: File cannot be found" << endl;
		return false;
	}
	
	reset();
	string s;
	cursor = m_lines.begin();

	while (getline(infile, s)) {
		//gets rid of return carraige if there
		if (s.size() > 0 && s.at(s.size() - 1) == '\r') {
			s.erase(s.size() - 1);
		}
		//push a row into m_lines container
		m_lines.push_back(s);
	}
	
	//makes cursor start at top left of screen
	m_cols = 0;
	m_rows = 0;
	cursor = m_lines.begin();
	return true;  // TODO
}

bool StudentTextEditor::save(std::string file) {
	ofstream outfile(file);
	if (!outfile) {
		cerr << "Error: Cannot create text file" << endl;
		return false;
	}
	file.clear();
	list<string>::iterator it = m_lines.begin();
	while (it != m_lines.end()) {
		outfile << (*it) + '\n';
		it++;
	}

	return true;  
}

void StudentTextEditor::reset() {
	//empty all lines and set cursor to top left
	m_lines.clear();
	m_rows = 0;
	m_cols = 0;
}

void StudentTextEditor::move(Dir dir) {

	list<string>::iterator endLine = m_lines.end();
	endLine--;

	switch (dir) {
		//move cursor up one row
	case UP:
		if (m_rows > 0) {
			m_rows--;
			cursor--;
			//if current line is longer than next line
			//move cursor to end of next line
			if (m_cols > (*cursor).size()) {
				m_cols = (*cursor).size();
			}
		}
		break;
	//move cursor down one row
	case DOWN:
		if (m_rows < m_lines.size()-1) {
			m_rows++;
			cursor++;
			//if current line is longer than next line
			//move cursor to end of next line
			if (m_cols > (*cursor).size()) {
				m_cols = (*cursor).size();
			}
		}
		break;
	//move cursor one position left
	//if cursor at beginning of line then move to end of previous line
	case LEFT:
			if (m_cols == 0 && cursor != m_lines.begin()) {
				cursor--;
				m_rows--;
				m_cols = (*cursor).size();

			}
			else if(m_cols > 0 ) {
				m_cols--;
			}
		
		break;
	//move cursor one position right
	//if cursor at end of line then move to beginning of next line
	case RIGHT:
		if ( m_rows < m_lines.size()) {
			if (m_cols == (*cursor).size() && cursor != endLine) {
				cursor++;
				m_rows++;
				m_cols = 0;
			}
			else if(m_cols < (*cursor).size() ){
				m_cols++;
			}
		}

		break;
	//move cursor to beginning of current line
	case HOME:
		m_cols = 0;
		break;
	//move cursor to end of current line
	case END:
		m_cols = (*cursor).size();
		break;
	}
	
}

void StudentTextEditor::del() {

	list<string>::iterator tempCur = cursor;
	list<string>::iterator endLine = m_lines.end();
	endLine--;
		//if cursor is one position after end of line
		//merge next line with current line
		//then delete next line
		if (m_cols == (*cursor).size() && cursor != endLine) {
				tempCur++;
				(*cursor) += (*tempCur);
				tempCur = m_lines.erase(tempCur);
				getUndo()->submit(Undo::Action::JOIN, m_rows, m_cols);
		}
		//delete a character at cursor position
		else if(m_cols != (*cursor).size()){
			char temp = (*cursor).at(m_cols);
			(*cursor).erase((*cursor).begin() + m_cols);
			getUndo()->submit(Undo::Action::DELETE, m_rows, m_cols, temp);
		}
	
}

void StudentTextEditor::backspace() {
	list<string>::iterator tempCur = cursor;

		//if cursor is at beginning of line
		//merge previous line with current line
		//then delete current line
		if (m_cols == 0 && cursor != m_lines.begin()) {
			cursor--;
			m_cols = (*cursor).size();
			(*cursor) += (*tempCur);
			tempCur = m_lines.erase(tempCur);
			m_rows--;
			getUndo()->submit(Undo::Action::JOIN, m_rows, m_cols);
		}
		//delete a character at cursor position
		else if(m_cols != 0){
			char temp = (*cursor).at(m_cols - 1);
			(*cursor).erase((*cursor).begin() + m_cols - 1);
			m_cols--;
			getUndo()->submit(Undo::Action::DELETE, m_rows, m_cols, temp);
		}
	
	
}

void StudentTextEditor::insert(char ch) {
	//if the cursor is not beyond the last line then insert a char
	if (cursor != m_lines.end()) {
		if (ch == '\t') {
			//insert 4 spaces at cursor position
			for (int i = 0; i < 4; i++) {
				(*cursor).insert((*cursor).begin() + m_cols, ' ');
				m_cols++;
			}
			getUndo()->submit(Undo::Action::INSERT, m_rows, m_cols, ' ');
		}
		else {
			//insert char at cursor position
			(*cursor).insert((*cursor).begin() + m_cols, ch);
			m_cols++;
			getUndo()->submit(Undo::Action::INSERT, m_rows, m_cols, ch);
		}

	}
}

void StudentTextEditor::enter() {
	string front = (*cursor).substr(0, m_cols);

	getUndo()->submit(Undo::Action::SPLIT, m_rows, m_cols);

	(*cursor).erase((*cursor).begin(), (*cursor).begin() + m_cols);
	m_lines.insert(cursor, front);
	m_rows++;
	m_cols = 0;
	
}
	


void StudentTextEditor::getPos(int& row, int& col) const {
	//position of where things are currently being edited
	row = m_rows; 
	col = m_cols; 
}

int StudentTextEditor::getLines(int startRow, int numRows, std::vector<std::string>& lines) const {
	
	if (startRow < 0 || numRows < 0 || startRow > m_lines.size() ) {
		return -1;
	}

	lines.clear();
	int numLines = 0;
	list<string>::const_iterator it = m_lines.begin();
	
	
	//iterator starts at startRow
	for (int j = 0; j < startRow; j++) {
		it++;
	}

	//starting at startRow and iterating numRows amount of times
	//push back a line from m_lines to lines
	for (int i = 0; i < numRows; i++) {
		if (startRow == m_lines.size()) {
			break;
		}
		if (it == m_lines.end()) {
			break;
		}
		lines.push_back(*it);
		numLines++;
		it++;

	}
	return numLines;
}

void StudentTextEditor::undo() {
	Undo::Action undoAction;
	int row;
	int col;
	int count;
	string text;
	undoAction = getUndo()->get(row, col, count, text);

	char cText = text[0];
	//if action for undo is a deletion then delete
	if (undoAction == Undo::Action::DELETE) {
		m_cols = col;
		m_rows = row;
		if (m_cols > 0) {
			(*cursor).erase((*cursor).begin() + m_cols - 1);
			m_cols--;
		}
		else {
			(*cursor).erase((*cursor).begin() + m_cols);
		}

	}
	//if action for undo is an insertion then insert a character at the cursor
	else if (undoAction == Undo::Action::INSERT) {
		m_cols = col;
		m_rows = row;
		(*cursor).insert((*cursor).begin() + m_cols, cText);
		m_cols++;
	}
	else if (undoAction == Undo::Action::JOIN) {
		m_cols = col;
		m_rows = row;
		list<string>::iterator tempCur = cursor;

		//move cursor to previous line and then merge cursor with temp
		//then delete the line that temp points to
		if (cursor != m_lines.begin()) {
			cursor--;
			
			(*cursor) += (*tempCur);
			tempCur = m_lines.erase(tempCur);
			}

	}
	else if (undoAction == Undo::Action::SPLIT) {
		m_cols = col;
		m_rows = row;

		string front = (*cursor).substr(0, m_cols);

		(*cursor).erase((*cursor).begin(), (*cursor).begin() + m_cols);

		m_lines.insert(cursor, front);

		m_rows++;
		m_cols = 0;
	}
	else {

	}
}
