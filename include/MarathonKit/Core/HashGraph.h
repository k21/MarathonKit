/*
 * This file is part of MarathonKit.
 * Copyright (C) 2016 Jakub Zika
 *
 * MarathonKit is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 * I am providing code in this repository to you under an open source license.
 * Because this is my personal repository, the license you receive to my code is
 * from me and not from my employer (Facebook).
 */

#ifndef MARATHON_KIT_CORE_HASH_GRAPH_H_
#define MARATHON_KIT_CORE_HASH_GRAPH_H_

#include <cassert>
#include <unordered_map>
#include <unordered_set>

namespace MarathonKit {
namespace Core {

template <
  typename Node,
  typename NodeHash = std::hash<Node>,
  typename NodeEqual = std::equal_to<Node>>
class HashGraph {
public:

  using NodeSet = std::unordered_set<Node, NodeHash, NodeEqual>;

  template <typename Value>
  using NodeMap = std::unordered_map<Node, Value, NodeHash, NodeEqual>;

  HashGraph(
      const NodeHash& nodeHash = NodeHash(),
      const NodeEqual& nodeEqual = NodeEqual()):
    nodes({}, nodeHash, nodeEqual),
    edges({}, nodeHash, nodeEqual),
    reverseEdges({}, nodeHash, nodeEqual) {}

  const NodeSet& getNodes() const {
    return nodes;
  }

  const NodeSet& getEdges(const Node& node) const {
    auto it = edges.find(node);
    assert(it != edges.end());
    return it->second;
  }

  const NodeSet& getReverseEdges(const Node& node) const {
    auto it = reverseEdges.find(node);
    assert(it != reverseEdges.end());
    return it->second;
  }

  bool hasNode(const Node& node) const {
    return nodes.count(node) > 0;
  }

  bool hasEdge(const Node& from, const Node& to) const {
    auto it = edges.find(from);
    if (it == edges.end()) {
      return false;
    }
    return it->second.count(to);
  }

  bool addNode(const Node& node) {
    bool ans = nodes.insert(node).second;
    if (ans) {
      ans = edges.emplace(node, newEmptyNodeSet()).second;
      assert(ans);
      ans = reverseEdges.emplace(node, newEmptyNodeSet()).second;
      assert(ans);
    }
    return ans;
  }

  bool removeNode(const Node& node) {
    if (!nodes.erase(node)) {
      return false;
    }

    auto it = edges.find(node);
    assert(it != edges.end());
    for (const Node& other : it->second) {
      auto otherIt = reverseEdges.find(other);
      assert(otherIt != reverseEdges.end());
      bool ans = otherIt->second.erase(node);
      assert(ans);
    }
    edges.erase(it);

    it = reverseEdges.find(node);
    assert(it != reverseEdges.end());
    for (const Node& other : it->second) {
      auto otherIt = edges.find(other);
      assert(otherIt != edges.end());
      bool ans = otherIt->second.erase(node);
      assert(ans);
    }
    reverseEdges.erase(it);

    return true;
  }

  bool addEdge(const Node& from, const Node& to) {
    assert(!nodes.key_eq()(from, to));

    auto edgeIt = edges.find(from);
    assert(edgeIt != edges.end());
    auto reverseEdgeIt = reverseEdges.find(to);
    assert(reverseEdgeIt != reverseEdges.end());

    bool ans1 = edgeIt->second.insert(to).second;
    bool ans2 = reverseEdgeIt->second.insert(from).second;
    assert(ans1 == ans2);
    return ans1;
  }

  bool removeEdge(const Node& from, const Node& to) {
    auto edgeIt = edges.find(from);
    assert(edgeIt != edges.end());
    auto reverseEdgeIt = reverseEdges.find(to);
    assert(reverseEdgeIt != reverseEdges.end());

    bool ans1 = edgeIt->second.erase(to);
    bool ans2 = reverseEdgeIt->second.erase(from);
    assert(ans1 == ans2);
    return ans1;
  }

  NodeSet newEmptyNodeSet() const {
    return NodeSet({}, nodes.hash_function(), nodes.key_eq());
  }

  template <typename Value>
  NodeMap<Value> newEmptyNodeMap() const {
    return NodeMap<Value>({}, nodes.hash_function(), nodes.key_eq());
  }

private:

  NodeSet nodes;
  NodeMap<NodeSet> edges;
  NodeMap<NodeSet> reverseEdges;

};

}}

#endif
