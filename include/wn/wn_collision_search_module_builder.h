#pragma once
// Search collision module of an article built in sora_melee: one fighter-category part, one group. The task category
// reaches the builder as the byte the search module takes (the caller narrows it).
// HYPOTHESIS: the source spelling is soCollisionSearchModuleBuildConfig<0, 1, 1, soCollisionSearchModuleImpl> (map name);
// that template name is taken by the fighters' one-argument placeholder.
#include <ft/builder/ft_module_builders.h>

template <typename T>
class wnCollisionSearchModuleBuildConfigArticle {
public:
    typedef T ModuleType;
};

template <typename T>
class soCollisionSearchModuleBuilder<wnCollisionSearchModuleBuildConfigArticle<T> > {
    soArrayVector<soCollisionSearchPart, 1> m_parts;
    soArrayVector<soCollisionGroup, 1> m_groups;
    T m_searchModule;
public:
    soCollisionSearchModuleBuilder(soModuleAccesser* acc, int taskId, u8 category) :
        m_parts(1, soCollisionSearchPart(soCollision::Category_Fighter), 0),
        m_groups(1, 0), m_searchModule(acc, taskId, category, &m_parts, true) { }
    T* getModule() { return &m_searchModule; }
};
