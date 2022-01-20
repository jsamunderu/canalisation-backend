#ifndef LOGIN_SERVICE_HPP
#define LOGIN_SERVICE_HPP
#include <pistache/endpoint.h>
#include <pistache/http.h>                                                      
#include <pistache/router.h>
#include <iostream>
#include "db_access.hpp"

class LoginService {
public:
	explicit LoginService(Pistache::Address addr, std::size_t threads_no,
		const MariadbAccess::Config& config)
		: httpEndpoint(std::make_unique<Pistache::Http::Endpoint>(addr)),
			threads_no(threads_no),
			db(config)
	{
		auto opts = Pistache::Http::Endpoint::options().threads(static_cast<int>(threads_no));
		httpEndpoint->init(opts);
		setupRoutes();
	}
	
	void start()
	{
		httpEndpoint->setHandler(router.handler());
		httpEndpoint->serve();
	}
private:
	std::size_t threads_no;
	std::unique_ptr<Pistache::Http::Endpoint> httpEndpoint;
	Pistache::Rest::Router router;
	LoginServiceDBAccess db;

	void setupRoutes()
	{
		Pistache::Rest::Routes::Post(router, "/login", Pistache::Rest::Routes::bind(&LoginService::handleLogin, this));
		Pistache::Rest::Routes::Post(router, "/logout", Pistache::Rest::Routes::bind(&LoginService::handleLoginOut, this));
	}

	void handleLogin(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response);
	void handleLoginOut(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response);
};
#endif
