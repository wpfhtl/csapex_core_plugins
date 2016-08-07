#ifndef ADABOOST_H
#define ADABOOST_H

/// COMPONENT
#include <csapex_ml/features_message.h>

/// PROJECT
#include <csapex/model/node.h>

/// SYSTEM
#include <opencv2/opencv.hpp>

namespace csapex {
class AdaBoost : public Node
{
public:
    AdaBoost();

    virtual void setup(NodeModifier &node_modifier) override;
    virtual void setupParameters(Parameterizable &parameters) override;
    virtual void process() override;

private:
    Input  *in_;
    Output *out_;

    bool      loaded_;
    bool      compute_labels_;
    cv::Boost boost_;


    void load();
};
}
#endif // ADABOOST_H