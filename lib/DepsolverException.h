
#ifndef DEPSOLVER_DEPSOLVER_EXCEPTION_H
#define DEPSOLVER_DEPSOLVER_EXCEPTION_H

class DepsolverException
{
public:
  DepsolverException() {}
  virtual ~DepsolverException() {}

public:
  virtual std::string getMessage() const = 0;
}; //class DepsolverException;

#endif //DEPSOLVER_DEPSOLVER_EXCEPTION_H;
