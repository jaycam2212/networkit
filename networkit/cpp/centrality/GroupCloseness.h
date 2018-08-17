/*
 * GroupCloseness.h
 *
 *  Created on: 03.10.2016
 *      Author: elisabetta bergamini
 */

#ifndef GROUPCLOSENESS_H_
#define GROUPCLOSENESS_H_

#include "../base/Algorithm.h"
#include "../graph/Graph.h"

namespace NetworKit {

/**
 * @ingroup centrality
 */
class GroupCloseness : public Algorithm {
public:
	/**
	 * Finds the group of nodes with highest (group) closeness centrality.
	 * The algorithm is the one proposed in Bergamini et al., ALENEX 2018 and
	 * finds a solution that is a (1-1/e)-approximation of the optimum.
	 * The worst-case running time of this approach is quadratic, but usually
	 * much faster in practice.
	 *
	 * @param G An unweighted graph.
	 * @param k Size of the group of nodes
	 * @param H If equal 0, simply runs the algorithm proposed in Bergamini et
	 * al.. If > 0, interrupts all BFSs after H iterations (suggested for very
	 * large networks).
	 * @
	 */
	GroupCloseness(const Graph &G, count k = 1, count H = 0);

	/**
	 * Computes the group with maximum closeness on the graph passed in the
	 * constructor.
	 */
	void run();

	/**
	 * Returns group with maximum closeness.
	 */
	std::vector<node> groupMaxCloseness();

	/**
	 * Computes farness (i.e., inverse of the closeness) for a given group
	 * (stopping after H iterations if H > 0).
	 */
	double computeFarness(std::vector<node> S,
	                      count H = std::numeric_limits<count>::max());

	/**
	 * Computes the score of a specific group.
	 */
	double scoreOfGroup(const std::vector<node> &group) const;

protected:
	edgeweight computeImprovement(node u, count n, Graph &G, count h);
	std::vector<count> newDistances(node u, count n, Graph &G, count h);
	Graph G;
	count k = 1;
	std::vector<count> D;
	count iters;
	count maxD;
	std::vector<count> d;
	std::vector<count> d1;
	std::vector<node> S;
	count H = 0;

	void checkGroup(const std::vector<node> &group) const;
};

inline std::vector<node> GroupCloseness::groupMaxCloseness() {
	if (!hasRun)
		throw std::runtime_error("Call run method first");
	return S;
}

inline void GroupCloseness::checkGroup(const std::vector<node> &group) const {
	std::vector<node> sortedV(group);
	std::sort(sortedV.begin(), sortedV.end());
	node u;
	auto checkNode = [&](node u) {
		if (!G.hasNode(u)) {
			std::stringstream err;
			err << "Error: node" << u << " is not in the graph.";
			throw std::runtime_error(err.str());
		}
	};
	for (count i = 0; i < sortedV.size() - 1; ++i) {
		u = sortedV[i];
		checkNode(u);
		if (u == sortedV[i + 1]) {
			throw std::runtime_error("Error: the set contains duplicate elements.");
		}
	}
	checkNode(sortedV.back());
}

inline double
GroupCloseness::scoreOfGroup(const std::vector<node> &group) const {

	std::vector<bool> explored(G.upperNodeIdBound(), false);
	std::vector<count> distance(G.upperNodeIdBound(), 0);

	for (count i = 0; i < group.size(); ++i) {
		explored[group[i]] = true;
	}

	std::vector<node> queue;
	auto exploreNode = [&](node w, count d) {
		explored[w] = true;
		queue.push_back(w);
		distance[w] = d;
	};

	count d = 1;
	for (auto u : group) {
		G.forNeighborsOf(u, [&](node v) {
			if (!explored[v]) {
				exploreNode(v, d);
			}
		});
	}

	while (queue.size() > 0) {
		++d;
		node u = queue.front();
		queue.erase(queue.begin());
		G.forNeighborsOf(u, [&](node v) {
			if (!explored[v]) {
				exploreNode(v, d);
			}
		});
	}

	double dSum = std::accumulate(distance.begin(), distance.end(), 0);
	return dSum == 0
	           ? 0.
	           : ((double)G.upperNodeIdBound() - (double)group.size()) / dSum;
}
} /* namespace NetworKit */
#endif /* GROUPCLOSENESS_H_ */
