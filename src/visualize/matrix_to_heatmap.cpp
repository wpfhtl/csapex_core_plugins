/// HEADER
#include "matrix_to_heatmap.h"

/// PROJECT
#include <csapex/utility/register_apex_plugin.h>
#include <csapex/model/connector_in.h>
#include <csapex/model/connector_out.h>
#include <utils_param/parameter_factory.h>
#include <utils_cv/heatmap.hpp>
#include <csapex_vision/cv_mat_message.h>

/// SYSTEM
#include <boost/assign/list_of.hpp>
#include <boost/function.hpp>

CSAPEX_REGISTER_CLASS(vision_plugins::MatrixToHeatmap, csapex::Node)

using namespace csapex;
using namespace csapex::connection_types;
using namespace vision_plugins;

MatrixToHeatmap::MatrixToHeatmap() :
    color_type_(BEZIER)
{
    Tag::createIfNotExists("Visualization");
    addTag(Tag::get("Visualization"));
    addTag(Tag::get("Vision"));
    addTag(Tag::get("vision_plugins"));

    std::map<std::string, int> types = boost::assign::map_list_of
            ("BEZIER", (int) BEZIER)
            ("PARABOLA", (int) PARABOLA);

    addParameter(param::ParameterFactory::declareParameterSet<int>("coloring", types),
                 boost::bind(&MatrixToHeatmap::update, this));

}

void MatrixToHeatmap::process()
{
    CvMatMessage::Ptr in = input_->getMessage<connection_types::CvMatMessage>();
    CvMatMessage::Ptr out(new connection_types::CvMatMessage(in->getEncoding()));


    cv::Mat working = in->value.clone();
    cv::Mat heatmap (working.rows, working.cols, CV_32FC3, cv::Scalar::all(0));
    cv::Mat mean    (working.rows, working.cols, CV_32FC1, cv::Scalar::all(0));
    cv::Mat mask;
    if(mask_->isConnected()) {
        CvMatMessage::Ptr mask_msg = mask_->getMessage<connection_types::CvMatMessage>();
        mask = mask_msg->value;
    } else {
        mask = cv::Mat(working.rows, working.cols, CV_8UC1, 255);
    }

    std::vector<cv::Mat> channels;
    cv::split(working, channels);

    for(std::vector<cv::Mat>::iterator it = channels.begin() ; it != channels.end() ; ++it) {
        it->convertTo(*it, CV_32FC1);
        mean += *it;
    }

    float divider = 1 / (float) channels.size();
    mean = mean * divider;

    utils_cv::Heatmap::colorFunction fc;
    switch(color_type_) {
    case BEZIER:
        fc = &utils_cv::Heatmap::bezierColor;
        break;
    case PARABOLA:
        fc = &utils_cv::Heatmap::parabolaColor;
        break;
    default:
        throw std::runtime_error("Unknown color function type!");
    }

    utils_cv::Heatmap::renderHeatmap(mean, heatmap, fc, mask);

    out->value = heatmap;
    output_->publish(out);
}

void MatrixToHeatmap::setup()
{
    setSynchronizedInputs(true);

    input_ = addInput<CvMatMessage>("matrix");
    output_ = addOutput<CvMatMessage>("heatmap");
    mask_   = addInput<CvMatMessage>("mask",true);

    update();
}

void MatrixToHeatmap::update()
{
    color_type_ = (ColorType) param<int>("coloring");
}
