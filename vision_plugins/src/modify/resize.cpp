/// HEADER
#include "resize.h"

/// PROJECT
#include <csapex/utility/register_apex_plugin.h>
#include <csapex/model/connector_in.h>
#include <csapex/model/connector_out.h>
#include <utils_param/parameter_factory.h>
#include <csapex_vision/cv_mat_message.h>

/// SYSTEM
#include <boost/assign/list_of.hpp>

CSAPEX_REGISTER_CLASS(vision_plugins::Resize, csapex::Node)

using namespace csapex::connection_types;
using namespace csapex;
using namespace vision_plugins;

Resize::Resize()
{
    addTag(Tag::get("Vision"));
    addTag(Tag::get("vision_plugins"));

    addParameter(param::ParameterFactory::declareRange("size width", 10, 10000, 640, 1),
                 boost::bind(&Resize::update, this));
    addParameter(param::ParameterFactory::declareRange("size height", 10, 10000, 480, 1),
                 boost::bind(&Resize::update, this));

    std::map<std::string, int> modes = boost::assign::map_list_of
            ("nearest", (int) CV_INTER_NN)
            ("linear", (int) CV_INTER_LINEAR)
            ("area", (int) CV_INTER_AREA)
            ("cubic", (int) CV_INTER_CUBIC)
            ("lanczos4", (int) CV_INTER_LANCZOS4);
    addParameter(param::ParameterFactory::declareParameterSet<int>("mode", modes), boost::bind(&Resize::update, this));

}

void Resize::process()
{
    CvMatMessage::Ptr in = input_->getMessage<connection_types::CvMatMessage>();
    CvMatMessage::Ptr out(new connection_types::CvMatMessage(in->getEncoding()));

    if(!in->value.empty()) {
        cv::resize(in->value, out->value, size_, 0.0, 0.0, mode_);
    } else {
        throw std::runtime_error("Cannot scale empty images!");
    }

    output_->publish(out);
}

void Resize::setup()
{
    input_  = addInput<CvMatMessage>("original");
    output_ = addOutput<CvMatMessage>("resize");
    update();
}

void Resize::update()
{
    size_.width  = param<int>("size width");
    size_.height = param<int>("size height");
    mode_     = param<int>("mode");
}
