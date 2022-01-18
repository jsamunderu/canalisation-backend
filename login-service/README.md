# Login Service

This service is responsible for authentication and session management

# Session Management

A user login and is validated via the REST API, and they are assigned a
token. The token will expire after a specific time of inactivity.

Other services connect to the login service through a gRPC interface
and verify sessions and check for authorization.
