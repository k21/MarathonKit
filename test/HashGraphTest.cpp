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

#include <string>
#include <utility>

#include <gmock/gmock.h>

#include "Core/HashGraph.h"

using MarathonKit::Core::HashGraph;
using std::make_pair;
using std::unordered_set;

TEST(HashGraphTest, canAddAndRemoveNodes) {
  HashGraph<int> graph;

  EXPECT_FALSE(graph.hasNode(1));
  EXPECT_FALSE(graph.hasNode(2));

  EXPECT_TRUE(graph.addNode(2));

  EXPECT_FALSE(graph.hasNode(1));
  EXPECT_TRUE(graph.hasNode(2));

  EXPECT_TRUE(graph.addNode(1));

  EXPECT_TRUE(graph.hasNode(1));
  EXPECT_TRUE(graph.hasNode(2));

  EXPECT_FALSE(graph.addNode(1));

  EXPECT_TRUE(graph.hasNode(1));
  EXPECT_TRUE(graph.hasNode(2));

  EXPECT_TRUE(graph.removeNode(2));

  EXPECT_TRUE(graph.hasNode(1));
  EXPECT_FALSE(graph.hasNode(2));

  EXPECT_FALSE(graph.removeNode(2));

  EXPECT_TRUE(graph.hasNode(1));
  EXPECT_FALSE(graph.hasNode(2));
}

TEST(HashGraphTest, canAddAndRemoveEdges) {
  HashGraph<int> graph;

  EXPECT_FALSE(graph.hasEdge(1, 2));
  EXPECT_FALSE(graph.hasEdge(1, 3));

  graph.addNode(1);
  graph.addNode(2);
  graph.addNode(3);

  EXPECT_FALSE(graph.hasEdge(1, 2));
  EXPECT_FALSE(graph.hasEdge(1, 3));

  EXPECT_TRUE(graph.addEdge(1, 2));

  EXPECT_TRUE(graph.hasEdge(1, 2));
  EXPECT_FALSE(graph.hasEdge(1, 3));

  EXPECT_FALSE(graph.addEdge(1, 2));

  EXPECT_TRUE(graph.hasEdge(1, 2));
  EXPECT_FALSE(graph.hasEdge(1, 3));

  EXPECT_TRUE(graph.addEdge(1, 3));

  EXPECT_TRUE(graph.hasEdge(1, 2));
  EXPECT_TRUE(graph.hasEdge(1, 3));

  EXPECT_TRUE(graph.removeEdge(1, 2));

  EXPECT_FALSE(graph.hasEdge(1, 2));
  EXPECT_TRUE(graph.hasEdge(1, 3));

  EXPECT_FALSE(graph.removeEdge(1, 2));

  EXPECT_FALSE(graph.hasEdge(1, 2));
  EXPECT_TRUE(graph.hasEdge(1, 3));
}

TEST(HashGraphTest, isDirected) {
  HashGraph<int> graph;

  graph.addNode(1);
  graph.addNode(2);
  graph.addNode(3);

  EXPECT_TRUE(graph.addEdge(1, 2));
  EXPECT_TRUE(graph.addEdge(2, 3));

  EXPECT_TRUE(graph.hasEdge(1, 2));
  EXPECT_TRUE(graph.hasEdge(2, 3));
  EXPECT_FALSE(graph.hasEdge(2, 1));
  EXPECT_FALSE(graph.hasEdge(3, 2));

  EXPECT_TRUE(graph.addEdge(2, 1));
  EXPECT_TRUE(graph.addEdge(3, 2));

  EXPECT_TRUE(graph.hasEdge(1, 2));
  EXPECT_TRUE(graph.hasEdge(2, 3));
  EXPECT_TRUE(graph.hasEdge(2, 1));
  EXPECT_TRUE(graph.hasEdge(3, 2));
}

TEST(HashGraphTest, canGetNodesAndEdges) {
  HashGraph<int> graph;

  graph.addNode(2);
  graph.addNode(4);
  graph.addNode(7);
  graph.addEdge(2, 4);
  graph.addEdge(2, 7);
  graph.addEdge(4, 7);

  EXPECT_EQ(unordered_set<int>({2, 4, 7}), graph.getNodes());
  EXPECT_EQ(unordered_set<int>({4, 7}), graph.getEdges(2));
  EXPECT_EQ(unordered_set<int>({7}), graph.getEdges(4));
  EXPECT_EQ(unordered_set<int>({}), graph.getEdges(7));
  EXPECT_EQ(unordered_set<int>({}), graph.getReverseEdges(2));
  EXPECT_EQ(unordered_set<int>({2}), graph.getReverseEdges(4));
  EXPECT_EQ(unordered_set<int>({2, 4}), graph.getReverseEdges(7));
}

