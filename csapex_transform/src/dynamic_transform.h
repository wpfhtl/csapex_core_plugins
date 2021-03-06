#ifndef DYNAMIC_TRANSFORM_H
#define DYNAMIC_TRANSFORM_H

/// PROJECT
#include <csapex/model/tickable_node.h>
#include <csapex/param/set_parameter.h>

/// SYSTEM
#include <ros/time.h>
#include <tf/tf.h>
#include <boost/optional.hpp>

namespace csapex {

class DynamicTransform : public csapex::TickableNode
{
public:
    DynamicTransform();

    virtual void setup(csapex::NodeModifier& node_modifier) override;
    virtual void setupParameters(Parameterizable& parameters) override;

    virtual void process() override;
    virtual bool canTick() override;
    virtual void tick() override;
    void update();

    void refresh();
    void resetTf();

    void freeze(bool frozen);

private:
    void publishTransform(const ros::Time& time);

private:
    Output* output_;

    Input* time_in_;

    Event* reset_;

    bool init_;
    int initial_retries_;

    param::SetParameter::Ptr source_p;
    param::SetParameter::Ptr target_p;

    bool exact_time_;

    bool frozen_;

    double roll;
    double pitch;
    double yaw;
    double x;
    double y;
    double z;

    boost::optional<tf::Transform> last_transform;
};

}

#endif // DYNAMIC_TRANSFORM_H
