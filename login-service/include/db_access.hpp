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

	void insertLogin(const std::string& uui, const std::string& token);

	void updateLoginActivity(const std::string& token);

	void updateLogout(const std::string& token);

	struct AuthResultSet {
		struct Data {
			std::string username;
			std::string password;
		};

		std::tuple<bool, Data> next();
		friend class LoginServiceDBAccess;
	private:
		char username[512];
		unsigned long username_len;
		my_bool username_is_null;
		my_bool username_error;
		char password[128];
		unsigned long password_len;
		my_bool password_is_null;
		my_bool password_error;

		static constexpr std::size_t BindLen = 2;
		AuthResultSet(std::shared_ptr<FetchQuery> fetch_auth);
		std::shared_ptr<FetchQuery> fetch_auth;
		std::shared_ptr<MYSQL_BIND[]> result;
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

		std::tuple<bool, Data> next();
		friend class LoginServiceDBAccess;
	private:
		char uuid[36];
		char token[128];
		std::time_t login_ts;
		std::time_t login_activity_ts;
		std::time_t logout_ts;
		unsigned long uuid_len;
		unsigned long token_len;
		my_bool is_null[5];
		my_bool error[5];
		LoginResultSet(std::shared_ptr<FetchQuery> fetch_login)
			: fetch_login(fetch_login), result(fetch_login->get()) {}
		std::shared_ptr<FetchQuery> fetch_login;
		MYSQL_BIND* result;
	};

	LoginResultSet fetchLogin(const std::string& token);
	
private:
	static constexpr std::string_view InsertLoginQuery =
		"INSERT INTO Login(uuid, token, login_ts, last_activity_ts) VALUES(?,?,?,?)";
	static constexpr std::string_view UpdateLoginActivityQuery =
		"UPDATE Login SET last_activity_ts = ? WHERE token = ?";
	static constexpr std::string_view UpdateLogoutQuery =
		"UPDATE Login SET logout_ts = ? WHERE token = ?";

	static constexpr std::size_t AuthQueryColumns = 2;
	static constexpr std::string_view AuthQuery =
		"SELECT username, password from Auth WHERE username = ?";
	static constexpr std::size_t LoginQueryColumns = 5;
	static constexpr std::string_view LoginQuery =
		"SELECT uuid, token, login_ts, last_activity_ts, logout_ts from Login WHERE token = ?";

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
