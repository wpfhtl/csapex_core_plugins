/// HEADER
#include "render_rois.h"

/// PROJECT
#include <csapex_core_plugins/vector_message.h>
#include <csapex_vision/cv_mat_message.h>
#include <csapex_vision/roi_message.h>

/// PROJECT
#include <csapex/model/connector_out.h>
#include <csapex/model/connector_in.h>
#include <utils_param/parameter_factory.h>

/// SYSTEM
#include <csapex/utility/register_apex_plugin.h>
#include <boost/foreach.hpp>

CSAPEX_REGISTER_CLASS(csapex::RenderROIs, csapex::Node)

using namespace csapex;
using namespace connection_types;

RenderROIs::RenderROIs()
{
    addTag(Tag::get("Vision"));

    addParameter(param::ParameterFactory::declare<int>("thickness", 1, 20, 1, 1));
    addParameter(param::ParameterFactory::declare<int>("r", 0, 255, 128, 1));
    addParameter(param::ParameterFactory::declare<int>("g", 0, 255, 128, 1));
    addParameter(param::ParameterFactory::declare<int>("b", 0, 255, 128, 1));
}

void RenderROIs::allConnectorsArrived()
{
    CvMatMessage::Ptr img = input_img_->getMessage<CvMatMessage>();
    VectorMessage<RoiMessage>::Ptr rois = input_rois_->getMessage<VectorMessage<RoiMessage> >();

    CvMatMessage::Ptr out(new CvMatMessage);

    cv::Scalar color(param<int>("b"), param<int>("g"), param<int>("r"));
    int thickness = param<int>("thickness");

    img->value.copyTo(out->value);

    BOOST_FOREACH(const RoiMessage::Ptr& roi, rois->value) {
        cv::rectangle(out->value, roi->value, color, thickness);
    }

    output_->publish(out);
}

void RenderROIs::setup()
{
    setSynchronizedInputs(true);

    input_img_ = addInput<CvMatMessage>("Image");
    input_rois_ = addInput<VectorMessage<RoiMessage> >("ROIs");

    output_ = addOutput<CvMatMessage>("Rendered Image");
}
