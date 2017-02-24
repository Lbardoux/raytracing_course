/**
 * @file Triangle.hpp
 * @brief Embarque la définition / implémentation du triangle hors du main.
 */
#ifndef TRIANGLE_HPP_INCLUDED
#define TRIANGLE_HPP_INCLUDED

#include "../core/gkit_core.hpp"
#include "../core/math_core.hpp"


//! representation d'un rayon.
struct Ray
{
    Point o;	//!< origine.
    Vector d;	//!< direction.
    float tmax;	//!< abscisse max pour les intersections valides.
    
    Ray( const Point origine, const Point extremite ) : o(origine), d(Vector(origine, extremite)), tmax(1) {}
    Ray( const Point origine, const Vector direction ) : o(origine), d(direction), tmax(FLT_MAX) {}
    
    //!	renvoie le point a l'abscisse t sur le rayon
    Point operator( ) ( const float t ) const { return o + t * d; }
};

struct Triangle : public TriangleData
{
    Triangle( ) : TriangleData() {}
    Triangle( const TriangleData& data ) : TriangleData(data) {}
    
    /* calcule l'intersection ray/triangle
        cf "fast, minimum storage ray-triangle intersection" 
        http://www.graphics.cornell.edu/pubs/1997/MT97.pdf

        renvoie faux s'il n'y a pas d'intersection valide, une intersection peut exister mais peut ne pas se trouver dans l'intervalle [0 htmax] du rayon. \n
        renvoie vrai + les coordonnees barycentriques (ru, rv) du point d'intersection + sa position le long du rayon (rt). \n
        convention barycentrique : t(u, v)= (1 - u - v) * a + u * b + v * c \n
    */
    bool intersect( const Ray &ray, const float htmax, float &rt, float &ru, float&rv ) const
    {
        /* begin calculating determinant - also used to calculate U parameter */
        Vector ac= Vector(Point(a), Point(c));
        Vector pvec= cross(ray.d, ac);

        /* if determinant is near zero, ray lies in plane of triangle */
        Vector ab= Vector(Point(a), Point(b));
        float det= dot(ab, pvec);
        if(det > -EPSILON && det < EPSILON)
            return false;

        float inv_det= 1.0f / det;

        /* calculate distance from vert0 to ray origin */
        Vector tvec(Point(a), ray.o);

        /* calculate U parameter and test bounds */
        float u= dot(tvec, pvec) * inv_det;
        if(u < 0.0f || u > 1.0f)
            return false;

        /* prepare to test V parameter */
        Vector qvec= cross(tvec, ab);

        /* calculate V parameter and test bounds */
        float v= dot(ray.d, qvec) * inv_det;
        if(v < 0.0f || u + v > 1.0f)
            return false;

        /* calculate t, ray intersects triangle */
        rt= dot(ac, qvec) * inv_det;
        ru= u;
        rv= v;

        // ne renvoie vrai que si l'intersection est valide (comprise entre tmin et tmax du rayon)
        return (rt <= htmax && rt > EPSILON);
    }

    //! renvoie l'aire du triangle
    float area( ) const
    {
        return length(cross(Point(b) - Point(a), Point(c) - Point(a))) / 2.f;
    }
    
    //! renvoie un point a l'interieur du triangle connaissant ses coordonnees barycentriques.
    //! convention p(u, v)= (1 - u - v) * a + u * b + v * c
    Point point( const float u, const float v ) const
    {
        float w= 1.f - u - v;
        return Point(Vector(a) * w + Vector(b) * u + Vector(c) * v);
    }

    //! renvoie une normale a l'interieur du triangle connaissant ses coordonnees barycentriques.
    //! convention p(u, v)= (1 - u - v) * a + u * b + v * c
    Vector normal( const float u, const float v ) const
    {
        float w= 1.f - u - v;
        return Vector(na) * w + Vector(nb) * u + Vector(nc) * v;
    }
};


//! representation d'une source de lumiere.
struct Source : public Triangle
{
    Color emission;     //! flux emis.
    
    Source( ) : Triangle(), emission() {}
    Source( const TriangleData& data, const Color& color ) : Triangle(data), emission(color) {}
    
    /**
	 * @brief      
	 * 
	 * Obtient un point aléatoire appartenant au triangle   
	 *
	 * @return     Le point aléatoire.
	 */
	Point getRandomPoint(void) const
	{
		int rndU = rand();
		int rndV = rndU % (RAND_MAX - rndU) + rndU;

		float u = static_cast <float> (rndU) / static_cast <float> (RAND_MAX);
		float v = static_cast <float> (rndV) / static_cast <float> (RAND_MAX);

		return this->point(u, v);
	}
};




#endif
