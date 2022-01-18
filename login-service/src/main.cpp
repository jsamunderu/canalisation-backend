#include <cstdlib>
#include <boost/log/trivial.hpp>
#include <yaml-cpp/depthguard.h>
#include <yaml-cpp/yaml.h>
#include <iostream>
#include <exception>
#include "login_service.hpp"

struct DBConfig {
	unsigned int port;
	std::string host;
	std::string db_name;
	std::string username;
	std::string password;
};

void set_config_defaults(YAML::Node& config)
{
	if (!config["port"]) {
		config["port"] = "8080";
	}
	if (!config["log-level"]) {
		config["log-level"] = "INFO";
	}
}

DBConfig get_db_config(const YAML::Node& dbConfig)
{
	DBConfig db_config;
	if (dbConfig["port"]) {
		db_config.port = dbConfig["port"].as<unsigned int>();
	}
	if (dbConfig["host"]) {
		db_config.host.assign(dbConfig["host"].as<std::string>());
	}
	if (dbConfig["db"]) {
		db_config.db_name.assign(dbConfig["db"].as<std::string>());
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

	set_config_defaults(config);

	BOOST_LOG_TRIVIAL(info) << "############# CONFIG ##############";
	BOOST_LOG_TRIVIAL(info) << "port: " << config["port"].as<std::string>();
	BOOST_LOG_TRIVIAL(info) << "log-level: " << config["log-level"].as<std::string>();

	YAML::Node db_config_yaml = config["database"];
	if (!db_config_yaml) {
		BOOST_LOG_TRIVIAL(error) << "Error! Database configuration not specified.";
		return EXIT_FAILURE;
	}
	DBConfig db_config = get_db_config(db_config_yaml);

	BOOST_LOG_TRIVIAL(info) << "\tdatabase: ";
	BOOST_LOG_TRIVIAL(info) << "\t\thost: " << db_config.host;
	BOOST_LOG_TRIVIAL(info) << "\t\tport: " << db_config.port;
	BOOST_LOG_TRIVIAL(info) << "\t\tdb: " << db_config.db_name;
	BOOST_LOG_TRIVIAL(info) << "\t\tusername: " << db_config.username;
	BOOST_LOG_TRIVIAL(info) << "\t\tpassword: " << db_config.password;

	BOOST_LOG_TRIVIAL(info) << "############# CONFIG ##############";

	BOOST_LOG_TRIVIAL(info) << "Login-service starting...";
	Pistache::Address addr(Pistache::Ipv4::any(), Pistache::Port(config["port"].as<std::string>()));

	std::size_t threads_no = 4;
	try {
        	LoginService service(addr,
			threads_no,
			db_config.port,
			db_config.host,
			db_config.db_name,
			db_config.username,
			db_config.password
		);
		service.start();
	} catch(std::exception& e) {
		BOOST_LOG_TRIVIAL(error) << "int main() caught exception: " << e.what() << std::endl;
	}

	BOOST_LOG_TRIVIAL(info) << "Login-service stopped.";
	return EXIT_SUCCESS;
}