TEST(HashGraphTest, removingNodeRemovesItsEdges) {
  HashGraph<int> graph;

  graph.addNode(1);
  graph.addNode(2);
  graph.addNode(3);
  graph.addEdge(1, 2);
  graph.addEdge(2, 3);

  EXPECT_TRUE(graph.hasEdge(1, 2));
  EXPECT_TRUE(graph.hasEdge(2, 3));

  EXPECT_TRUE(graph.removeNode(2));

  EXPECT_FALSE(graph.hasEdge(1, 2));
  EXPECT_FALSE(graph.hasEdge(2, 3));

  EXPECT_TRUE(graph.addNode(2));

  EXPECT_FALSE(graph.hasEdge(1, 2));
  EXPECT_FALSE(graph.hasEdge(2, 3));
}

template <typename Set, typename Element>
void testSet(Set s, Element e1, Element e2) {
  EXPECT_EQ(0, s.size());
  EXPECT_TRUE(s.insert(e1).second);
  EXPECT_EQ(1, s.size());
  EXPECT_FALSE(s.insert(e1).second);
  EXPECT_EQ(1, s.size());
  EXPECT_TRUE(s.insert(e2).second);
  EXPECT_EQ(2, s.size());
  EXPECT_FALSE(s.insert(e2).second);
  EXPECT_EQ(2, s.size());
  EXPECT_TRUE(s.erase(e1));
  EXPECT_EQ(1, s.size());
  EXPECT_FALSE(s.erase(e1));
  EXPECT_EQ(1, s.size());
  EXPECT_TRUE(s.erase(e2));
  EXPECT_EQ(0, s.size());
  EXPECT_FALSE(s.erase(e2));
  EXPECT_EQ(0, s.size());
}

template <typename Map, typename Key, typename Value>
void testMap(Map m, Key k1, Value v1, Key k2, Value v2) {
  EXPECT_EQ(0, m.size());
  m[k1] = v2;
  EXPECT_EQ(v2, m[k1]);
  EXPECT_EQ(1, m.size());
  m[k1] = v1;
  EXPECT_EQ(v1, m[k1]);
  EXPECT_EQ(1, m.size());
  m[k2] = v1;
  EXPECT_EQ(v1, m[k2]);
  EXPECT_EQ(2, m.size());
  m[k2] = v2;
  EXPECT_EQ(v2, m[k2]);
  EXPECT_EQ(2, m.size());
  EXPECT_TRUE(m.erase(k1));
  EXPECT_EQ(1, m.size());
  EXPECT_FALSE(m.erase(k1));
  EXPECT_EQ(1, m.size());
  EXPECT_TRUE(m.erase(k2));
  EXPECT_EQ(0, m.size());
  EXPECT_FALSE(m.erase(k2));
  EXPECT_EQ(0, m.size());
}

TEST(HashGraphTest, createNodeSetsAndMaps) {
  HashGraph<int> graph;

  auto s = graph.newEmptyNodeSet();
  auto m = graph.newEmptyNodeMap<std::string>();
  testSet(s, 4, 7);
  testMap(m, 4, "four", 7, "seven");

  auto es = graph.newEmptyEdgeSet();
  auto em = graph.newEmptyEdgeMap<std::string>();
  testSet(es, make_pair(4, 5), make_pair(6, 7));
  testMap(em, make_pair(4, 5) , "four", make_pair(6, 7), "seven");
}

