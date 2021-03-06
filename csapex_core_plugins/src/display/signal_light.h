#ifndef SIGNAL_LIGHT_H_
#define SIGNAL_LIGHT_H_

/// PROJECT
#include <csapex/model/node.h>

namespace csapex {

class CSAPEX_EXPORT_PLUGIN SignalLight : public Node
{
public:
    SignalLight();

    virtual void setupParameters(Parameterizable &parameters) override;

    virtual void process() override;
    virtual void setup(csapex::NodeModifier& node_modifier) override;

public:
    csapex::slim_signal::Signal<void(int)> display_request;

protected:
    void display(int state);

private:
    Slot* slot_red_;
    Slot* slot_yellow_;
    Slot* slot_green_;
};

}

#endif // SIGNAL_LIGHT_H_
