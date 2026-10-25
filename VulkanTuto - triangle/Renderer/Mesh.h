#pragma once

#include <vector>
#include <cstdint>
#include "Vertex.h"

const float couleur = 1.0f;
const float nocouleur = 0.0f;

const std::vector<Vertex> vertices = {
	{{-0.5f, -0.5f, 0.5f}, {couleur, nocouleur, nocouleur }, {0.0f, 0.0f}}, //top
	{{0.5f, -0.5f, 0.5f}, {couleur, nocouleur, nocouleur }, {1.0f, 0.0f}},
	{{0.5f, 0.5f, 0.5f}, {couleur, nocouleur, nocouleur }, {1.0f, 1.0f}},
	{{-0.5f, 0.5f, 0.5f}, {couleur, nocouleur, nocouleur }, {0.0f, 1.0f}},

	{{-0.5f, -0.5f, -0.5f}, {couleur, nocouleur, nocouleur }, {0.0f, 0.0f}}, //side1
	{{-0.5f, -0.5f, 0.5f}, {couleur, nocouleur, nocouleur }, {1.0f, 0.0f}},
	{{-0.5f, 0.5f, 0.5f}, {couleur, nocouleur, nocouleur }, {1.0f, 1.0f}},
	{{-0.5f, 0.5f, -0.5f}, {couleur, nocouleur, nocouleur }, {0.0f, 1.0f}},

	{{0.5f, -0.5f, 0.5f}, {couleur, nocouleur, nocouleur }, {0.0f, 0.0f}}, //side2
	{{0.5f, -0.5f, -0.5f}, {couleur, nocouleur, nocouleur }, {1.0f, 0.0f}},
	{{0.5f, 0.5f, -0.5f}, {couleur, nocouleur, nocouleur }, {1.0f, 1.0f}},
	{{0.5f, 0.5f, 0.5f}, {couleur, nocouleur, nocouleur }, {0.0f, 1.0f}},

	{{-0.5f, -0.5f, 0.5f}, {couleur, nocouleur, nocouleur }, {0.0f, 0.0f}}, //side3
	{{-0.5f, -0.5f, -0.5f}, {couleur, nocouleur, nocouleur }, {1.0f, 0.0f}},
	{{0.5f, -0.5f, -0.5f}, {couleur, nocouleur, nocouleur }, {1.0f, 1.0f}},
	{{0.5f, -0.5f, 0.5f}, {couleur, nocouleur, nocouleur }, {0.0f, 1.0f}},

	{{0.5f, 0.5f, 0.5f}, {couleur, nocouleur, nocouleur }, {0.0f, 0.0f}}, //side4
	{{0.5f, 0.5f, -0.5f}, {couleur, nocouleur, nocouleur }, {1.0f, 0.0f}},
	{{-0.5f, 0.5f, -0.5f}, {couleur, nocouleur, nocouleur }, {1.0f, 1.0f}},
	{{-0.5f, 0.5f, 0.5f}, {couleur, nocouleur, nocouleur }, {0.0f, 1.0f}},

	{{-0.5f, 0.5f, -0.5f}, {couleur, nocouleur, nocouleur }, {0.0f, 0.0f}}, //bottom
	{{0.5f, 0.5f, -0.5f}, {couleur, nocouleur, nocouleur }, {1.0f, 0.0f}},
	{{0.5f, -0.5f, -0.5f}, {couleur, nocouleur, nocouleur }, {1.0f, 1.0f}},
	{{-0.5f, -0.5f, -0.5f}, {couleur, nocouleur, nocouleur }, {0.0f, 1.0f}}
};

const std::vector<uint16_t> indices = {
	0, 1, 2, 2, 3, 0,
	4, 5, 6, 6, 7, 4,
	8, 9, 10, 10, 11, 8,
	12, 13, 14, 14, 15, 12,
	16, 17, 18, 18, 19, 16,
	20, 21, 22, 22, 23, 20
};

//    float3(0.157, 1.0, 0.522),
//    float3(0.408, 0.03, 0.03),
//    float3(0.408, 0.03, 0.03)
