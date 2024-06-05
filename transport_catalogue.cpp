#include "transport_catalogue.h"
#include <string_view>

namespace Catalogue {

void TransportCatalogue::Add(Stop stop) {
	stops_storage_.push_back(std::move(stop));

	stops_[stops_storage_.back().name] = &stops_storage_.back();
	buses_through_stop_[stops_storage_.back().name];
}

void TransportCatalogue::Add(std::string_view number, const std::vector<std::string_view>& stops) {
	Bus newBus = { .number = std::string(number),.stop_names = {} };

	for (const auto& stop : stops) {
		auto it = stops_.find(stop);
		if (it != stops_.end()) {
			newBus.stop_names.push_back(it->second);
		}
	}

	buses_storage_.push_back(std::move(newBus));
	buses_[buses_storage_.back().number] = &buses_storage_.back();

	for (const auto& stop : stops) {
		buses_through_stop_[stop].insert(buses_storage_.back().number);
	}
}

const Bus& TransportCatalogue::GetBus(std::string_view number) const {
	return buses_.contains(number) ? *buses_.at(number) : throw std::runtime_error("Bus not found");
}

const Stop& TransportCatalogue::GetStop(std::string_view name) const {
	return stops_.contains(name) ? *stops_.at(name) : throw std::runtime_error("Stop not found");
}

std::set<std::string_view> TransportCatalogue::GetBusesByStop(const std::string_view name) const {
	return buses_through_stop_.at(name);
}

bool TransportCatalogue::ContainsBus(std::string_view number)const {
	return buses_.contains(number);
}

bool TransportCatalogue::ContainsStop(std::string_view name)const {
	return stops_.contains(name);
}

double Catalogue::TransportCatalogue::ComputeStopsDistance(const std::vector<Stop*>& stop_names) const {
	double result = 0.0;
	for (size_t i = 1; i < stop_names.size(); i++) {
		result += GetDistanceBetweenStops(stop_names.at(i - 1), stop_names.at(i));
	}
	return result;
}

double TransportCatalogue::GetDistanceBetweenStops(Stop* stop1, Stop* stop2) const {
	if (stop1->distances_to_other_stops.contains(stop2->name)) {
		return stop1->distances_to_other_stops.find(stop2->name)->second;
	}
	else if (stop2->distances_to_other_stops.contains(stop1->name))
	{
		return stop2->distances_to_other_stops.find(stop1->name)->second;
	}
	else
	{
		return 0; //���� ������ �� ������ ���� �� �������� ������
	}
}

void TransportCatalogue::SetDistanceBetweenStops(Stop* stop1, Stop* stop2, double distance) const {
	stop1->distances_to_other_stops[stop2->name] = distance;
}

double TransportCatalogue::ComputeGeoDistance(const std::vector<Stop*>& stop_names) const {
	double result = 0.0;

	for (size_t i = 1; i < stop_names.size(); i++) {
		Geo::Coordinates from = stop_names[i - 1]->coordinates;
		Geo::Coordinates to = stop_names[i]->coordinates;

		result += Geo::ComputeDistance(std::move(from), std::move(to));
	}
	return result;
}

} // namespace TransportCatalogue