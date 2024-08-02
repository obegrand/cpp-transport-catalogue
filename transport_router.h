#pragma once
#include "router.h"
#include "transport_catalogue.h"
#include <memory>

namespace transport {

struct RouteElement {
	std::string name;
	double time = 0.;
	int stop_count = 0;
};

using Route = std::vector<RouteElement>;

struct RoutingSettings {
	int bus_wait_time_ = 0;
	double bus_velocity_ = 0.0;
};

class Router{
public:
	Router(RoutingSettings settings, const catalogue::TransportCatalogue& catalogue)
		:settings_(settings) {
		BuildGraph(catalogue);
	}

	const Route FindRoute(const std::string_view stop_from, const std::string_view stop_to) const;
	const graph::DirectedWeightedGraph<double>& GetGraph() const;
	const std::string& GetStopName(const graph::VertexId stop_name) const;
	int GetWaitTime() const;

private:
	RoutingSettings settings_;

	std::unordered_map<std::string, graph::VertexId> stop_ids_;
	std::unordered_map<graph::VertexId, std::string> stop_ids_reverse_;
	graph::DirectedWeightedGraph<double> graph_;
	std::unique_ptr<graph::Router<double>> router_;

	const graph::DirectedWeightedGraph<double>& BuildGraph(const catalogue::TransportCatalogue& catalogue);
};
}