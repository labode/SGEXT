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

#include "compare_graphs.hpp"
#include "get_vtk_points_from_graph.hpp"
#include "graph_points_locator.hpp"
#include "filter_spatial_graph.hpp"
#include <boost/graph/copy.hpp>
#include <boost/graph/iteration_macros.hpp>

namespace SG {

std::pair<EdgeDescriptorUnorderedSet, VertexDescriptorUnorderedSet>
remove_edges_and_nodes_from_high_info_graph(
        const GraphType & g0,
        const GraphType & g1)
{
    std::vector<std::reference_wrapper<const GraphType>> graphs;
    graphs.reserve(2);
    graphs.push_back(std::cref(g0));
    graphs.push_back(std::cref(g1));
    auto merger_map_pair = SG::get_vtk_points_from_graphs(graphs);
    auto & mergePoints = merger_map_pair.first;
    auto & idMap = merger_map_pair.second;
    auto kdtree = SG::build_kdtree_locator(mergePoints->GetPoints());

    // So... the big question: how do we compare graphs and construct the result?
    // a)
    // - iterate over every unique point of the kdtree.
    // Each point has a vector<graph_descriptor> associated to it.
    // - our candidates to take action on are points that do not exist in all the graphs.
    //  - kdtree locator can be used to find nearest point in other graphs.
    //
    // b) construct the graph
    // Use a filtered_graph (view)
    // filter by a set of vertex:
    // filter by a set of edges:
    // And optionally copy it into a new graph a the end

    SG::VertexDescriptorUnorderedSet remove_nodes;
    SG::EdgeDescriptorUnorderedSet remove_edges;
    // Iterate over all nodes of high-freq graph.
    // - Work on points that do not exist in the other graph
    // A) vertex in highG
    //  - edge_point in lowG ---> Means: new branch!
    //    Study neighbors of the vertex:
    // B) edge_point in highG
    //  - vertex in lowG ---> Means: branch is extended!
    //
    {
      GraphType::vertex_descriptor v;
      BGL_FORALL_VERTICES(v, g1, GraphType) {
        vtkIdType id = kdtree->FindClosestPoint(g1[v].pos.data());
        const auto & gdescs = idMap[id];
        const auto & gdesc0 = gdescs[0];
        const auto & gdesc1 = gdescs[1];
        assert(gdesc1.exist && gdesc1.is_vertex);
        if(!gdesc0.is_vertex) {
          // Interesting times, graph has evolved
          // We can:
          // - find the closest points per graph with graph_closest_points_by_radius_locator
          //   this does not use topology of graphs, and it is not ensured that the closest
          //   point is relevant, but should be good most of the time.
          //   It could be used to ensure that there is no vertex in the graph close by.
          //   (maybe gdesc0 does not exist because "noise" in pos, but the vertex is close by)
          //   TODO: Find points around graph0 to check that vertex really does not exit.
          // - or check neighbors of current vertex.

          // Case: Remove branch between existing
          // |  |
          // |__|
          // |  |
          if(gdesc0.exist) {
            GraphType::vertex_descriptor v_adj;
            BGL_FORALL_ADJ(v, v_adj, g1, GraphType){
              vtkIdType id_adj = kdtree->FindClosestPoint(g1[v_adj].pos.data());
              const auto & gdescs_adj = idMap[id_adj];
              const auto & gdesc_adj0 = gdescs_adj[0];
              // if it exists, but it is not a vertex
              if(gdesc_adj0.exist && gdesc_adj0.is_edge) {
                if(true) {
                  std::cout << "Source: v: " << v << " ; pos: " << g1[v].pos[0] <<", " << g1[v].pos[1] << std::endl;
                  std::cout << "Adj: v_adj: " << v_adj << " ; pos: " << g1[v_adj].pos[0] <<", " << g1[v_adj].pos[1] << std::endl;
                  SG::print_graph_descriptor(gdesc_adj0, "graph_desc at graph0");
                }
                // returns any edge between nodes. Ensure, TODO: HOW? there are no parallel edges.
                auto any_edge_exist = boost::edge(v, v_adj, g1);
                // edge exist for sure, no need to check with .second
                // also no need to worry about inserting the edge twice, the container is a set.
                remove_edges.insert(any_edge_exist.first);
              }
            }
          }
        }
      }
    }

    // TODO hard coded! FIX -- hit performance if too big, bad comparison if too low.
    double radius = 2.0;
    SG::EdgeDescriptorUnorderedSet growing_edges;
    // Iterate over all vertices of low info graph
    {
      GraphType::vertex_descriptor v;
      BGL_FORALL_VERTICES(v, g0, GraphType) {
        // vtkIdType id = kdtree->FindClosestPoint(g0[v].pos.data());
        auto closest_points_list_from_g0_vertex =
          SG::graph_closest_points_by_radius_locator(g0[v].pos,
              kdtree, idMap, radius);
        auto closest_descriptors_from_g0_vertex =
          closest_existing_descriptors_by_graph(closest_points_list_from_g0_vertex, idMap);

        // const auto & gdescs = idMap[closest_descriptors_from_g0_vertex[];
        const auto & gdesc0 = closest_descriptors_from_g0_vertex[0].descriptor;
        const auto & gdesc1 = closest_descriptors_from_g0_vertex[1].descriptor;
        assert(gdesc0.exist && gdesc0.is_vertex);
        // |__|
        // |__|
        // |  |
        if(gdesc1.is_edge) { // equivalent to !gdesc1.is_vertex && gdesc1.exist
          // Low graph has grown from an end point
          auto source_g1 = boost::source(gdesc1.edge_d, g1);
          auto target_g1 = boost::target(gdesc1.edge_d, g1);
          // vtkIdType id_source_g1 = kdtree->FindClosestPoint(g1[source_g1].pos.data());
          // vtkIdType id_target_g1 = kdtree->FindClosestPoint(g1[target_g1].pos.data());
          auto closest_points_list_from_source_g1 =
            SG::graph_closest_points_by_radius_locator(g1[source_g1].pos,
                kdtree, idMap, radius);
          auto closest_descriptors_from_source_g1 =
            closest_existing_descriptors_by_graph(closest_points_list_from_source_g1, idMap);
          auto closest_points_list_from_target_g1 =
            SG::graph_closest_points_by_radius_locator(g1[target_g1].pos,
                kdtree, idMap, radius);
          auto closest_descriptors_from_target_g1 =
            closest_existing_descriptors_by_graph(closest_points_list_from_target_g1, idMap);
          // const auto & gdescs_source_g1 = idMap[id_source_g1];
          // const auto & gdescs_target_g1 = idMap[id_target_g1];
          const auto & gdesc_source0 = gdescs_source_g1[0];
          const auto & gdesc_target0 = gdescs_target_g1[0];
          // TODO these source0/target0 are PROBABLY not existant!
          // DEVELOP a FindClosestPointInGraph(index_graph (0 or 1))
          // using the kdtree radius (sorted by distance), and return
          // the first point that exist in target graph. It is a projection.
          // Use it instead of FindClosestPoint
          // DONE
          //
          // if source0 is not existant
          // - the graph has grown/extended.
          // if source0 is an edge point.
          // - a merge into an existing vessel. EXPLORE FURTHER
          // if source0 is a node
          // - two branches headed in opposite directions have merged . GOOD
          // vtkIdType id_source0 = kdtree->FindClosestPoint(gdesc_target0.vertex_dpos.data());
          // vtkIdType id_target0 = kdtree->FindClosestPoint(target_g1_pos.data());
          // if further exploration generates a loop/cycle. Warning.
          // - It could be an ongoing valid merge: Get a higher info graph?
          // - It could be an ongoing invalid fusion: Get a higher info graph?
          // Discard both cases for now, a higher info graph will solve it (it won't be a cycle)
        }
      }
    }

    return std::make_pair(remove_edges, remove_nodes);
};


GraphType
compare_low_and_high_info_graphs(
  const GraphType & g0,
  const GraphType & g1)
{
    auto edges_nodes_to_remove = remove_edges_and_nodes_from_high_info_graph(g0, g1);
    const auto & remove_edges = edges_nodes_to_remove.first;
    const auto & remove_nodes = edges_nodes_to_remove.second;
    return filter_by_sets(remove_edges, remove_nodes, g1);
}

} // ns SG
