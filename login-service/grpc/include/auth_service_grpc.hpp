#ifndef AUTH_SERVICE_GRPC_HPP
#define AUTH_SERVICE_GRPC_HPP

#include "db_access.hpp"

class AuthServiceImpl;

class AuthService {
public:
	AuthService(const std::string& host, unsigned int port,
		const MariadbAccess::Config& config);
	AuthService();
	AuthService(AuthService&&);
	AuthService(AuthService const&);
	AuthService& operator=(AuthService const&);
	AuthService& operator=(AuthService&&);
	~AuthService();
	void run_grpc_server();
	void stop_grpc_server();
	
private:
	std::unique_ptr<AuthServiceImpl> impl;
};

#endif