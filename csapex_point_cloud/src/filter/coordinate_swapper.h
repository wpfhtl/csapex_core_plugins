#ifndef COORDINATE_SWAPPER_H
#define COORDINATE_SWAPPER_H

/// PROJECT
#include <csapex/model/node.h>
#include <csapex_point_cloud/point_cloud_message.h>

namespace csapex {
class CoordinateSwapper : public Node
{
public:
    CoordinateSwapper();

    virtual void setup(csapex::NodeModifier& node_modifier) override;
    virtual void process() override;

    template <class PointT>
    void inputCloud(typename pcl::PointCloud<PointT>::ConstPtr cloud);

private:
    Input*  input_;
    Output* output_;


};
}
#endif // COORDINATE_SWAPPER_H
