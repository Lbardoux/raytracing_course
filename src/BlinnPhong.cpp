

#include "BlinnPhong.hpp"

Color BlinnPhong(const BlinnPhongWrapper& wrap, float coef) noexcept
{
    const Material& material  = wrap.mesh->triangle_material(wrap.hit->object_id);
    Vector          PO        = normalize(Vector(wrap.hit->p, *wrap.observer));
    Vector          PS        = normalize(Vector(wrap.hit->p, *wrap.src));
    Vector          H         = normalize((PO + PS)/2.0f);
    float           cosTheta  = std::max(0.0f, dot(wrap.hit->n, PS));
    float           cosThetaH = std::max(0.0f, dot(wrap.hit->n, H));
    float           f         = ((material.ns + 1.0f)/(2.0f*M_PI))*std::pow(cosThetaH, material.ns);
    
    return (1.0f-coef)*material.specular*cosTheta*f + (coef)*cosTheta*material.diffuse;
}


