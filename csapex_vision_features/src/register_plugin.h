#ifndef REGISTER_PLUGIN_H
#define REGISTER_PLUGIN_H

/// HEADER
#include <csapex/core/core_plugin.h>

namespace csapex {

class RegisterVisionFeaturePlugin : public CorePlugin
{
public:
    RegisterVisionFeaturePlugin();

    void init(CsApexCore& core);
};

}

#endif // REGISTER_PLUGIN_H
