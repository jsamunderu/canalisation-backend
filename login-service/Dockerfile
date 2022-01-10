FROM gcc:4.9 AS builder
COPY . /opt
WORKDIR /opt
RUN cc login_service.c -o login-service

FROM gcr.io/distroless/cc
COPY --from=builder /opt /opt
WORKDIR /opt
CMD ["./login-service"]
