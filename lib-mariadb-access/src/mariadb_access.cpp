#include <sstream>
#include <iostream>
#include "mariadb_access.hpp"

MariadbAccess::MariadbAccess(unsigned int port, const std::string& hostname,
	const std::string& database, const std::string& username, const std::string& password)
{
	if ((con = mysql_init(NULL)) == NULL) {
		std::stringstream ss;
		ss << "mysql_init()";

		throw Exception(DatabaseAccessError::CONNECTION, ss.str());
    	}

	if (mysql_real_connect(con, hostname.c_str(), username.c_str(), password.c_str(), database.c_str(), port, NULL, 0) == NULL) {
		std::stringstream ss;
		ss << mysql_error(con);

		throw Exception(DatabaseAccessError::CONNECTION, ss.str());
	}
}

MariadbAccess::~MariadbAccess()
{
	if (con) {
		mysql_close(con);
	}
}

MariadbStatement::MariadbStatement(std::shared_ptr<MariadbAccess> con, const std::string& stmt_str)
	: con(con)
{
	stmt = mysql_stmt_init(con->get());
	if (mysql_stmt_prepare(stmt, stmt_str.c_str(), -1)) {
		throw Exception(DatabaseAccessError::STATEMENT, reportError());
	}
}

MariadbStatement::~MariadbStatement()
{
	if (stmt) {
		mysql_stmt_close(stmt);
	}
}

std::string MariadbStatement::reportError()
{
	if (!stmt) {
		throw Exception(DatabaseAccessError::STATEMENT, "");
	}
	std::stringstream ss;
	ss << "Statement init error: errno[" << mysql_stmt_errno(stmt)
		<< "] state[" << mysql_stmt_sqlstate(stmt)
		<< "] error["<< mysql_stmt_error(stmt) << "]";
	return ss.str();
}

void FetchQuery::fetch(MYSQL_BIND parameters[])
{
	mysql_stmt_bind_param(stmt->get(), parameters);
	if (mysql_stmt_execute(stmt->get())) {
		throw Exception(DatabaseAccessError::STATEMENT, stmt->reportError());
	}
	if (mysql_stmt_bind_result(stmt->get(), result.get())) {
		throw Exception(DatabaseAccessError::STATEMENT, stmt->reportError());
	}
}

bool FetchQuery::next()
{
	return mysql_stmt_fetch(stmt->get()) == 0;
}

void ExecQuery::execute(MYSQL_BIND parameters[])
{
	mysql_stmt_bind_param(stmt->get(), parameters);
	if (mysql_stmt_execute(stmt->get())) {
		throw Exception(DatabaseAccessError::STATEMENT, stmt->reportError());
	}
}