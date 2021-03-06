#ifndef ADABOOST_H
#define ADABOOST_H

/// COMPONENT
#include <csapex_ml/features_message.h>

/// PROJECT
#include <csapex/model/node.h>

/// SYSTEM
#include <opencv2/opencv.hpp>

namespace csapex {
class CSAPEX_EXPORT_PLUGIN AdaBoost : public Node
{
public:
    AdaBoost();

    virtual void setup(NodeModifier &node_modifier) override;
    virtual void setupParameters(Parameterizable &parameters) override;
    virtual void process() override;

private:
    Input  *in_;
    Output *out_;
    Slot   *reload_;

    bool        loaded_;
    bool        compute_labels_;
    cv::Boost   boost_;
    std::string path_;

    void reload();
    void updateMethod();
};
}
#endif // ADABOOST_H
