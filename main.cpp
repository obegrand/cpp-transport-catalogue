#include "request_handler.h"
#include "json_reader.h"
#include "map_renderer.h"
#include "svg.h"
#include "log_duration.h"

int main() {
	LOG_DURATION("total time");
	catalogue::TransportCatalogue catalogue;
	JsonReader reader(std::cin, catalogue);
	reader.FillCatalogue();
	transport::Router router(
		/*settings*/ reader.FillRoutingSettings(
						reader.GetRoutingSettings().AsMap()
					 ),
		/*catalogue*/ catalogue);
	RequestHandler(reader, catalogue, router);
}