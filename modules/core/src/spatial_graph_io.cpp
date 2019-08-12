/* ********************************************************************
 * Copyright (C) 2020 Pablo Hernandez-Cerdan.
 *
 * This file is part of SGEXT: http://github.com/phcerdan/sgext.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 *
 * *******************************************************************/

#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/graph/adj_list_serialize.hpp>
#include <boost/graph/graphviz.hpp>

#include "spatial_graph_io.hpp"

namespace SG {

boost::dynamic_properties get_write_dynamic_properties_sg(GraphType &graph) {
    boost::dynamic_properties dp;
    dp.property("node_id", boost::get(boost::vertex_index, graph));
    dp.property("spatial_node", boost::get(boost::vertex_bundle, graph));
    dp.property("spatial_edge", boost::get(boost::edge_bundle, graph));
    return dp;
}
boost::dynamic_properties get_read_dynamic_properties_sg(GraphType &graph) {
    boost::dynamic_properties dp;
    dp.property("node_id", boost::get(&SpatialNode::id, graph));
    dp.property("spatial_node", boost::get(boost::vertex_bundle, graph));
    dp.property("spatial_edge", boost::get(boost::edge_bundle, graph));
    return dp;
}

void write_graphviz_sg(std::ostream &os, GraphType &graph) {
    auto dp = get_write_dynamic_properties_sg(graph);
    boost::write_graphviz_dp(os, graph, dp);
}
void write_graphviz_sg(const std::string &output_file, GraphType &graph) {
    auto dp = get_write_dynamic_properties_sg(graph);
    std::ofstream ofile(output_file);
    boost::write_graphviz_dp(ofile, graph, dp);
}

void read_graphviz_sg(std::istream &is, GraphType &graph) {
    auto dp = get_read_dynamic_properties_sg(graph);
    boost::read_graphviz(is, graph, dp);
}
void read_graphviz_sg(const std::string &input_file, GraphType &graph) {
    auto dp = get_read_dynamic_properties_sg(graph);
    std::ifstream ifile(input_file);
    boost::read_graphviz(ifile, graph, dp);
}

GraphType read_graphviz_sg(const std::string &input_file) {
    GraphType graph;
    read_graphviz_sg(input_file, graph);
    return graph;
}

// Serialize
void write_serialized_sg(std::ostream &os, GraphType &graph) {
    boost::archive::text_oarchive arch(os);
    arch << graph;
}
void write_serialized_sg(const std::string &output_file, GraphType &graph) {
    std::ofstream os(output_file);
    return write_serialized_sg(os, graph);
}

void read_serialized_sg(std::istream &is, GraphType &graph) {
    boost::archive::text_iarchive arch(is);
    arch >> graph;
}
void read_serialized_sg(const std::string &input_file, GraphType &graph) {
    std::ifstream ifile(input_file);
    read_serialized_sg(ifile, graph);
}

GraphType read_serialized_sg(const std::string &input_file) {
    GraphType graph;
    read_serialized_sg(input_file, graph);
    return graph;
}

} // namespace SG
