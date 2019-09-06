//
// Created by yue on 18-3-16.
//

#ifndef MAMODEL_H
#define MAMODEL_H

#include <vector>
#include "ARMAMath.h"

class MAMoel{
private:
    std::vector<double> data;
    int p;

public:


	
MAMoel(std::vector<double> data, int p) {
    this->data=data;
    this->p=p;
}

std::vector<std::vector<double> > solveCoeOfMA() {
    std::vector<std::vector<double> > vec;
    ARMAMath ar_math;
    std::vector<double>  maCoe(ar_math.computeMACoe(this->data,this->p));
    vec.push_back(maCoe);
    return vec;
}

};
#endif //MAMODEL_H
//
// Created by yue on 18-3-16.
//

