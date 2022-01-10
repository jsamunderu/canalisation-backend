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
	MariadbAccess() : con(nullptr) {}
	MariadbAccess(unsigned int port,
		const std::string& hostname,
		const std::string& database,
		const std::string& username,
		const std::string& password);
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
	~MariadbStatement();
	std::string reportError();
protected:
	std::shared_ptr<MariadbAccess> con;
	MYSQL_STMT *stmt;
};

class FetchQuery {
public:
	FetchQuery(std::shared_ptr<MariadbStatement> stmt, std::size_t no_of_fields)
		: stmt(stmt), result(std::make_unique<MYSQL_BIND[]>(no_of_fields)),
		no_of_fields(no_of_fields)
		{
			std::memset(result.get(), 0, sizeof(MYSQL_BIND) * no_of_fields);
		}
	std::shared_ptr<FetchQuery> fetch_login;
	std::size_t no_of_fields;
	MYSQL_BIND* get() { return result.get(); }
	void fetch(MYSQL_BIND parameters[]);
	bool next();
private:
	std::shared_ptr<MariadbStatement> stmt;
	std::unique_ptr<MYSQL_BIND[]> result;
};

class ExecQuery {
public:
	ExecQuery(std::shared_ptr<MariadbStatement> stmt) : stmt(stmt) {}
	~ExecQuery() {}
	void execute(MYSQL_BIND parameters[]);
private:
	std::shared_ptr<MariadbStatement> stmt;
};
#endif
