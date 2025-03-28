# Repository configuration
repository_name        = "cpp-microservice"
repository_description = "High-performance C++ microservice for efficient data processing and real-time analytics"
repository_visibility  = "public"
organization_name      = "" # Leave blank to use personal account associated with the token
# Branch protection rules
branch_protections = {
  main = {
    pattern                         = "main"
    enforce_admins                  = true
    require_signed_commits          = true
    required_status_checks          = ["build-and-test", "security-scan"]
    required_approving_review_count = 2
    dismiss_stale_reviews           = true
    require_code_owner_reviews      = true
    allow_force_pushes              = false
    allow_deletions                 = false
  }
}
