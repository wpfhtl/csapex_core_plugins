/// HEADER
#include "flip.h"

/// PROJECT
#include <csapex/utility/register_apex_plugin.h>
#include <csapex/msg/input.h>
#include <csapex/msg/output.h>
#include <utils_param/parameter_factory.h>
#include <csapex_vision/cv_mat_message.h>
#include <csapex/model/node_modifier.h>

/// SYSTEM
#include <boost/assign.hpp>

using namespace csapex;
using namespace csapex::connection_types;
using namespace vision_plugins;

CSAPEX_REGISTER_CLASS(vision_plugins::Flip, csapex::Node)

Flip::Flip()
{
}

void Flip::process()
{
    CvMatMessage::Ptr in = input_->getMessage<connection_types::CvMatMessage>();
    CvMatMessage::Ptr out(new connection_types::CvMatMessage(in->getEncoding(), in->stamp));

    switch(mode_) {
    case -1:
    case 0:
    case 1:
        cv::flip(in->value, out->value, mode_);
        break;

    case 2: // +90
        cv::transpose(in->value, out->value);
        cv::flip(out->value, out->value, 0);
        break;

    case 3: // -90
        cv::transpose(in->value, out->value);
        cv::flip(out->value, out->value, 1);
        break;

    }
    output_->publish(out);
}

void Flip::setup()
{
    input_ = modifier_->addInput<CvMatMessage>("original");
    output_ = modifier_->addOutput<CvMatMessage>("flipped");
}

void Flip::setupParameters()
{
    std::map<std::string, int> types = boost::assign::map_list_of
            ("v", 0)
            ("h", 1)
            ("+90", 2)
            ("-90", 3)
            ("v+h", -1);
    addParameter(param::ParameterFactory::declareParameterSet("type", types, -1),
                 boost::bind(&Flip::update, this));
}

void Flip::update()
{
    mode_ = readParameter<int>("type");
}
