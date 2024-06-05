#pragma once
#include <string>
#include <set>
#include <unordered_map>
#include <vector>
#include <deque>

#include "geo.h"

namespace Catalogue {

struct Stop {
	std::string name;
	Geo::Coordinates coordinates;
	std::unordered_map<std::string, double> distances_to_other_stops;
};

struct Bus {
	std::string number;
	std::vector<Stop*> stop_names;
};

class TransportCatalogue {
public:
	void Add(Stop stop);
	void Add(std::string_view number, const std::vector<std::string_view>& stops);

	const Bus& GetBus(std::string_view number) const;
	const Stop& GetStop(std::string_view name) const;

	std::set<std::string_view> GetBusesByStop(std::string_view name)const;

	bool ContainsBus(std::string_view number)const;
	bool ContainsStop(std::string_view number)const;

	double ComputeStopsDistance(const std::vector<Stop*>& stop_names)const;
	double ComputeGeoDistance(const std::vector<Stop*>& stop_names) const;

	double GetDistanceBetweenStops(Stop* stop1, Stop* stop2)const;
	void SetDistanceBetweenStops(Stop* stop1, Stop* stop2, double distance)const;

private:

	std::deque<Stop> stops_storage_;
	std::unordered_map<std::string_view, Stop*> stops_;

	std::deque<Bus> buses_storage_;
	std::unordered_map<std::string_view, Bus*> buses_;

	std::unordered_map<std::string_view, std::set<std::string_view>> buses_through_stop_;
};

} // namespace TransportCatalogue