#ifndef LOGIN_SERVICE_DB_ACCESS_HPP
#define LOGIN_SERVICE_DB_ACCESS_HPP
#include <memory>
#include <ctime>
#include "mariadb_access.hpp"
class LoginServiceDBAccess {
public:
	LoginServiceDBAccess(unsigned int port,
		const std::string& hostname,
		const std::string& database,
		const std::string& username,
		const std::string& password
	);

	unsigned long long insertLogin(const std::string& uui, const std::string& token);

	unsigned long long updateLoginActivity(const std::string& token);

	unsigned long long updateLogout(const std::string& token);

	struct AuthResultSet {
		struct Data {
			std::string username;
			std::string password;
		};

		unsigned long long rows () { return count; }
		std::tuple<bool, Data> next();
		~AuthResultSet();
		friend class LoginServiceDBAccess;
	private:
		char username_buf[256];
		char password_buf[256];
		my_bool username_is_null;
		my_bool password_is_null;
		unsigned long username_len = sizeof(username_buf);
		unsigned long password_len = sizeof(password_buf);
		my_bool username_error;
		my_bool password_error;
		AuthResultSet(std::shared_ptr<FetchQuery> fetch_auth, unsigned long long count);
		std::shared_ptr<FetchQuery> fetch_auth;
		std::unique_ptr<MYSQL_BIND[]> result;
		MYSQL_RES *rs_metadata;
		unsigned long long count;
	};

	AuthResultSet fetchAuth(const std::string& username);

	struct LoginResultSet {
		struct Data {
			std::string uuid;
			std::string token;
			std::time_t login_ts;
			std::time_t login_activity_ts;
			std::time_t logout_ts;
		};

		unsigned long long rows () { return count; }
		std::tuple<bool, Data> next();
		~LoginResultSet();
		friend class LoginServiceDBAccess;
	private:
		char uuid_buf[36];
		char token_buf[128];
		std::time_t login_ts;
		std::time_t login_activity_ts;
		std::time_t logout_ts;
		unsigned long buf_len[5] = {sizeof(uuid_buf), sizeof(token_buf), 0, 0, 0};
		my_bool is_null[5];
		my_bool error[5];
		LoginResultSet(std::shared_ptr<FetchQuery> fetch_login, unsigned long long count);
		std::shared_ptr<FetchQuery> fetch_login;
		std::unique_ptr<MYSQL_BIND[]> result;
		MYSQL_RES *rs_metadata;
		unsigned long long count;
	};

	LoginResultSet fetchLogin(const std::string& token);
	
private:
	static constexpr std::string_view InsertLoginQuery =
		"INSERT INTO Login(uuid, token, login_ts, last_activity_ts) VALUES(?,?,now(),now())";
	static constexpr std::string_view UpdateLoginActivityQuery =
		"UPDATE Login SET last_activity_ts = ? WHERE token = ?";
	static constexpr std::string_view UpdateLogoutQuery =
		"UPDATE Login SET logout_ts = ? WHERE token = ?";

	static constexpr std::size_t AuthQueryColumns = 2;
	static constexpr std::string_view AuthQuery =
		"SELECT username, password from Auth WHERE username = ?";
	static constexpr std::size_t LoginQueryColumns = 5;
	static constexpr std::string_view LoginQuery =
		"SELECT uuid, token, login_ts, last_activity_ts, logout_ts from Login";

	std::shared_ptr<MariadbAccess> mariadb_access;

	std::shared_ptr<MariadbStatement> insert_login_stmt;
	std::shared_ptr<MariadbStatement> update_last_activity_stmt;
	std::shared_ptr<MariadbStatement> update_logout_ts_stmt;

	std::shared_ptr<MariadbStatement> auth_stmt;
	std::shared_ptr<MariadbStatement> login_stmt;

	std::shared_ptr<ExecQuery> insert_login;
	std::shared_ptr<ExecQuery> update_last_activity;
	std::shared_ptr<ExecQuery> update_logout_ts;

	std::shared_ptr<FetchQuery> fetch_auth;
	std::shared_ptr<FetchQuery> fetch_login;
};

#endif
