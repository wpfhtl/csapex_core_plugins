#include "convert_type.h"

/// PROJECT
#include <csapex/msg/io.h>
#include <csapex/param/parameter_factory.h>
#include <csapex_opencv/cv_mat_message.h>
#include <csapex/model/node_modifier.h>
#include <csapex/utility/register_apex_plugin.h>

using namespace csapex;
using namespace csapex::connection_types;
using namespace csapex;

CSAPEX_REGISTER_CLASS(csapex::ConvertType, csapex::Node)

namespace {
    template<typename Tp>
    inline void normalize(cv::Mat &mat)
    {
        double min;
        double max;
        cv::minMaxLoc(mat, &min, &max);
        mat = mat - min;
        max -= min;
        mat = mat / max;
        Tp fac = std::numeric_limits<Tp>::max();
        mat = mat * fac;
    }
}

ConvertType::ConvertType() :
    mode_(CV_8U)
{
}

void ConvertType::process()
{
    CvMatMessage::ConstPtr in = msg::getMessage<connection_types::CvMatMessage>(input_);
    CvMatMessage::Ptr out(new connection_types::CvMatMessage(in->getEncoding(), in->stamp_micro_seconds));

    switch (mode_) {
    case CV_8U:
        if(normalize_)
            normalize<unsigned char>(out->value);
        in->value.convertTo(out->value,  CV_8UC(in->value.channels()));
        break;
    case CV_8S:
        if(normalize_)
            normalize<signed char>(out->value);
        in->value.convertTo(out->value,  CV_8SC(in->value.channels()));
        break;
    case CV_16U:
        if(normalize_)
            normalize<unsigned short int>(out->value);
        in->value.convertTo(out->value, CV_16UC(in->value.channels()));
        break;
    case CV_16S:
        if(normalize_)
            normalize<signed short int>(out->value);
        in->value.convertTo(out->value, CV_16SC(in->value.channels()));
        break;
    case CV_32S:
        if(normalize_)
            normalize<signed int>(out->value);
        in->value.convertTo(out->value, CV_32SC(in->value.channels()));
        break;
    case CV_32F:
        if(normalize_)
            normalize<float>(out->value);
        in->value.convertTo(out->value, CV_32FC(in->value.channels()));
        break;
    case CV_64F:
        if(normalize_)
            normalize<double>(out->value);
        in->value.convertTo(out->value, CV_64FC(in->value.channels()));
        break;
    default:
        throw std::runtime_error("Unknown conversion goal type!");
        break;
    }

    msg::publish(output_, out);
}

void ConvertType::setup(NodeModifier& node_modifier)
{
    input_ =  node_modifier.addInput<CvMatMessage>("original");
    output_ = node_modifier.addOutput<CvMatMessage>("converted");
    update();
}

void ConvertType::setupParameters(Parameterizable& parameters)
{
    std::map<std::string, int> types = {
        {" 8 Bit unsigned", CV_8U},
        {" 8 Bit signed",   CV_8S},
        {"16 Bit usigned",  CV_16U},
        {"16 Bit signed",   CV_16S},
        {"32 Bit signed",   CV_32S},
        {"32 Bit floating", CV_32F},
        {"64 Bit floating", CV_64F}
    };

    parameters.addParameter(csapex::param::ParameterFactory::declareParameterSet("convert to", types, CV_8U),
                            std::bind(&ConvertType::update, this));

    parameters.addParameter(csapex::param::ParameterFactory::declareBool("normalize", false),
                            std::bind(&ConvertType::update, this));
}

void ConvertType::update()
{
    mode_ = readParameter<int>("convert to");
}
