# Dark Server Dockerfile
# Multi-stage build for optimized production image

# Build stage
FROM rust:1.75-slim AS builder

# Install build dependencies
RUN apt-get update && apt-get install -y \
    pkg-config \
    libssl-dev \
    && rm -rf /var/lib/apt/lists/*

# Create app directory
WORKDIR /app

# Copy manifests
COPY Cargo.toml Cargo.lock ./

# Create a dummy main.rs to build dependencies
RUN mkdir src && echo "fn main() {}" > src/main.rs

# Build dependencies (this layer will be cached)
RUN cargo build --release && rm -rf src

# Copy source code
COPY src ./src

# Build the application
RUN cargo build --release

# Runtime stage
FROM debian:bookworm-slim

# Install runtime dependencies
RUN apt-get update && apt-get install -y \
    ca-certificates \
    && rm -rf /var/lib/apt/lists/*

# Create non-root user
RUN useradd -r -s /bin/false darkserver

# Create app directory
WORKDIR /app

# Copy binary from builder stage
COPY --from=builder /app/target/release/dark-server /usr/local/bin/dark-server

# Copy documentation
COPY README.md LICENSE CHANGELOG.md ./

# Change ownership
RUN chown -R darkserver:darkserver /app

# Switch to non-root user
USER darkserver

# Expose ports
EXPOSE 8889 9998

# Health check
HEALTHCHECK --interval=30s --timeout=10s --start-period=5s --retries=3 \
    CMD curl -f http://localhost:8889/health || exit 1

# Set environment variables
ENV RUST_LOG=info
ENV DARK_SERVER_HTTP_PORT=8889
ENV DARK_SERVER_WS_PORT=9998
ENV DARK_SERVER_HOST=0.0.0.0

# Run the application
CMD ["dark-server"]

# Metadata
LABEL maintainer="Dark Server Contributors"
LABEL description="High-performance HTTP-to-WebSocket proxy server"
LABEL version="1.0.0"
LABEL org.opencontainers.image.source="https://github.com/stevessr/dark-server"
LABEL org.opencontainers.image.documentation="https://github.com/stevessr/dark-server/blob/main/README.md"
LABEL org.opencontainers.image.licenses="MIT"
