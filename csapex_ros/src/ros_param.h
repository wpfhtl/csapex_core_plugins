#ifndef ROS_PARAM_H
#define ROS_PARAM_H

/// COMPONENT

/// PROJECT
#include <csapex_ros/ros_node.h>

/// SYSTEM
#include <ros/service.h>

namespace csapex {


class RosParam : public csapex::RosNode
{
public:
    RosParam();

    void setupParameters();
    void setupROS();
    void processROS();

private:
    void update();

private:
    Input* in_;
    Output* out_;
};


}

#endif // ROS_PARAM_H
