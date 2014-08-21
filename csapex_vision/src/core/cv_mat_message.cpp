/// HEADER
#include <csapex_vision/cv_mat_message.h>

using namespace csapex;
using namespace connection_types;


CvMatMessage::CvMatMessage()
    : MessageTemplate<cv::Mat, CvMatMessage> ("/camera"), encoding(enc::bgr)
{}

CvMatMessage::CvMatMessage(const Encoding& encoding)
    : MessageTemplate<cv::Mat, CvMatMessage> ("/camera"), encoding(encoding)
{}

ConnectionType::Ptr CvMatMessage::clone() {
    Ptr new_msg(new CvMatMessage(encoding));
    value.copyTo(new_msg->value);
    return new_msg;
}

void CvMatMessage::writeRaw(const std::string &path, const std::string &suffix) const
{
    std::string file = path + "/img" + suffix + ".jpg";
    cv::imwrite(file, value);
}

const Encoding& CvMatMessage::getEncoding() const
{
    return encoding;
}

void CvMatMessage::setEncoding(const Encoding &e)
{
    encoding = e;
}



/// YAML
namespace YAML {
Node convert<csapex::connection_types::CvMatMessage>::encode(const csapex::connection_types::CvMatMessage& rhs) {
    std::cerr << "CvMatMessage can't be encoded" << std::endl;
    Node node;
    return node;
}

bool convert<csapex::connection_types::CvMatMessage>::decode(const Node& node, csapex::connection_types::CvMatMessage& rhs) {
    std::cerr << "CvMatMessage can't be decoded" << std::endl;
    return true;
}
}
