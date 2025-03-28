#!/usr/bin/env bash

#############################################################################
# setup-template-project.sh
#
# Description: 
# This script automates the setup of GitHub repositories for C++ microservices
# by configuring vulnerability alerts, GitHub Actions secrets, repository
# environments, and other essential settings.
#
# Usage:
#   ./setup-template-project.sh -t <GITHUB_TOKEN> [-r <REPOSITORY>] [-o <ORGANIZATION>]
#
# Options:
#   -t, --token         GitHub personal access token (required)
#   -r, --repository    Repository name (default: current repository)
#   -o, --org           GitHub organization (default: extracted from remote URL)
#   -h, --help          Display this help message
#
# Required dependencies:
#   - GitHub CLI (gh)
#   - curl
#   - jq
#
# Author: Your Name
# Version: 1.0.0
#############################################################################

set -eo pipefail

# Color codes for terminal output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[0;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Default values
GITHUB_TOKEN=""
REPO_NAME=""
ORG_NAME=""

#############################################################################
# HELPER FUNCTIONS
#############################################################################

# Log messages with different severity levels
log_info() {
  echo -e "${BLUE}[INFO]${NC} $1"
}

log_success() {
  echo -e "${GREEN}[SUCCESS]${NC} $1"
}

log_warning() {
  echo -e "${YELLOW}[WARNING]${NC} $1"
}

log_error() {
  echo -e "${RED}[ERROR]${NC} $1" >&2
}

# Display help message
show_help() {
  grep "^# " "$0" | cut -c 3- | sed -n '/Description:/,/^$/p'
  echo "Options:"
  grep "^#   -" "$0" | cut -c 6-
  exit 0
}

# Check if a command exists
command_exists() {
  command -v "$1" >/dev/null 2>&1
}

# Check for required dependencies
check_dependencies() {
  log_info "Checking for required dependencies..."
  
  local missing_deps=0
  
  for cmd in gh curl jq; do
    if ! command_exists "$cmd"; then
      log_error "$cmd is required but not installed."
      missing_deps=1
    fi
  done
  
  if [ $missing_deps -eq 1 ]; then
    log_error "Please install the missing dependencies and try again."
    exit 1
  fi
  
  log_success "All required dependencies are installed."
}

# Parse command line arguments
parse_args() {
  while [[ $# -gt 0 ]]; do
    case "$1" in
      -t|--token)
        GITHUB_TOKEN="$2"
        shift 2
        ;;
      -r|--repository)
        REPO_NAME="$2"
        shift 2
        ;;
      -o|--org)
        ORG_NAME="$2"
        shift 2
        ;;
      -h|--help)
        show_help
        ;;
      *)
        log_error "Unknown option: $1"
        show_help
        ;;
    esac
  done
  
  # Validate required arguments
  if [ -z "$GITHUB_TOKEN" ]; then
    log_error "GitHub token is required. Use -t or --token to provide it."
    exit 1
  fi
}

# Detect repository and organization if not provided
detect_repo_info() {
  if [ -z "$REPO_NAME" ]; then
    REPO_NAME=$(basename -s .git "$(git config --get remote.origin.url 2>/dev/null || echo "")")
    if [ -z "$REPO_NAME" ]; then
      log_error "Could not detect repository name. Please specify with -r or --repository."
      exit 1
    fi
  fi
  
  if [ -z "$ORG_NAME" ]; then
    # Try to extract organization from remote URL
    remote_url=$(git config --get remote.origin.url 2>/dev/null || echo "")
    if [[ $remote_url =~ github\.com[:/]([^/]+)/[^/]+ ]]; then
      ORG_NAME="${BASH_REMATCH[1]}"
    else
      log_error "Could not detect organization name. Please specify with -o or --org."
      exit 1
    fi
  fi
  
  log_info "Using repository: $ORG_NAME/$REPO_NAME"
}

