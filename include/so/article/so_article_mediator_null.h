#pragma once

// soArticleMediatorNull: the article mediator of stage objects that generate no articles of their own.

#include <so/article/so_article_mediator.h>

class soArticleMediatorNull : public soArticleMediator {
public:
    soArticleMediatorNull() : soArticleMediator(nullptr) { }
    virtual ~soArticleMediatorNull() { }
    virtual soArticle* generate(s32, soModuleAccesser*) { return (soArticle*)g_ftRobotNullArticleStorage; }
    virtual void deactivate() { }
    virtual bool isGeneratable(soModuleAccesser*, s32) { return false; }
    virtual s32 getActiveNum(soModuleAccesser*, s32) { return 0; }
    virtual s32 getGenerateMaxNum(s32) { return 0; }
    virtual s32 getMediateNum() { return 0; }
    virtual void setAutoRecycle(bool) { }
    virtual bool shoot(soModuleAccesser*, soArticle*) { return true; }
};
