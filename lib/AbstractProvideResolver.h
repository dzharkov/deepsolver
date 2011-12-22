
#ifndef DEPSOLVER_ABSTRACT_PROVIDE_RESOLVER_H
#define DEPSOLVER_ABSTRACT_PROVIDE_RESOLVER_H

class AbstractProvideResolver
{
public:
  virtual ~AbstractProvideResolver() {}

public:
  virtual void resolveProvide(PackageId packageId, PackageIdVector& providers) const = 0;
}; //class AbstractProvideResolver;

#endif //DEPSOLVER_ABSTRACT_PROVIDE_RESOLVER_H;
