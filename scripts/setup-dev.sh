#!/bin/bash
set -euo pipefail

# Configuration
SECRETS_DIR="$HOME/.secrets/cpp-microservice"
ENV_FILE=".env"
TEMPLATE_FILE=".env.template"

# Create secrets directory
mkdir -p "$SECRETS_DIR"
chmod 700 "$SECRETS_DIR"

# Copy env template if not exists
if [ ! -f "$ENV_FILE" ]; then
    cp "$TEMPLATE_FILE" "$ENV_FILE"
    chmod 600 "$ENV_FILE"
fi

echo "Development environment setup complete."
echo "Please edit $ENV_FILE and add your GitHub credentials."
echo "Secrets should be stored in $SECRETS_DIR"
