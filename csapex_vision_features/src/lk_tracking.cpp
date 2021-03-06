/// HEADER
#include "lk_tracking.h"

/// PROJECT
#include <csapex/model/tag.h>
#include <csapex/msg/io.h>
#include <csapex_opencv/cv_mat_message.h>
#include <csapex/param/parameter_factory.h>
#include <csapex/model/node_modifier.h>
#include <csapex/utility/register_apex_plugin.h>
#include <csapex_vision_features/keypoint_message.h>

/// SYSTEM


CSAPEX_REGISTER_CLASS(csapex::LKTracking, csapex::Node)

using namespace csapex;
using namespace connection_types;

LKTracking::LKTracking()
    : init_(true)
{
}

void LKTracking::setupParameters(Parameterizable &parameters)
{
    std::function<void(csapex::param::Parameter*)> cb = std::bind(&LKTracking::update, this, std::placeholders::_1);

    parameters.addParameter(csapex::param::ParameterFactory::declareRange<int>("winSize", 10, 80, 31, 1));
    parameters.addParameter(csapex::param::ParameterFactory::declareRange<int>("subPixWinSize", 1, 40, 10, 1), cb);

    parameters.addParameter(csapex::param::ParameterFactory::declareTrigger("reset"), cb);

    parameters.addParameter(csapex::param::ParameterFactory::declareRange<int>("debug/circlesize", 1, 15, 2, 1));
}

void LKTracking::process()
{
    CvMatMessage::ConstPtr img = msg::getMessage<CvMatMessage>(in_image_);
    if(!img->hasChannels(1, CV_8U)) {
        throw std::runtime_error("input image must be 1-channel");
    }

    KeypointMessage::ConstPtr keypoints = msg::getMessage<KeypointMessage>(in_keypoints_);

    // TODO: parameterize
    cv::TermCriteria termcrit(CV_TERMCRIT_ITER|CV_TERMCRIT_EPS, 20, 0.03);

    if(init_) {
        points[0].clear();
        points[1].clear();

        if(keypoints->value.empty()) {
            node_modifier_->setWarning("No input points, cannot initialize LK Tracker");
            return;
        }

        node_modifier_->setNoError();

        int spws = readParameter<int>("subPixWinSize");
        cv::Size subPixWinSize (spws, spws);


        // extract the points from the keypoints
        for(std::vector<cv::KeyPoint>::const_iterator it = keypoints->value.begin(); it != keypoints->value.end(); ++it) {
            const cv::KeyPoint& kp = *it;
            points[1].push_back(kp.pt);
        }
        cv::cornerSubPix(img->value, points[1], subPixWinSize, cv::Size(-1,-1), termcrit);
        init_ = false;

    } else if(!points[0].empty()) {
        int ws = readParameter<int>("winSize");
        cv::Size winSize(ws, ws);

        std::vector<uchar> status;
        std::vector<float> err;

        cv::calcOpticalFlowPyrLK(prevGray, img->value, points[0], points[1], status, err, winSize,
                3, termcrit, 0, 0.001);

        size_t k = 0;

        CvMatMessage::Ptr out_dbg(new CvMatMessage(enc::bgr, img->stamp_micro_seconds));

        bool debug = msg::isConnected(out_debug_);
        if(debug) {
            cv::cvtColor(img->value, out_dbg->value, CV_GRAY2BGR);
        }

        int circlesize = readParameter<int>("debug/circlesize");

        for( std::size_t i = 0; i < points[1].size(); i++ ) {
            if( !status[i] )
                continue;

            if(debug) {
                points[1][k] = points[1][i];
                cv::circle(out_dbg->value, points[1][i], circlesize, cv::Scalar(0,255,0), -1, 8);
            }

            ++k;
        }

        if(debug) {
            msg::publish(out_debug_, out_dbg);
        }
        points[1].resize(k);
    }

    std::swap(points[1], points[0]);

    img->value.copyTo(prevGray);
}

void LKTracking::reset()
{
    init_ = true;
}

void LKTracking::update(const csapex::param::Parameter*)
{
    reset();
}

void LKTracking::setup(NodeModifier& node_modifier)
{
    in_image_ = node_modifier.addInput<CvMatMessage>("Image");
    in_keypoints_ = node_modifier.addInput<KeypointMessage>("Keypoints");

    out_debug_ = node_modifier.addOutput<CvMatMessage>("Debug");
}
