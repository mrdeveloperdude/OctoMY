#ifndef MESHDATA_HPP
#define MESHDATA_HPP

#include <rhi/qrhi.h>

// Define a simple MeshData structure for overlay geometry.
struct MeshData {
	// Pointers to vertex and index buffers.
	// These should be non-const for binding.
	QRhiBuffer *vertexBuffer;
	QRhiBuffer *indexBuffer;
	// Number of indices to draw.
	int indexCount;
};


#endif // MESHDATA_HPP
