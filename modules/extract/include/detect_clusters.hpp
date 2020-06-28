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

#ifndef SGEXT_DETECT_CLUSTERS_HPP
#define SGEXT_DETECT_CLUSTERS_HPP

#include "spatial_graph.hpp"

namespace SG {

/**
 * Use detect_clusters_visitor to obtain a cluster label map, mappings
 * each vertex that belongs to a cluster to a label.
 *
 * For assigning that cluster label to spatial_node_id use @ref
 * assign_cluster_label_to_spatial_node_id
 *
 *
 * @param input_sg graph from where detect clusters
 * @param cluster_radius the cluster condition
 * @param verbose
 *
 * @return  cluster label map
 */
std::unordered_map<GraphType::vertex_descriptor, GraphType::vertex_descriptor>
detect_clusters_with_radius(const GraphType &input_sg,
                            const double &cluster_radius,
                            bool use_cluster_centroid = true,
                            bool verbose = false);

/**
 * Modify spatial_node::id of input_sg with the cluster label.
 * Only modifies spatial_node::id that belong to a cluster.
 *
 * Useful for visualization.
 *
 * @param input_sg  input spatial graph
 * @param cluster_label_map obtained from @ref detect_clusters_with_radius or
 * similar.
 */
void assign_cluster_label_to_spatial_node_id(
        GraphType &input_sg,
        std::unordered_map<GraphType::vertex_descriptor, GraphType::vertex_descriptor>
                &cluster_label_map);
} // end namespace SG
#endif