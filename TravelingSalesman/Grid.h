#pragma once

#include <array>

#include "Utils.h"

class Node;

class Grid {
public:
	static const size_t s_kGridSize = 16;

	Grid();
	~Grid();
	
	// Sets the node at the specified grid position.
	void setGridNode(size_t row, size_t col, Node* node);

	// Gets the node at the specified grid position.
	Node* getGridNode(size_t row, size_t col) const;

	// Weird proxy for safe 2D index operations.
	// Stores a pointer to a grid row that can then be indexed by column.
	class IndexProxy {
	public:
		IndexProxy(const std::array<Node*, s_kGridSize>* _array);

		// Accesses a node in the 2D grid through an index proxy.
		// Returns a null pointer on invalid index.
		Node* operator[](size_t col) const;
	private:
		const std::array<Node*, s_kGridSize>* m_array;
	};

	// Accesses a node in the 2D grid through an index proxy.
	// Returns an index proxy pointing to the specified row in the grid.
	// If an invalid row is supplied, a dummy index proxy is returned that 
	// always returns a null pointer when indexed into.
	IndexProxy operator[](size_t row) const;

	// Returns whether two nodes are connectable.
	bool areConnectable(Node* from, Node* to) const;

private:
	NDArray<Node*, s_kGridSize, s_kGridSize> m_internalGrid;
};

