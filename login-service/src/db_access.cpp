#include <iostream>
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

	fetch_auth(std::make_shared<FetchQuery>(auth_stmt)),
	fetch_login(std::make_shared<FetchQuery>(login_stmt))
{
}

LoginServiceDBAccess::AuthResultSet::AuthResultSet(std::shared_ptr<FetchQuery> fetch_auth,
		unsigned long long count)
	: fetch_auth(fetch_auth), result(std::make_unique<MYSQL_BIND[]>(count)),
		count(count), rs_metadata(nullptr)
{
	rs_metadata = mysql_stmt_result_metadata(fetch_auth->get().get());
	if (rs_metadata == nullptr) {
		throw Exception(DatabaseAccessError::STATEMENT, fetch_auth->get().reportError());
	}
	MYSQL_FIELD *fields = mysql_fetch_fields(rs_metadata);
	std::memset(result.get(), 0, sizeof(MYSQL_BIND) * count);
	result[0].buffer_type = fields[0].type;
	result[0].buffer = username_buf;
	result[0].buffer_length = username_len;
	result[0].is_null = &username_is_null;
	result[0].error = &username_error;
	result[0].length = &username_len;

	result[1].buffer_type = fields[0].type;
	result[1].buffer = password_buf;
	result[1].buffer_length = password_len;
	result[1].is_null = &password_is_null;
	result[1].error = &password_error;
	result[1].length = &password_len;

	if (mysql_stmt_bind_result(fetch_auth->get().get(), result.get())) {
		throw Exception(DatabaseAccessError::STATEMENT, fetch_auth->get().reportError());
	}
}

LoginServiceDBAccess::AuthResultSet::~AuthResultSet()
{
	if (rs_metadata) {
		mysql_free_result(rs_metadata);
	}
	mysql_stmt_reset(fetch_auth->get().get());
}

void LoginServiceDBAccess::AuthResultSet::copy(LoginServiceDBAccess::AuthResultSet const& other)
{
	std::memcpy(this->username_buf, username_buf, sizeof(username_buf));
	std::memcpy(this->password_buf, password_buf, sizeof(password_buf));
	this->username_is_null = other.username_is_null;
	this->password_is_null = other.password_is_null;
	this->username_len = username_len;
	this->password_len = password_len;
	this->username_error = other.username_error;
	this->password_error = other.password_error;
	this->fetch_auth = other.fetch_auth;
	this->result.reset(other.result.get());
	this->rs_metadata = other.rs_metadata;
	this->count = other.count;
}

std::tuple<bool, LoginServiceDBAccess::AuthResultSet::Data> LoginServiceDBAccess::AuthResultSet::next()
{
	Data data;
	if (!fetch_auth->next()) {
		return std::tuple{false, data};
	}

	data.username.assign(static_cast<const char *>(username_buf), static_cast<std::size_t>(username_len));
	data.password.assign(static_cast<const char *>(password_buf), static_cast<std::size_t>(password_len));
	return std::tuple{true, data};
}

LoginServiceDBAccess::LoginResultSet::LoginResultSet(std::shared_ptr<FetchQuery> fetch_login, unsigned long long count)
	: fetch_login(fetch_login), result(std::make_unique<MYSQL_BIND[]>(count)),
		count(count), rs_metadata(nullptr)
{
	rs_metadata = mysql_stmt_result_metadata(fetch_login->get().get());
	if (rs_metadata == nullptr) {
		throw Exception(DatabaseAccessError::STATEMENT, fetch_login->get().reportError());
	}
	MYSQL_FIELD *fields = mysql_fetch_fields(rs_metadata);
	std::memset(result.get(), 0, sizeof(MYSQL_BIND) * count);

	result[0].buffer_type = fields[0].type;
	result[0].buffer = uuid_buf;
	result[0].buffer_length = buf_len[0];
	result[0].is_null = &is_null[0];
	result[0].error = &error[0];
	result[0].length = &buf_len[0];

	result[1].buffer_type = fields[1].type;
	result[1].buffer = token_buf;
	result[1].buffer_length = buf_len[1];
	result[1].is_null = &is_null[1];
	result[1].error = &error[1];
	result[1].length = &buf_len[1];

	result[2].buffer_type = fields[2].type;
	result[2].buffer = &login_ts;
	result[2].buffer_length = sizeof(login_ts);
	result[2].is_null = &is_null[2];
	result[2].error = &error[2];
	result[2].length = &buf_len[2];

	result[3].buffer_type = fields[3].type;
	result[3].buffer = &login_activity_ts;
	result[3].buffer_length = sizeof(login_activity_ts);
	result[3].is_null = &is_null[3];
	result[3].error = &error[3];
	result[3].length = &buf_len[3];

	result[4].buffer_type = fields[4].type;
	result[4].buffer = &logout_ts;
	result[4].buffer_length = sizeof(logout_ts);
	result[4].is_null = &is_null[4];
	result[4].error = &error[4];
	result[4].length = &buf_len[4];

	if (mysql_stmt_bind_result(fetch_login->get().get(), result.get())) {
		throw Exception(DatabaseAccessError::STATEMENT, fetch_login->get().reportError());
	}
}

