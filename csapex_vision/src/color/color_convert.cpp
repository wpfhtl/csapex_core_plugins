/// HEADER
#include "color_convert.h"

/// PROJECT
#include <csapex/msg/io.h>
#include <csapex/param/parameter_factory.h>
#include <csapex/model/node_modifier.h>
#include <csapex/utility/register_apex_plugin.h>

CSAPEX_REGISTER_CLASS(csapex::ColorConvert, csapex::Node)

using namespace csapex;
using namespace csapex;
using namespace csapex::connection_types;

ColorConvert::ColorConvert()
{
}

ColorConvert::~ColorConvert()
{
}

void ColorConvert::setup(NodeModifier& node_modifier)
{
    input_img_ = node_modifier.addInput<CvMatMessage>("original");
    output_img_ = node_modifier.addOutput<CvMatMessage>("converted");
}

void ColorConvert::setupParameters(Parameterizable& parameters)
{
    std::map<std::string, int> encodings = {
        {"YUV", (int) YUV},
        {"RGB", (int) RGB},
        {"BGR", (int) BGR},
        {"HSL", (int) HSL},
        {"HSV", (int) HSV},
        {"MONO",(int) MONO},
        {"LAB", (int) LAB}
    };

    parameters.addParameter(csapex::param::ParameterFactory::declareParameterSet("input", encodings, (int) BGR));
    parameters.addParameter(csapex::param::ParameterFactory::declareParameterSet("output", encodings, (int) MONO));

    cs_pair_to_operation_.insert(csiPair(csPair(RGB, BGR), (int) CV_RGB2BGR));
    cs_pair_to_operation_.insert(csiPair(csPair(BGR, RGB), (int) CV_BGR2RGB));

    cs_pair_to_operation_.insert(csiPair(csPair(RGB, YUV), (int) CV_RGB2YUV));
    cs_pair_to_operation_.insert(csiPair(csPair(BGR, YUV), (int) CV_BGR2YUV));
    cs_pair_to_operation_.insert(csiPair(csPair(YUV, RGB), (int) CV_YUV2RGB));
    cs_pair_to_operation_.insert(csiPair(csPair(YUV, BGR), (int) CV_YUV2BGR));

    cs_pair_to_operation_.insert(csiPair(csPair(RGB, HSV), (int) CV_RGB2HSV));
    cs_pair_to_operation_.insert(csiPair(csPair(BGR, HSV), (int) CV_BGR2HSV));
    cs_pair_to_operation_.insert(csiPair(csPair(HSV, RGB), (int) CV_HSV2RGB));
    cs_pair_to_operation_.insert(csiPair(csPair(HSV, BGR), (int) CV_HSV2BGR));

    cs_pair_to_operation_.insert(csiPair(csPair(RGB, HSL), (int) CV_RGB2HLS));
    cs_pair_to_operation_.insert(csiPair(csPair(BGR, HSL), (int) CV_BGR2HLS));
    cs_pair_to_operation_.insert(csiPair(csPair(HSL, RGB), (int) CV_HLS2RGB));
    cs_pair_to_operation_.insert(csiPair(csPair(HSL, BGR), (int) CV_HLS2BGR));

    cs_pair_to_operation_.insert(csiPair(csPair(RGB, MONO), (int) CV_RGB2GRAY));
    cs_pair_to_operation_.insert(csiPair(csPair(BGR, MONO), (int) CV_BGR2GRAY));
    cs_pair_to_operation_.insert(csiPair(csPair(MONO, RGB), (int) CV_GRAY2RGB));
    cs_pair_to_operation_.insert(csiPair(csPair(MONO, BGR), (int) CV_GRAY2BGR));

    cs_pair_to_operation_.insert(csiPair(csPair(RGB, LAB), (int) CV_RGB2Lab));
    cs_pair_to_operation_.insert(csiPair(csPair(BGR, LAB), (int) CV_BGR2Lab));
    cs_pair_to_operation_.insert(csiPair(csPair(LAB, RGB), (int) CV_Lab2RGB));
    cs_pair_to_operation_.insert(csiPair(csPair(LAB, BGR), (int) CV_Lab2BGR));

    cs_to_encoding_[YUV]  = enc::yuv;
    cs_to_encoding_[RGB]  = enc::rgb;
    cs_to_encoding_[BGR]  = enc::bgr;
    cs_to_encoding_[HSL]  = enc::hsl;
    cs_to_encoding_[HSV]  = enc::hsv;
    cs_to_encoding_[MONO] = enc::mono;
    cs_to_encoding_[LAB]  = enc::lab;
}

void ColorConvert::process()
{
    CvMatMessage::ConstPtr img = msg::getMessage<CvMatMessage>(input_img_);

    csPair cspair;
    cspair.first  = static_cast<ColorSpace> (readParameter<int>("input"));
    cspair.second = static_cast<ColorSpace> (readParameter<int>("output"));

    CvMatMessage::Ptr out(new CvMatMessage(cs_to_encoding_[cspair.second], img->stamp_micro_seconds));

    if(img->getEncoding().channelCount() != cs_to_encoding_[cspair.first].channelCount()) {
        std::stringstream error;
        error << "Conversion not applicable! Input encoding #" << img->getEncoding().channelCount() << ", target #"  <<  cs_to_encoding_[cspair.first].channelCount();
        throw std::runtime_error(error.str());
    }

    if(cspair.first != cspair.second) {
        if(cs_pair_to_operation_.find(cspair) != cs_pair_to_operation_.end()) {
            int mode = cs_pair_to_operation_[cspair];
            cv::cvtColor(img->value, out->value, mode);


            if((int) out->getEncoding().channelCount() != out->value.channels()) {
                throw std::runtime_error("Conversion didn't work!");
            }
        } else {
            throw std::runtime_error("Conversion not supported!");
        }
        msg::publish(output_img_, out);

    } else {
        msg::publish(output_img_, img);
    }
}
