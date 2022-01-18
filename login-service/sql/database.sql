--DROP DATABASE IF EXISTS canalisaction_access;

--CREATE DATABASE canalisaction_access;

CREATE TABLE Auth (
	uuid CHAR(36) PRIMARY KEY,
	username VARCHAR(256) NOT NULL,
	password VARCHAR(256) NOT NULL
);

CREATE TABLE Login (
	uuid CHAR(36) PRIMARY KEY,
	token VARCHAR(128) NOT NULL,
	login_ts TIMESTAMP NOT NULL,
	last_activity_ts TIMESTAMP NOT NULL,
	logout_ts TIMESTAMP
);