#############################################################################
# GITHUB CONFIGURATION FUNCTIONS
#############################################################################

# Configure vulnerability alerts and security settings
configure_vulnerability_alerts() {
  log_info "Configuring vulnerability alerts and security settings..."
  
  # Enable Dependabot alerts
  curl -s -X PUT \
    -H "Authorization: token $GITHUB_TOKEN" \
    -H "Accept: application/vnd.github.v3+json" \
    "https://api.github.com/repos/$ORG_NAME/$REPO_NAME/vulnerability-alerts" \
    -d '{}'
  
  # Enable Dependabot security updates
  curl -s -X PUT \
    -H "Authorization: token $GITHUB_TOKEN" \
    -H "Accept: application/vnd.github.v3+json" \
    "https://api.github.com/repos/$ORG_NAME/$REPO_NAME/automated-security-fixes" \
    -d '{}'
  
  # Enable all security features
  curl -s -X PATCH \
    -H "Authorization: token $GITHUB_TOKEN" \
    -H "Accept: application/vnd.github.v3+json" \
    "https://api.github.com/repos/$ORG_NAME/$REPO_NAME" \
    -d '{"security_and_analysis": {"secret_scanning": {"status": "enabled"}, "secret_scanning_push_protection": {"status": "enabled"}, "dependabot_security_updates": {"status": "enabled"}}}'
  
  if [ $? -eq 0 ]; then
    log_success "Successfully configured vulnerability alerts and security settings."
  else
    log_error "Failed to configure vulnerability alerts and security settings."
    return 1
  fi
}

# Set up GitHub Actions secrets
setup_github_actions_secrets() {
  log_info "Setting up GitHub Actions secrets..."
  
  # Authenticate with GitHub CLI
  echo "$GITHUB_TOKEN" | gh auth login --with-token
  
  # Define secrets to be set (example)
  declare -A secrets=(
    ["CONTAINER_REGISTRY_TOKEN"]="Default value or leave empty to prompt"
    ["DEPLOY_KEY"]=""
    ["SONAR_TOKEN"]=""
  )
  
  for secret_name in "${!secrets[@]}"; do
    secret_value="${secrets[$secret_name]}"
    
    # If the secret value is empty, prompt the user for input
    if [ -z "$secret_value" ]; then
      read -r -p "Enter value for $secret_name (leave empty to skip): " secret_value
      # Skip if the user didn't provide a value
      if [ -z "$secret_value" ]; then
        log_warning "Skipping $secret_name as no value was provided."
        continue
      fi
    fi
    
    # Set the secret
    echo "$secret_value" | gh secret set "$secret_name" --repo "$ORG_NAME/$REPO_NAME"
    
    if [ $? -eq 0 ]; then
      log_success "Successfully set secret: $secret_name"
    else
      log_error "Failed to set secret: $secret_name"
    fi
  done
}

# Configure repository environments
configure_repository_environments() {
  log_info "Configuring repository environments..."
  
  # Define environments to create
  environments=("production" "staging" "development")
  
  for env in "${environments[@]}"; do
    log_info "Setting up '$env' environment..."
    
    # Create environment if it doesn't exist
    curl -s -X PUT \
      -H "Authorization: token $GITHUB_TOKEN" \
      -H "Accept: application/vnd.github.v3+json" \
      "https://api.github.com/repos/$ORG_NAME/$REPO_NAME/environments/$env" \
      -d '{}'
    
    # Configure environment-specific protection rules for production
    if [ "$env" == "production" ]; then
      curl -s -X PUT \
        -H "Authorization: token $GITHUB_TOKEN" \
        -H "Accept: application/vnd.github.v3+json" \
        "https://api.github.com/repos/$ORG_NAME/$REPO_NAME/environments/$env" \
        -d '{
          "wait_timer": 30,
          "reviewers": [],
          "deployment_branch_policy": {
            "protected_branches": true,
            "custom_branch_policies": false
          }
        }'
    fi
    
    if [ $? -eq 0 ]; then
      log_success "Successfully configured environment: $env"
    else
      log_error "Failed to configure environment: $env"
    fi
  done
}

