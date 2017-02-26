#include "BinaryTree.hpp"
#include <algorithm>

namespace 
{
	typedef BinaryTree::Node Node;
};

void BinaryTree::init(const Scene& scene)
{
	this->triangles = &scene.triangles;
	
	// ATTENTION, à voir si c'est size ou (size - 1) !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	this->root = this->build_node(0, scene.triangles.size());
}

void BinaryTree::findBox(const triangle_ind_t begin, const triangle_ind_t end, BoundingBox& bbox)
{
	triangle_ind_t indTab = 0;
	axes_t axes;
	float * triangle;
    
    triangle = static_cast<float*>(&(*this->triangles)[begin].a.x);

    for(axes = 0; axes < 3; ++axes)
    {
		bbox.pmin[axes] = triangle[axes];
		bbox.pmax[axes] = triangle[axes];
	}
	
	for(indTab = 1; indTab < end; ++indTab)
	{
		triangle = static_cast<float*>(&(*this->triangles)[indTab].a.x);
		for(axes = 0; axes < 3; ++axes)
			if(bbox.pmin[axes] > triangle[axes])
				bbox.pmin[axes] = triangle[axes];
			else if(bbox.pmax[axes] < triangle[axes])
				bbox.pmax[axes] = triangle[axes];
	}
}

axes_t BinaryTree::largerAxe(const BoundingBox& bbox)
{
	float max, calc;
	axes_t axe = 0;

	// max = bvh->box.max[0] - bvh->box.min[0];
	max = bbox.pmax[0] - bbox.pmin[0];
	
	for(axes_t i = 1; i < 3; ++i)
	{
		calc = bbox.pmax[i] - bbox.pmin[i];
		if(max < calc)
		{
			max = calc;
			axe = i;
		}
	}
	
	return axe;
}

node_ind_t BinaryTree::build_node(const triangle_ind_t begin, const triangle_ind_t end)
{
	// Ajout d'un Node vide
	this->nodes.emplace_back();

	Node& noeud    = this->nodes.back();
	node_ind_t ind = this->nodes.size() - 1;

    if(end - begin <= 1)
    {
        // construire une feuille qui reference le triangle d'indice begin
    	noeud.triangle = begin;

        // renvoyer l'indice de la feuille
        return ind;
    }

    // construire la boite englobante des centres des triangles d'indices [begin .. end[
    this->findBox(begin, end, noeud.bbox);

    // trouver l'axe le plus etire de la boite englobante
    axes_t axe = this->largerAxe(noeud.bbox);

    // couper en 2 au milieu de boite englobante sur l'axe le plus etire
    float coupe = (noeud.bbox.pmax[axe] + noeud.bbox.pmin[axe]) / 2.0f;

    // partitionner les triangles par rapport a la "coupe"
    Triangle *pmid = std::partition(triangles->data() + begin, triangles->data() + end, predicat(axe, coupe));
    triangle_ind_t mid = std::distance(triangles->data(), pmid);

    // construire le fils gauche
    noeud.left = build_node(begin, mid);

    // construire le fils droit
    noeud.right = build_node(mid, end);

    // renvoyer l'indice du noeud
    return ind;
}