TEST(HashGraphTest, canUseCustomEquality) {
  struct Hash {
    int mod;
    Hash(int mod_): mod(mod_) {}
    size_t operator () (int i) const {
      return std::hash<int>()(i % mod) + 42;
    }
  };
  struct Eq {
    int mod;
    Eq(int mod_): mod(mod_) {}
    bool operator () (int i1, int i2) const {
      return i1 % mod == i2 % mod;
    }
  };

  HashGraph<int, Hash, Eq> graph(Hash(7), Eq(7));

  EXPECT_FALSE(graph.hasNode(2));
  EXPECT_FALSE(graph.hasNode(9));
  EXPECT_FALSE(graph.hasNode(4));
  EXPECT_FALSE(graph.hasNode(11));

  EXPECT_TRUE(graph.addNode(2));

  EXPECT_TRUE(graph.hasNode(2));
  EXPECT_TRUE(graph.hasNode(9));
  EXPECT_FALSE(graph.hasNode(4));
  EXPECT_FALSE(graph.hasNode(11));

  EXPECT_FALSE(graph.addNode(9));

  EXPECT_TRUE(graph.hasNode(2));
  EXPECT_TRUE(graph.hasNode(9));
  EXPECT_FALSE(graph.hasNode(4));
  EXPECT_FALSE(graph.hasNode(11));

  EXPECT_TRUE(graph.addNode(11));

  EXPECT_TRUE(graph.hasNode(2));
  EXPECT_TRUE(graph.hasNode(9));
  EXPECT_TRUE(graph.hasNode(4));
  EXPECT_TRUE(graph.hasNode(11));

  EXPECT_FALSE(graph.hasEdge(11, 2));
  EXPECT_FALSE(graph.hasEdge(4, 9));

  EXPECT_TRUE(graph.addEdge(4, 9));

  EXPECT_TRUE(graph.hasEdge(11, 2));
  EXPECT_TRUE(graph.hasEdge(4, 9));

  EXPECT_FALSE(graph.addEdge(11, 2));

  EXPECT_TRUE(graph.removeNode(16));
  EXPECT_TRUE(graph.removeNode(18));

  EXPECT_EQ(0, graph.getNodes().size());

  auto s = graph.newEmptyNodeSet();
  auto m = graph.newEmptyNodeMap<std::string>();
  testSet(s, 4, 7);
  testMap(m, 4, "four", 7, "seven");

  auto es = graph.newEmptyEdgeSet();
  auto em = graph.newEmptyEdgeMap<std::string>();
  testSet(es, make_pair(4, 5), make_pair(6, 7));
  testMap(em, make_pair(4, 5) , "four", make_pair(6, 7), "seven");
}

TEST(HashGraphTest, canUseCustomObject) {
  struct Node {
    explicit Node(int id_): id(id_) {}
    int id;
    bool operator == (const Node& other) const {
      return id == other.id;
    }
  };
  struct Hash {
    int q;
    Hash(int q_): q(q_) {}
    size_t operator () (const Node& node) const {
      return std::hash<int>()(node.id + q);
    }
  };

  HashGraph<Node, Hash> graph(Hash(42));

  EXPECT_FALSE(graph.hasNode(Node(2)));
  EXPECT_FALSE(graph.hasNode(Node(4)));

  EXPECT_TRUE(graph.addNode(Node(2)));

  EXPECT_TRUE(graph.hasNode(Node(2)));
  EXPECT_FALSE(graph.hasNode(Node(4)));

  EXPECT_TRUE(graph.addNode(Node(4)));

  EXPECT_TRUE(graph.hasNode(Node(2)));
  EXPECT_TRUE(graph.hasNode(Node(4)));

  EXPECT_FALSE(graph.hasEdge(Node(2), Node(4)));
  EXPECT_FALSE(graph.hasEdge(Node(4), Node(2)));

  EXPECT_TRUE(graph.addEdge(Node(2), Node(4)));

  EXPECT_TRUE(graph.hasEdge(Node(2), Node(4)));
  EXPECT_FALSE(graph.hasEdge(Node(4), Node(2)));

  EXPECT_TRUE(graph.removeNode(Node(2)));
  EXPECT_TRUE(graph.removeNode(Node(4)));

  EXPECT_EQ(0, graph.getNodes().size());

  auto s = graph.newEmptyNodeSet();
  auto m = graph.newEmptyNodeMap<std::string>();
  testSet(s, Node(4), Node(7));
  testMap(m, Node(4), "four", Node(7), "seven");

  auto es = graph.newEmptyEdgeSet();
  auto em = graph.newEmptyEdgeMap<std::string>();
  testSet(es, make_pair(Node(4), Node(5)), make_pair(Node(6), Node(7)));
  testMap(
      em,
      make_pair(Node(4), Node(5)), "four",
      make_pair(Node(6), Node(7)), "seven");
}
