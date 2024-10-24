#ifndef MATERIAL_H
#define MATERIAL_H

class Material {
public:
    virtual void send_uniform();
};


class PhongMaterial: public Material {

};

#endif
