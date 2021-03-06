#ifndef ENTRY_HPP
#define ENTRY_HPP

#include <distribution.hpp>
#include <mean.hpp>

namespace csapex {
using DataIndex = std::array<int, 3>;

struct Entry {
    int                      cluster;
    DataIndex                index;
    std::vector<int>         indices;
    bool                     valid;

    Entry() :
        cluster(-1),
        valid(false)
    {
    }
};

struct EntryStatistical  : public Entry
{
    math::Mean<1>         depth_mean;
    math::Distribution<3> distribution;
    EntryStatistical() :
        Entry()
    {
    }
};

struct EntryStatisticalColor : public EntryStatistical
{
    math::Mean<3> color_mean;
    EntryStatisticalColor() :
        EntryStatistical()
    {
    }
};

struct EntryStatisticalMono : public EntryStatistical
{
    math::Mean<1> mono_mean;
    EntryStatisticalMono() :
        EntryStatistical()
    {
    }
};




}

#endif // ENTRY_HPP
