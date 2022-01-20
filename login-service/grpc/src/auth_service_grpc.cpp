#include <iostream>
#include <memory>
#include <string>
#include <sstream>
#include <boost/log/trivial.hpp>
#include <grpcpp/ext/proto_server_reflection_plugin.h>
#include <grpcpp/grpcpp.h>
#include <grpcpp/health_check_service_interface.h>
#include "login.grpc.pb.h"
#include "auth_service_grpc.hpp"

constexpr int SessionLifetime = 10000;

class AuthServiceImpl final : public grpc::auth_service::AuthService::Service {
public:
	AuthServiceImpl(const std::string& host, unsigned int port,
		const MariadbAccess::Config& config)
	: port(port), host(host), db(config) {}
	void run_grpc_server();
	void stop_grpc_server();
	grpc::Status IsAuthorised(grpc::ServerContext* context,
			const grpc::auth_service::AuthRequest* request,
			grpc::auth_service::AuthResponse* response) override;

private:
	unsigned int port;
	std::string host;
	LoginServiceDBAccess db;
	std::unique_ptr<grpc::Server> server;
};

grpc::Status AuthServiceImpl::IsAuthorised(grpc::ServerContext* context,
		const grpc::auth_service::AuthRequest* request,
		grpc::auth_service::AuthResponse* response) {

	auto token = request->token();
	auto loginResult = db.fetchLogin(token);
	auto [status, data] = loginResult.next();
	if (!status) {
		return grpc::Status(grpc::StatusCode::UNAUTHENTICATED, "Access Denied!");
	}

	auto now = std::time(nullptr);
	auto diff = std::difftime(now, data.login_activity_ts);
	if (diff < SessionLifetime) {
		response->set_status(false);
	}

	response->set_status(true);

	return grpc::Status::OK;
}

void AuthServiceImpl::run_grpc_server() {
	std::stringstream ss;
	ss << host << ":" << port;

	BOOST_LOG_TRIVIAL(info) << "grpc addr: " << ss.str();
	std::string server_address(ss.str());

	grpc::EnableDefaultHealthCheckService(true);
	grpc::reflection::InitProtoReflectionServerBuilderPlugin();

	grpc::ServerBuilder builder;

	builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());

	builder.RegisterService(this);

	server = builder.BuildAndStart();
	BOOST_LOG_TRIVIAL(info) << "Server listening on " << server_address;

	server->Wait();
	std::cout << "################# why" << std::endl;
}

void AuthServiceImpl::stop_grpc_server()
{
	BOOST_LOG_TRIVIAL(info) << "Server stoping.";
	server->Shutdown();
	BOOST_LOG_TRIVIAL(info) << "Server stopped.";
}


void AuthService::run_grpc_server()
{
	impl->run_grpc_server();
}
void AuthService::stop_grpc_server()
{
	impl->stop_grpc_server();
}

AuthService::AuthService(const std::string& host, unsigned int port,
		const MariadbAccess::Config& config)
	: impl(std::make_unique<AuthServiceImpl>(host, port, config))
{
}


AuthService::AuthService() = default;
AuthService::AuthService(AuthService&&) = default;
AuthService::AuthService(AuthService const &other)
{
	impl = std::move(const_cast<AuthService&>(other).impl);
}
AuthService& AuthService::operator=(AuthService const &other)
{
	impl = std::move(const_cast<AuthService&>(other).impl);
	return *this;
}
AuthService& AuthService::operator=(AuthService&& other)
{
	impl = std::move(const_cast<AuthService&>(other).impl);
	return *this;
}

AuthService::~AuthService() = default;
