#include <cstdlib>
#include <boost/log/trivial.hpp>
#include <yaml-cpp/depthguard.h>
#include <yaml-cpp/yaml.h>
#include <thread>
#include <iostream>
#include <exception>
#include "login_service.hpp"
#include "auth_service_grpc.hpp"

void set_config_defaults(YAML::Node& config)
{
	if (!config["port"]) {
		config["port"] = "8080";
	}
	if (!config["log-level"]) {
		config["log-level"] = "INFO";
	}
}

const MariadbAccess::Config get_db_config(const YAML::Node& dbConfig)
{
	MariadbAccess::Config db_config;
	if (dbConfig["port"]) {
		db_config.port = dbConfig["port"].as<unsigned int>();
	}
	if (dbConfig["host"]) {
		db_config.host.assign(dbConfig["host"].as<std::string>());
	}
	if (dbConfig["db"]) {
		db_config.database.assign(dbConfig["db"].as<std::string>());
	}
	if (dbConfig["username"]) {
		db_config.username.assign(dbConfig["username"].as<std::string>());
	}
	if (dbConfig["password"]) {
		db_config.password.assign(dbConfig["password"].as<std::string>());
	}
	return db_config;
}

int main(int argc, char *argv[]) {
	YAML::Node config;
	for (int i = 0; i < argc; i++) {
		BOOST_LOG_TRIVIAL(info) << argv[i];
	}
	if (argc > 2 && std::string("-f") == std::string(argv[1])) {
		BOOST_LOG_TRIVIAL(info) << "Loading config file: " << std::string(argv[2]);
		config = YAML::LoadFile(std::string(argv[2]));
	}
	YAML::Node rest_config_yaml = config["rest"];
	if (!rest_config_yaml) {
		BOOST_LOG_TRIVIAL(error) << "Error! REST configuration not specified.";
		return EXIT_FAILURE;
	}
	set_config_defaults(rest_config_yaml);

	YAML::Node grpc_config_yaml = config["grpc"];
	if (!rest_config_yaml) {
		BOOST_LOG_TRIVIAL(error) << "Error! gRPC configuration not specified.";
		return EXIT_FAILURE;
	}

	BOOST_LOG_TRIVIAL(info) << "############# CONFIG ##############";
	BOOST_LOG_TRIVIAL(info) << "REST port: " << rest_config_yaml["port"].as<std::string>();
	BOOST_LOG_TRIVIAL(info) << "gRPC port: " << grpc_config_yaml["port"].as<std::string>();
	BOOST_LOG_TRIVIAL(info) << "log-level: " << config["log-level"].as<std::string>();

	YAML::Node db_config_yaml = config["database"];
	if (!db_config_yaml) {
		BOOST_LOG_TRIVIAL(error) << "Error! Database configuration not specified.";
		return EXIT_FAILURE;
	}
	MariadbAccess::Config db_config = get_db_config(db_config_yaml);

	BOOST_LOG_TRIVIAL(info) << "\tdatabase: ";
	BOOST_LOG_TRIVIAL(info) << "\t\thost: " << db_config.host;
	BOOST_LOG_TRIVIAL(info) << "\t\tport: " << db_config.port;
	BOOST_LOG_TRIVIAL(info) << "\t\tdb: " << db_config.database;
	BOOST_LOG_TRIVIAL(info) << "\t\tusername: " << db_config.username;
	BOOST_LOG_TRIVIAL(info) << "\t\tpassword: " << db_config.password;

	BOOST_LOG_TRIVIAL(info) << "############# CONFIG ##############";

	BOOST_LOG_TRIVIAL(info) << "Login-service starting...";

	try {
		AuthService grpc_service(grpc_config_yaml["host"].as<std::string>(),
		grpc_config_yaml["port"].as<unsigned int>(), db_config);

		std::thread authServer([&grpc_service]{ grpc_service.run_grpc_server(); });


		Pistache::Address addr(Pistache::Ipv4::any(), Pistache::Port(rest_config_yaml["port"].as<std::string>()));

		std::size_t threads_no = 4;
        	LoginService rest_service(addr,
			threads_no,
			db_config
		);
		rest_service.start();

		gsl::finally([&grpc_service, &authServer] {	
			grpc_service.stop_grpc_server();
			authServer.join();
		});
	} catch(std::exception& e) {
		BOOST_LOG_TRIVIAL(error) << "int main() caught exception: " << e.what() << std::endl;
	}

	BOOST_LOG_TRIVIAL(info) << "Login-service stopped.";
	return EXIT_SUCCESS;
}