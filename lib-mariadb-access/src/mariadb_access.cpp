#include <sstream>
#include <iostream>
#include "mariadb_access.hpp"

MariadbAccess::MariadbAccess(const MariadbAccess::Config& config)
{
	if ((con = mysql_init(NULL)) == NULL) {
		std::stringstream ss;
		ss << "mysql_init()";

		throw Exception(DatabaseAccessError::CONNECTION, ss.str());
    	}

	if (mysql_real_connect(con, config.host.c_str(),
			config.username.c_str(), config.password.c_str(),
			config.database.c_str(), config.port, NULL, 0) == NULL) {
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

unsigned long long FetchQuery::fetch(MYSQL_BIND parameters[])
{
	if (mysql_stmt_bind_param(stmt->get(), parameters)) {
		throw Exception(DatabaseAccessError::STATEMENT, stmt->reportError());
	}

	if (mysql_stmt_execute(stmt->get())) {
		throw Exception(DatabaseAccessError::STATEMENT, stmt->reportError());
	}

	auto fields = mysql_stmt_field_count(stmt->get());

	return fields;
}

bool FetchQuery::next()
{
	auto status = mysql_stmt_fetch(stmt->get());
	switch (status) {
	case MYSQL_NO_DATA:
		return false;
	case 1:
		throw Exception(DatabaseAccessError::STATEMENT, stmt->reportError());
	case MYSQL_DATA_TRUNCATED:
		break;
	default:
		break;
	}
	return true;
}

unsigned long long ExecQuery::execute(MYSQL_BIND parameters[])
{
	if (mysql_stmt_bind_param(stmt->get(), parameters)) {
		throw Exception(DatabaseAccessError::STATEMENT, stmt->reportError());
	}
	if (mysql_stmt_execute(stmt->get())) {
		throw Exception(DatabaseAccessError::STATEMENT, stmt->reportError());
	}
	
	auto rows = mysql_affected_rows(stmt->get_con().get());

	return rows;
}