# Configure branch protection rules
configure_branch_protection() {
  log_info "Configuring branch protection rules for 'main' branch..."
  
  curl -s -X PUT \
    -H "Authorization: token $GITHUB_TOKEN" \
    -H "Accept: application/vnd.github.v3+json" \
    "https://api.github.com/repos/$ORG_NAME/$REPO_NAME/branches/main/protection" \
    -d '{
      "required_status_checks": {
        "strict": true,
        "contexts": ["build-and-test", "security-scan"]
      },
      "enforce_admins": true,
      "required_pull_request_reviews": {
        "dismiss_stale_reviews": true,
        "require_code_owner_reviews": true,
        "required_approving_review_count": 1
      },
      "restrictions": null
    }'
  
  if [ $? -eq 0 ]; then
    log_success "Successfully configured branch protection rules for 'main' branch."
  else
    log_error "Failed to configure branch protection rules for 'main' branch."
    return 1
  fi
}

#############################################################################
# MAIN EXECUTION
#############################################################################

main() {
  log_info "Starting GitHub repository setup..."
  
  # Check dependencies
  check_dependencies
  
  # Parse command line arguments
  parse_args "$@"
  
  # Detect repository information
  detect_repo_info
  
  # Configure vulnerability alerts
  configure_vulnerability_alerts || log_error "Failed to configure vulnerability alerts."
  
  # Set up GitHub Actions secrets
  setup_github_actions_secrets || log_error "Failed to set up GitHub Actions secrets."
  
  # Configure repository environments
  configure_repository_environments || log_error "Failed to configure repository environments."
  
  # Configure branch protection
  configure_branch_protection || log_error "Failed to configure branch protection."
  
  log_success "GitHub repository setup completed successfully!"
  log_info "Repository: $ORG_NAME/$REPO_NAME is now configured with:"
  log_info "- Vulnerability alerts and security settings"
  log_info "- GitHub Actions secrets"
  log_info "- Repository environments (production, staging, development)"
  log_info "- Branch protection rules for 'main' branch"
  
  log_info "Manual steps that may be required:"
  log_info "1. Set up project-specific GitHub Actions secrets that couldn't be automated"
  log_info "2. Configure CODEOWNERS file if not already present"
  log_info "3. Customize environment protection rules as needed"
}

# Execute main function with all arguments
main "$@"

#!/bin/bash
set -euo pipefail

# Configuration
GITHUB_USERNAME="0xb007ab1e"
TEMPLATE_ROOT="/home/b007ab1e/src/Projects/Templates"
BASE_LANGS=("C++" "Python" "Go" "Rust")
BASE_TYPES=("microservice" "library" "cli" "web")

# Color output
RED='\033[0;31m'
GREEN='\033[0;32m'
BLUE='\033[0;34m'
NC='\033[0m'

log_info() { echo -e "${BLUE}[INFO]${NC} $1"; }
log_success() { echo -e "${GREEN}[SUCCESS]${NC} $1"; }
log_error() { echo -e "${RED}[ERROR]${NC} $1"; }

check_prerequisites() {
    log_info "Checking prerequisites..."
    
    # Check if gh is installed
    if ! command -v gh &> /dev/null; then
        log_error "GitHub CLI (gh) not found. Please install it first."
        exit 1
    fi

    # Verify gh authentication
    if ! gh auth status &> /dev/null; then
        log_error "Not authenticated with GitHub. Please run 'gh auth login' first."
        exit 1
    fi

    # Check if git is configured
    if ! git config --get user.email &> /dev/null; then
        log_error "Git email not configured. Please configure git first."
        exit 1
    fi
}

