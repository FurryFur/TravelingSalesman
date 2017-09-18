#include <math.h>

#include <nanogui\object.h>

#include "Grid.h"
#include "Node.h"

Grid::Grid()
	: m_internalGrid{}
{
}


Grid::~Grid()
{
}

void Grid::setGridNode(size_t row, size_t col, nanogui::ref<Node> node)
{
	m_internalGrid[row][col] = std::move(node);
}

nanogui::ref<Node> Grid::getGridNode(size_t row, size_t col) const
{
	if (row < s_kGridSize && col < s_kGridSize)
		return m_internalGrid[row][col];

	return nanogui::ref<Node>{};
}

Grid::IndexProxy Grid::operator[](size_t row) const
{
	if (row < m_internalGrid.size())
		return Grid::IndexProxy(&m_internalGrid[row]);

	return Grid::IndexProxy(nullptr);
}

bool Grid::areConnectable(Node* from, Node* to) const
{
	// Null and direct obstruction check
	if (!from || !to || from->isObstructed() || to->isObstructed())
		return false;

	// Get row and column info
	size_t fromR = from->getRow();
	size_t fromC = from->getCol();
	size_t toR = to->getRow();
	size_t toC = to->getCol();

	// Calculate relative offset
	int relR = static_cast<int>(toR - fromR);
	int relC = static_cast<int>(toC - fromC);

	// If more than one cell width away, then the 'to' node is unreachable
	if (std::abs(relR) > 1 || std::abs(relC) > 1)
		return false;

	// If 'to' node is non-diagonal then it is reachable
	if (relR == 0 || relC == 0)
		return true;

	// Check cross diagonal
	size_t crossDiagR = fromR + relR;
	size_t crossDiagC = fromC + relC;
	const nanogui::ref<Node>& crossDiag1 = m_internalGrid[crossDiagR][fromC];
	const nanogui::ref<Node>& crossDiag2 = m_internalGrid[fromR][crossDiagC];
	if (crossDiag1->isObstructed() || crossDiag2->isObstructed())
		return false;

	// If no obstructions were found then the 'to' node must be reachable
	return true;
}

Grid::IndexProxy::IndexProxy(const std::array<nanogui::ref<Node>, s_kGridSize>* _array) : 
	m_array(_array) 
{ 

}

nanogui::ref<Node> Grid::IndexProxy::operator[](size_t col) const
{
	if (m_array && col < m_array->size())
		return (*m_array)[col];

	return nanogui::ref<Node>{};
}
