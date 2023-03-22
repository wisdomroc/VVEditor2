#ifndef SINGLETONPROJECT_H
#define SINGLETONPROJECT_H

#include "WRP_Project.h"

class SingletonProject
{
public:
    SingletonProject();
    static WRP_Project project_;
};

#endif // SINGLETONPROJECT_H
