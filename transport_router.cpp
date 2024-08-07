#include "transport_router.h"

namespace transport {

	const graph::DirectedWeightedGraph<double>& transport::Router::BuildGraph(const catalogue::TransportCatalogue& catalogue) {
		const auto& all_stops = catalogue.GetAllStops();
		const auto& all_buses = catalogue.GetAllBuses();

		graph::DirectedWeightedGraph<double> stops_graph(all_stops.size());
		double bus_speed = settings_.bus_velocity_ * (100.0 / 6.0);

		{
			graph::VertexId vertex_id = 0;

			for (const auto& [stop_name, stop] : all_stops) {
				stop_ids_reverse_[vertex_id] = stop->name;
				stop_ids_[stop->name] = vertex_id++;
			}
		}

		for (const auto& [bus_name, bus] : all_buses) {
			const std::vector<catalogue::Stop*>& stops = bus->stop_names;
			for (size_t from_id = 0; from_id < stops.size(); ++from_id) {
				for (size_t to_id = from_id + 1; to_id < stops.size(); ++to_id) {

					const catalogue::Stop* stop_from = stops[from_id];
					const catalogue::Stop* stop_to = stops[to_id];

					double distance = 0;
					double distance_reverse = 0;

					for (size_t from_to_count = from_id; from_to_count < to_id; ++from_to_count) {
						distance += catalogue.GetDistanceBetweenStops(stops[from_to_count], stops[from_to_count + 1]);
						distance_reverse += catalogue.GetDistanceBetweenStops(stops[from_to_count + 1], stops[from_to_count]);
					}

					stops_graph.AddEdge({
						/*from*/	stop_ids_.at(stop_from->name),
						/*to*/		stop_ids_.at(stop_to->name),
						/*distance*/static_cast<double>(distance) / bus_speed + static_cast<double>(settings_.bus_wait_time_),
						/*name*/	bus->number,
						/*stops_count*/	to_id - from_id
						});


					if (!bus->is_roundtrip) {
						stops_graph.AddEdge({
							/*from*/	stop_ids_.at(stop_to->name),
							/*to*/		stop_ids_.at(stop_from->name),
							/*distance*/static_cast<double>(distance_reverse) / bus_speed + static_cast<double>(settings_.bus_wait_time_),
							/*name*/	bus->number,
							/*stops_count*/	to_id - from_id
							});
					}
				}
			}
		}

		graph_ = std::move(stops_graph);
		router_ = std::make_unique<graph::Router<double>>(graph_);

		return graph_;
	}

	const Route Router::FindRoute(const std::string_view stop_from, const std::string_view stop_to) const {
		Route result;
		const auto& router = router_->BuildRoute(stop_ids_.at(std::string(stop_from)), stop_ids_.at(std::string(stop_to)));
		if (!router) return result;
		for (const graph::EdgeId& id : router.value().edges) {
			const graph::Edge<double> edge = graph_.GetEdge(id);

			RouteElement element_stop({
				stop_ids_reverse_.at(edge.from),
				static_cast<double>(settings_.bus_wait_time_),
				0
				});
			result.emplace_back(element_stop);

			RouteElement element_bus({
				std::string(edge.name),
				edge.weight - settings_.bus_wait_time_,
				static_cast<int>(edge.stops_count)
				});
			result.emplace_back(element_bus);
		}

		return result;
	}

	const graph::DirectedWeightedGraph<double>& Router::GetGraph() const {
		return graph_;
	}
}