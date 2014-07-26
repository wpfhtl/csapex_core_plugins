/// HEADER
#include "color_pointcloud.h"

/// PROJECT
#include <csapex/model/connector_in.h>
#include <csapex/model/connector_out.h>
#include <csapex_vision/cv_mat_message.h>
#include <utils_param/parameter_factory.h>
#include <csapex/utility/color.hpp>
#include <csapex/model/node_modifier.h>
#include <csapex_point_cloud/point_cloud_message.h>
#include <csapex/utility/register_apex_plugin.h>

/// SYSTEM
#include <pcl/point_types.h>
#include <pcl/conversions.h>


CSAPEX_REGISTER_CLASS(csapex::ColorPointCloud, csapex::Node)

using namespace csapex;
using namespace csapex::connection_types;

#define FLOOD_DEFAULT_LABEL 0

ColorPointCloud::ColorPointCloud()
{
}

void ColorPointCloud::process()
{
    PointCloudMessage::Ptr msg(input_->getMessage<PointCloudMessage>());

    boost::apply_visitor (PointCloudMessage::Dispatch<ColorPointCloud>(this, msg), msg->value);
}

void ColorPointCloud::setup()
{
    input_  = modifier_->addInput<PointCloudMessage>("Labeled PointCloud");
    output_ = modifier_->addOutput<PointCloudMessage>("Colored PointCloud");
}

namespace implementation {

struct Color {
    Color(uchar _r = 0, uchar _g = 0, uchar _b = 0) :
        r(_r),
        g(_g),
        b(_b){}

    uchar r;
    uchar g;
    uchar b;
};

template<class PointT>
struct Impl {
    inline static void convert(const typename pcl::PointCloud<PointT>::Ptr src,
                               typename pcl::PointCloud<pcl::PointXYZRGB>::Ptr dst)
    {
        dst->height = src->height;
        dst->header = src->header;
        dst->width  = src->width;

        std::map<unsigned int, Color> colors;
        colors.insert(std::make_pair(FLOOD_DEFAULT_LABEL, Color()));
        for(typename pcl::PointCloud<PointT>::const_iterator it = src->begin() ; it != src->end() ; ++it) {
            if(colors.find(it->label) == colors.end()) {
                double r,g,b;
                color::fromCount(colors.size(), r,g,b);
                colors.insert(std::make_pair(it->label,Color(r,g,b)));
            }
            Color c = colors.at(it->label);
            pcl::PointXYZRGB p(c.r, c.g, c.b);
            p.x = it->x;
            p.y = it->y;
            p.z = it->z;
            dst->push_back(p);
        }
    }
};

template<class PointT>
struct Conversion {
    static void apply(const typename pcl::PointCloud<PointT>::Ptr src,
                      typename pcl::PointCloud<pcl::PointXYZRGB>::Ptr dst)
    {
        throw std::runtime_error("Type of pointcloud must be labeled!");
    }
};

template<>
struct Conversion<pcl::PointXYZL>{
    static void apply(const typename pcl::PointCloud<pcl::PointXYZL>::Ptr src,
                      typename pcl::PointCloud<pcl::PointXYZRGB>::Ptr dst)
    {
        Impl<pcl::PointXYZL>::convert(src, dst);
    }

};

template<>
struct Conversion<pcl::PointXYZRGBL>{
    static void apply(const typename pcl::PointCloud<pcl::PointXYZRGBL>::Ptr src,
                      typename pcl::PointCloud<pcl::PointXYZRGB>::Ptr dst)
    {
        Impl<pcl::PointXYZRGBL>::convert(src, dst);
    }
};
}

template <class PointT>
void ColorPointCloud::inputCloud(typename pcl::PointCloud<PointT>::Ptr cloud)
{
    PointCloudMessage::Ptr out(new PointCloudMessage(cloud->header.frame_id));
    pcl::PointCloud<pcl::PointXYZRGB>::Ptr out_cloud(new pcl::PointCloud<pcl::PointXYZRGB>);

    ainfo << cloud->header.frame_id << "!" << std::endl;
    implementation::Conversion<PointT>::apply(cloud, out_cloud);

    out->value = out_cloud;
    output_->publish(out);
}
