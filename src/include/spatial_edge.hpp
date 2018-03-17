#ifndef SPATIAL_EDGE_HPP
#define SPATIAL_EDGE_HPP

#include "array_utilities.hpp"
#include <vector>

namespace SG {
using PointType = ArrayUtilities::Array3D;
using PointContainer = std::vector<PointType>;

struct SpatialEdge {
    using PointType = ArrayUtilities::Array3D;
    using PointContainer = std::vector<PointType>;
    /// Spatial Points between the nodes of the edge.
    PointContainer edge_points;
};

/* Stream operators */
inline static std::ostream &operator<<(std::ostream &os,
                                       const SpatialEdge &se) {
    auto size = se.edge_points.size();
    os << "[";
    for (size_t i = 0; i + 1 < size; ++i) {
        os << "{" << se.edge_points[i][0] << " " << se.edge_points[i][1] << " "
           << se.edge_points[i][2] << "},";
    }
    if (size > 0) {
        os << "{" << se.edge_points[size - 1][0] << " " << se.edge_points[size - 1][1] << " "
            << se.edge_points[size - 1][2] << "}";
    }
    os << "]";
    return os;
}
inline static std::istream &operator>>(std::istream &is, SpatialEdge &se) {
    auto &edge_points = se.edge_points;

    std::string s(std::istreambuf_iterator<char>(is), {});
    std::string delim_start = "{";
    std::string delim_end = "}";
    auto first = s.find(delim_start);
    auto last = s.find(delim_end);
    auto pos = first;
    std::string clean;
    while ( pos != std::string::npos )
    {
        double x = 0;
        double y = 0;
        double z = 0;
        clean = s.substr(pos + delim_start.length() , last - delim_end.length());
        std::istringstream is_clean(clean);
        is_clean >> x >> y >> z;
        edge_points.push_back({{x,y,z}});
        s.erase(0, last + delim_end.length());
        pos = s.find(delim_start);
    }
    // set is to the end or lexical_cast fails.
    is.seekg (0, is.end);
    return is;
}

} // namespace SG
#endif
