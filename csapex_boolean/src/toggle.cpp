/// HEADER
#include "toggle.h"

/// PROJECT
#include <csapex/msg/io.h>
#include <csapex/param/parameter_factory.h>
#include <csapex/model/node_modifier.h>
#include <csapex/utility/register_apex_plugin.h>
#include <csapex/msg/generic_value_message.hpp>

CSAPEX_REGISTER_CLASS(csapex::boolean::Toggle, csapex::Node)

using namespace csapex;
using namespace csapex::boolean;

Toggle::Toggle()
{
}

void Toggle::setupParameters(Parameterizable &parameters)
{
    parameters.addParameter(csapex::param::ParameterFactory::declareBool("true", true),
                 std::bind(&Toggle::setSignal, this));
}

void Toggle::setup(NodeModifier& node_modifier)
{
    out = node_modifier.addOutput<bool>("Signal");
}

void Toggle::process()
{

}

void Toggle::tick()
{
    msg::publish(out, signal_);
}

void Toggle::setSignal()
{
    signal_ = readParameter<bool>("true");
}
