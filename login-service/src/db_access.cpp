#include "db_access.hpp"

LoginServiceDBAccess::LoginServiceDBAccess(unsigned int port,
	const std::string& hostname,
	const std::string& database,
	const std::string& username,
	const std::string& password
) : mariadb_access(std::make_shared<MariadbAccess>(port, hostname, database, username, password)),

	insert_login_stmt(std::make_shared<MariadbStatement>(mariadb_access, InsertLoginQuery.data())),
	update_last_activity_stmt(std::make_shared<MariadbStatement>(mariadb_access, UpdateLoginActivityQuery.data())),
	update_logout_ts_stmt(std::make_shared<MariadbStatement>(mariadb_access, UpdateLogoutQuery.data())),

	auth_stmt(std::make_shared<MariadbStatement>(mariadb_access, AuthQuery.data())),
	login_stmt(std::make_shared<MariadbStatement>(mariadb_access, LoginQuery.data())),

	insert_login(std::make_shared<ExecQuery>(insert_login_stmt)),
	update_last_activity(std::make_shared<ExecQuery>(update_last_activity_stmt)),
	update_logout_ts(std::make_shared<ExecQuery>(update_logout_ts_stmt)),

	fetch_auth(std::make_shared<FetchQuery>(auth_stmt, AuthQueryColumns)),
	fetch_login(std::make_shared<FetchQuery>(login_stmt, LoginQueryColumns))
{
}

LoginServiceDBAccess::AuthResultSet::AuthResultSet(std::shared_ptr<FetchQuery> fetch_auth)
	: fetch_auth(fetch_auth), result(fetch_auth->get())
{
	result[0].buffer_type = MYSQL_TYPE_STRING;
	result[0].buffer = username;
	result[0].is_null = &username_is_null;
	result[0].length = &username_len;
	result[0].error = &username_error;

	result[1].buffer_type = MYSQL_TYPE_STRING;
	result[1].buffer = password;
	result[1].is_null = &password_is_null;
	result[1].length = &password_len;
	result[1].error = &password_error;
}

std::tuple<bool, LoginServiceDBAccess::AuthResultSet::Data> LoginServiceDBAccess::AuthResultSet::next()
{
	if (!fetch_auth->next()) {
		return std::tuple{false, Data{}};
	}

	Data data;
	data.username.assign(static_cast<const char *>(username), static_cast<std::size_t>(username_len));
	data.password.assign(static_cast<const char *>(password), static_cast<std::size_t>(password_len));
	return std::tuple{true, data};
}

std::tuple<bool, LoginServiceDBAccess::LoginResultSet::Data> LoginServiceDBAccess::LoginResultSet::next()
{
	if (!fetch_login->next()) {
		return std::tuple{false, Data{}};
	}

	Data data;
	return std::tuple{true, data};
}

void LoginServiceDBAccess::insertLogin(const std::string& uuid, const std::string& token)
{
	MYSQL_BIND bind[2];
	std::memset(bind, 0, sizeof(bind));

	std::size_t uuid_length = uuid.size();

	bind[0].buffer_type = MYSQL_TYPE_STRING;
	bind[0].buffer = const_cast<char *>(uuid.c_str());
	bind[0].buffer_length= uuid.size();
	bind[0].is_null= 0;
	bind[0].length= &uuid_length;

	std::size_t tkn_length = token.size();
	bind[1].buffer_type = MYSQL_TYPE_STRING;
	bind[1].buffer = const_cast<char *>(token.c_str());
	bind[1].buffer_length= token.size();
	bind[1].is_null= 0;
	bind[1].length= &tkn_length;

	insert_login->execute(bind);
}

void LoginServiceDBAccess::updateLoginActivity(const std::string& token)
{
	MYSQL_BIND bind[2];
	std::memset(bind, 0, sizeof(bind));

	std::size_t tkn_length = token.size();

	bind[0].buffer_type = MYSQL_TYPE_STRING;
	bind[0].buffer = const_cast<char *>(token.c_str());
	bind[0].buffer_length= token.size();
	bind[0].is_null= 0;
	bind[0].length= &tkn_length;

	update_last_activity->execute(bind);
}

void LoginServiceDBAccess::updateLogout(const std::string& token)
{
	MYSQL_BIND bind[2];
	std::memset(bind, 0, sizeof(bind));

	std::size_t tkn_length = token.size();

	bind[0].buffer_type = MYSQL_TYPE_STRING;
	bind[0].buffer = const_cast<char *>(token.c_str());
	bind[0].buffer_length= token.size();
	bind[0].is_null= 0;
	bind[0].length= &tkn_length;

	update_last_activity->execute(bind);
}

LoginServiceDBAccess::AuthResultSet LoginServiceDBAccess::fetchAuth(const std::string& username)
{
	MYSQL_BIND bind[1];
	std::memset(bind, 0, sizeof(bind));

	std::size_t uname_length = username.size();

	bind[0].buffer_type = MYSQL_TYPE_STRING;
	bind[0].buffer = const_cast<char *>(username.c_str());
	bind[0].buffer_length= username.size();
	bind[0].is_null= 0;
	bind[0].length= &uname_length;

	fetch_auth->fetch(bind);

	return AuthResultSet(fetch_auth);
}

LoginServiceDBAccess::LoginResultSet LoginServiceDBAccess::fetchLogin(const std::string& token)
{
	MYSQL_BIND bind[1];
	std::memset(bind, 0, sizeof(bind));

	std::size_t tkn_length = token.size();

	bind[0].buffer_type = MYSQL_TYPE_STRING;
	bind[0].buffer = const_cast<char *>(token.c_str());
	bind[0].buffer_length= token.size();
	bind[0].is_null= 0;
	bind[0].length= &tkn_length;

	fetch_login->fetch(bind);

	return LoginResultSet(fetch_login);
}