LoginServiceDBAccess::LoginResultSet::~LoginResultSet()
{
	if (rs_metadata) {
		mysql_free_result(rs_metadata);
	}
	mysql_stmt_reset(fetch_login->get().get());
}

void LoginServiceDBAccess::LoginResultSet::copy(LoginServiceDBAccess::LoginResultSet const& other)
{
	std::memcpy(this->uuid_buf, uuid_buf, sizeof(uuid_buf));
	std::memcpy(this->token_buf, token_buf, sizeof(token_buf));
	std::memcpy(this->buf_len, buf_len, sizeof(buf_len));
	std::memcpy(this->is_null, is_null, sizeof(is_null));
	std::memcpy(this->error, error, sizeof(error));
	this->login_ts = other.login_ts;
	this->login_activity_ts = other.login_activity_ts;
	this->logout_ts = other.logout_ts;
	this->fetch_login = other.fetch_login;
	this->result.reset(other.result.get());
	this->rs_metadata = other.rs_metadata;
	this->count = other.count;
}

std::tuple<bool, LoginServiceDBAccess::LoginResultSet::Data> LoginServiceDBAccess::LoginResultSet::next()
{
	Data data;
	if (!fetch_login->next()) {
		return std::tuple{false, Data{}};
	}
	data.uuid.assign(static_cast<const char *>(uuid_buf));
	data.token.assign(static_cast<const char *>(token_buf));
	data.login_ts = login_ts;
	data.login_activity_ts = login_activity_ts;
	data.logout_ts = logout_ts;
	return std::tuple{true, data};
}

unsigned long long LoginServiceDBAccess::insertLogin(const std::string& uuid, const std::string& token)
{
	MYSQL_BIND bind[2];
	std::memset(bind, 0, sizeof(bind));

	std::size_t uuid_length = uuid.size();
	bind[0].buffer_type = MYSQL_TYPE_STRING;
	bind[0].buffer = const_cast<char *>(uuid.c_str());
	bind[0].buffer_length = uuid.size();
	bind[0].is_null = 0;
	bind[0].length = &uuid_length;

	std::size_t tkn_length = token.size();
	bind[1].buffer_type = MYSQL_TYPE_STRING;
	bind[1].buffer = const_cast<char *>(token.c_str());
	bind[1].buffer_length = token.size();
	bind[1].is_null = 0;
	bind[1].length = &tkn_length;

	return insert_login->execute(bind);
}

unsigned long long LoginServiceDBAccess::updateLoginActivity(const std::string& token)
{
	MYSQL_BIND bind[1];
	std::memset(bind, 0, sizeof(bind));

	std::size_t tkn_length = token.size();
	bind[0].buffer_type = MYSQL_TYPE_STRING;
	bind[0].buffer = const_cast<char *>(token.c_str());
	bind[0].buffer_length = token.size();
	bind[0].is_null = 0;
	bind[0].length = &tkn_length;

	return update_last_activity->execute(bind);
}

unsigned long long LoginServiceDBAccess::updateLogout(const std::string& token)
{
	MYSQL_BIND bind[1];
	std::memset(bind, 0, sizeof(bind));

	std::size_t tkn_length = token.size();
	bind[0].buffer_type = MYSQL_TYPE_STRING;
	bind[0].buffer = const_cast<char *>(token.c_str());
	bind[0].buffer_length = token.size();
	bind[0].is_null = 0;
	bind[0].length = &tkn_length;

	return update_last_activity->execute(bind);
}

LoginServiceDBAccess::AuthResultSet LoginServiceDBAccess::fetchAuth(const std::string& username)
{
	MYSQL_BIND bind[1];
	std::memset(bind, 0, sizeof(bind));

	my_bool username_is_null = 0;
	my_bool username_error = 0;

	std::size_t uname_length = username.size();
	bind[0].buffer_type = MYSQL_TYPE_STRING;
	bind[0].buffer = const_cast<char *>(username.c_str());
	bind[0].buffer_length = username.size();
	bind[0].is_null = &username_is_null;
	bind[0].length = &uname_length;
	bind[0].error = &username_error;

	auto fields = fetch_auth->fetch(bind);

	return AuthResultSet(fetch_auth, fields);
}

LoginServiceDBAccess::LoginResultSet LoginServiceDBAccess::fetchLogin(const std::string& token)
{
	MYSQL_BIND bind[1];
	std::memset(bind, 0, sizeof(bind));

	std::size_t tkn_length = token.size();
	bind[0].buffer_type = MYSQL_TYPE_STRING;
	bind[0].buffer = const_cast<char *>(token.c_str());
	bind[0].buffer_length = token.size();
	bind[0].is_null = 0;
	bind[0].length = &tkn_length;

	auto fields = fetch_login->fetch(bind);

	return LoginResultSet(fetch_login, fields);
}