setup_main_repository() {
    log_info "Setting up main Templates repository..."
    
    # Create and initialize Templates repository
    mkdir -p "$TEMPLATE_ROOT"
    cd "$TEMPLATE_ROOT" || exit 1

    # Create base structure
    for lang in "${BASE_LANGS[@]}"; do
        for type in "${BASE_TYPES[@]}"; do
            mkdir -p "$lang/$type"
        done
    done

    # Initialize git if not already initialized
    if [ ! -d .git ]; then
        git init
        gh repo create "$GITHUB_USERNAME/Templates" --private --source=. --remote=origin || {
            log_error "Failed to create Templates repository"
            exit 1
        }
    fi
}

setup_cpp_microservice() {
    log_info "Setting up C++ microservice template..."
    
    local TEMPLATE_PATH="$TEMPLATE_ROOT/C++/microservice/base-template"
    
    # Create repository if it doesn't exist
    if ! gh repo view "$GITHUB_USERNAME/cpp-microservice-template" &> /dev/null; then
        gh repo create "$GITHUB_USERNAME/cpp-microservice-template" --private || {
            log_error "Failed to create cpp-microservice-template repository"
            exit 1
        }
    fi

    # Clone and set up the repository
    git clone "https://github.com/$GITHUB_USERNAME/cpp-microservice-template.git" "$TEMPLATE_PATH" || {
        log_error "Failed to clone cpp-microservice-template"
        exit 1
    }
    cd "$TEMPLATE_PATH" || exit 1

    # Set up GitHub environments and secrets
    for env in "development" "staging" "production"; do
        gh api -X PUT "/repos/$GITHUB_USERNAME/cpp-microservice-template/environments/$env" \
            --field environment_name="$env" || {
            log_error "Failed to create environment: $env"
            continue
        }
        
        # Set environment-specific secrets
        gh secret set "DATABASE_URL" -b "{{DATABASE_URL}}" --env "$env" || log_error "Failed to set DATABASE_URL for $env"
        gh secret set "API_ENDPOINT" -b "{{API_ENDPOINT}}" --env "$env" || log_error "Failed to set API_ENDPOINT for $env"
    done

    # Set repository secrets
    gh secret set "DOCKER_REGISTRY_TOKEN" -b "{{DOCKER_REGISTRY_TOKEN}}" || log_error "Failed to set DOCKER_REGISTRY_TOKEN"
    gh secret set "SERVICE_API_KEY" -b "{{SERVICE_API_KEY}}" || log_error "Failed to set SERVICE_API_KEY"
    gh secret set "TEST_DATABASE_URL" -b "{{TEST_DATABASE_URL}}" || log_error "Failed to set TEST_DATABASE_URL"

    log_success "C++ microservice template setup complete"
}

setup_local_development() {
    log_info "Setting up local development environment..."
    
    # Create secrets directory
    local SECRETS_DIR="$HOME/.secrets/cpp-microservice"
    mkdir -p "$SECRETS_DIR"
    chmod 700 "$SECRETS_DIR"

    # Generate development secrets template
    cat > "$SECRETS_DIR/template.env" << 'EOLS'
# Development Environment Configuration
DATABASE_URL={{DATABASE_URL}}
API_KEY={{API_KEY}}
API_ENDPOINT={{API_ENDPOINT}}
DOCKER_REGISTRY_TOKEN={{DOCKER_REGISTRY_TOKEN}}
EOLS
    chmod 600 "$SECRETS_DIR/template.env"

    log_success "Local development environment setup complete"
}

main() {
    log_info "Starting project setup..."
    
    check_prerequisites || exit 1
    setup_main_repository || exit 1
    setup_cpp_microservice || exit 1
    setup_local_development || exit 1

    log_success "Project setup complete!"
    log_info "Next steps:"
    echo "1. Replace placeholder values in $HOME/.secrets/cpp-microservice/template.env"
    echo "2. Review and customize GitHub Actions workflows"
}

main "$@"
