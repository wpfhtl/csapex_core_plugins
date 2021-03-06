/// HEADER
#include <csapex_scan_2d/scan_message.h>

/// PROJECT
#include <csapex/utility/assert.h>
#include <csapex/utility/register_msg.h>
#include <cslibs_laser_processing/common/yaml-io.hpp>

CSAPEX_REGISTER_MESSAGE(csapex::connection_types::ScanMessage)

using namespace csapex;
using namespace connection_types;
using namespace lib_laser_processing;

ScanMessage::ScanMessage()
    : MessageTemplate<lib_laser_processing::Scan, ScanMessage> ("/")
{}



/// YAML
namespace YAML {
Node convert<csapex::connection_types::ScanMessage>::encode(const csapex::connection_types::ScanMessage& rhs)
{
    Node node = convert<csapex::connection_types::Message>::encode(rhs);

    node["value"] = rhs.value;
    return node;
}

bool convert<csapex::connection_types::ScanMessage>::decode(const Node& node, csapex::connection_types::ScanMessage& rhs)
{
    if(!node.IsMap()) {
        return false;
    }

    convert<csapex::connection_types::Message>::decode(node, rhs);
    rhs.value = node.as<Scan>();
    return true;
}
}
