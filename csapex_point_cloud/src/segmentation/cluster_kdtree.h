#ifndef CLUSTERPOINTCLOUDKDTREE_H
#define CLUSTERPOINTCLOUDKDTREE_H

/// PROJECT
#include <csapex/model/node.h>
#include <csapex_point_cloud/point_cloud_message.h>
#include <csapex/profiling/timer.h>
#include <csapex/profiling/interlude.hpp>
#include <cslibs_kdtree/kdtree.hpp>

namespace detail
{
struct NodeIndex
{
    using                           BinType     = std::array<double, 3>;
    using                           Type        = std::array<int, 3>;
    using                           PivotType   = double;
    static constexpr std::size_t    Dimension   = 3;

    const BinType bin_sizes;

    NodeIndex(const BinType& bin_sizes) : bin_sizes(bin_sizes) {}

    template<typename PointT>
    inline constexpr bool is_valid(const PointT& point) const
    {
        return !(std::isnan(point.x) || std::isnan(point.y) || std::isnan(point.z));
    }

    template<typename PointT>
    inline constexpr Type create(const PointT& point) const
    {
        return { static_cast<int>(std::floor(point.x / bin_sizes[0])),
                 static_cast<int>(std::floor(point.y / bin_sizes[1])),
                 static_cast<int>(std::floor(point.z / bin_sizes[2]))};
    }
};

struct NodeData : public kdtree::KDTreeNodeClusteringSupport
{
    std::vector<std::size_t> indices;

    NodeData() = default;

    template<typename PointT>
    inline NodeData(const PointT& point, std::size_t index)
    {
        indices.emplace_back(index);
    }

    inline void merge(const NodeData& other)
    {
        indices.insert(indices.end(),
                       other.indices.begin(),
                       other.indices.end());
    }
};

using KDTree = kdtree::buffered::KDTree<NodeIndex, NodeData>;
using KDTreePtr = std::shared_ptr<KDTree>;
}

namespace csapex
{
class ClusterPointcloudKDTree : public csapex::Node
{
public:

    ClusterPointcloudKDTree();

    virtual void setup(csapex::NodeModifier& node_modifier) override;
    virtual void setupParameters(Parameterizable &parameters) override;
    virtual void process() override;

    template <class PointT>
    void inputCloud(typename pcl::PointCloud<PointT>::ConstPtr cloud);

private:
    Input*  in_cloud_;
    Input*  in_indices_;
    Output* out_;
    Output* out_debug_;

    struct ClusterParams
    {
        std::array<double, 3>   bin_sizes;
        std::array<int, 2>      cluster_sizes;
    } cluster_params_;

    std::size_t        last_size_;
    detail::KDTreePtr  kdtree_;

};
}
#endif // CLUSTERPOINTCLOUDKDTREE_H
