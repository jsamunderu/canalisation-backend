#include <json11.hpp>
#include <boost/log/trivial.hpp>
#include "login_service.hpp"

struct Auth {
	Auth(const json11::Json& json) {
		if (!json["username"].is_null()) {
			username = json["username"].string_value();
		}
		if (!json["password"].is_null()) {
			password = json["password"].string_value();
		}
	}
	std::string username;
	std::string password;
};

void LoginService::handleLogin(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response)
{
	std::string err;
	auto json = json11::Json::parse(request.body(), err);
	if (!err.empty()) {
		BOOST_LOG_TRIVIAL(info) << "Failed: " << err;
		response.send(Pistache::Http::Code::Bad_Request, err);
	}

	Auth auth = {json};
	BOOST_LOG_TRIVIAL(info) << "Payload: " << auth.username << " " << auth.password;
	response.send(Pistache::Http::Code::Ok, json.dump());
	db.insertLogin("test", "test");
}

void LoginService::handleLoginOut(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response)
{
	response.send(Pistache::Http::Code::No_Content);
}
