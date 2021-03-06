#ifndef HOGCLASSIFIER_H
#define HOGCLASSIFIER_H

/// PROJECT
#include <csapex/model/node.h>
#include "hog.h"

/// https://github.com/DaHoC/trainHOG/wiki/trainHOG-Tutorial
/// TO CLASSIFY ROI DIRECTLY

namespace csapex {
class HOGClassifier : public csapex::Node
{
public:

    HOGClassifier();

    void setupParameters(Parameterizable& parameters) override;
    void setup(csapex::NodeModifier& node_modifier) override;
    void process() override;

private:
    enum ThresholdType { GREATER = 0, LESS, LESS_EQUAL, GREATER_EQUAL};
    enum SVMType  {DEFAULT = 0, DAIMLER, CUSTOM, NONE};
    enum AdaptionType {SCALE, TRY_GROW, GROW_STRICT};
    enum ClassificationType {BACKGROUND = 0, HUMAN = 1, HUMAN_PART = 2, UNKNOWN = 3};

    csapex::Input  *in_img_;
    csapex::Input  *in_rois_;
    csapex::Output *out_rois_;

    HOGDescriptor   hog_;
    SVMType         prev_svm_type_;
    bool            mirror_;
    int             cells_x_;
    int             cells_y_;
    int             cell_size_;
    int             block_size_;
    int             block_stride_;
    int             n_bins_;
    bool            signed_gradient_;
    int             adaption_type_;
    double          ratio_hog_;

    std::vector<float> svm_;
    int                svm_type_;
    double             svm_thresh_;
    int                svm_thresh_type_;

    bool getData(const cv::Mat &src, const cv::Rect &roi, cv::Mat &dst);
    void load();
    void setParameters(const int cell_size, const int cells_x,
                       const int cells_y,
                       const int block_size,
                       const int block_stride,
                       const int bins,
                       const bool signed_gradient);
};
}
#endif // HOGCLASSIFIER_H
