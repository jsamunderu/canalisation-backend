#ifndef LIB_MARIADB_ACCESS_HPP
#define LIB_MARIADB_ACCESS_HPP
#include <string>
#include <exception>
#include <sstream>
#include <memory>
#include <mysql.h>
#include <gsl/gsl>
enum class DatabaseAccessError { CONNECTION = -1, RUNTIME = -2, DATA_ERROR = -3, STATEMENT = -4 };
class Exception : public std::exception {
public:
	Exception(DatabaseAccessError code, const std::string& message) : code(code), message(message) {}
	virtual const char* what() const noexcept { return message.c_str(); }
	virtual ~Exception() {}
private:
	std::string message;
	DatabaseAccessError code;
};

class MariadbAccess {
public:

	struct Config {
		unsigned int port;
		std::string host;
		std::string database;
		std::string username;
		std::string password;
	};

	MariadbAccess() : con(nullptr) {}
	MariadbAccess(const Config&);
	MYSQL* get() { return con; }
	~MariadbAccess();
private:
	MYSQL* con;
};

class MariadbStatement {
public:
	MariadbStatement() : stmt(nullptr) {}
	MariadbStatement(std::shared_ptr<MariadbAccess> con, const std::string& stmt_str);
	MYSQL_STMT* get() { return stmt; }
	MariadbAccess& get_con() { return *con.get(); }

	~MariadbStatement();
	std::string reportError();
protected:
	std::shared_ptr<MariadbAccess> con;
	MYSQL_STMT *stmt;
};

class ExecQuery {
public:
	ExecQuery(std::shared_ptr<MariadbStatement> stmt) : stmt(stmt) {}
	~ExecQuery() {}
	MariadbStatement& get() { return *stmt.get(); }
	unsigned long long execute(MYSQL_BIND parameters[]);
private:
	std::shared_ptr<MariadbStatement> stmt;
};

class FetchQuery {
public:
	FetchQuery(std::shared_ptr<MariadbStatement> stmt) : stmt(stmt) {}
	~FetchQuery() {}
	MariadbStatement& get() { return *stmt.get(); }
	unsigned long long fetch(MYSQL_BIND parameters[]);
	bool next();
private:
	std::shared_ptr<MariadbStatement> stmt;
};
#endif
