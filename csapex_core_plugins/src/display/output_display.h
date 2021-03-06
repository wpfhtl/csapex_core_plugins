#ifndef OUTPUT_DISPLAY_H
#define OUTPUT_DISPLAY_H

/// PROJECT
#include <csapex/model/node.h>
#include <csapex/view/view_fwd.h>
#include "csapex_core_plugins_node_export.h"

/// SYSTEM
#include <QImage>
#include <QSharedPointer>

namespace csapex
{

class Input;

class CSAPEX_CORE_PLUGINS_NODE_EXPORT OutputDisplay : public Node
{
    friend class OutputDisplayAdapter;

public:
    OutputDisplay();
    virtual ~OutputDisplay();

    void setup(csapex::NodeModifier& node_modifier) override;
    virtual void process() override;

    void setAdapted();

protected:
    Input* input_;

public:
    csapex::slim_signal::Signal<void(QImage)> display_request;
    MessageRendererPtr renderer_;

    bool adapted_;
};

}

#endif // OUTPUT_DISPLAY_H
