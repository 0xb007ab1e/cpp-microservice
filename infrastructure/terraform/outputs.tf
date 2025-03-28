
output "repository_html_url" {
  description = "The HTML URL of the repository"
  value       = github_repository.microservice_repo.html_url
}

output "repository_ssh_clone_url" {
  description = "The SSH URL of the repository"
  value       = github_repository.microservice_repo.ssh_clone_url
}

output "repository_http_clone_url" {
  description = "The HTTP URL of the repository"
  value       = github_repository.microservice_repo.http_clone_url
}

output "repository_git_clone_url" {
  description = "The Git URL of the repository"
  value       = github_repository.microservice_repo.git_clone_url
}

output "repository_id" {
  description = "The ID of the repository"
  value       = github_repository.microservice_repo.id
}

output "repository_node_id" {
  description = "The Node ID of the repository"
  value       = github_repository.microservice_repo.node_id
}
output "branch_protection_rules" {
  description = "The branch protection rules applied to the repository"
  value = {
    for key, protection in github_branch_protection.protection : key => {
      pattern                         = protection.pattern
      enforce_admins                  = protection.enforce_admins
      require_signed_commits          = protection.require_signed_commits
      allows_force_pushes             = protection.allows_force_pushes
      allows_deletions                = protection.allows_deletions
      required_approving_review_count = protection.required_pull_request_reviews[0].required_approving_review_count
      dismiss_stale_reviews           = protection.required_pull_request_reviews[0].dismiss_stale_reviews
      require_code_owner_reviews      = protection.required_pull_request_reviews[0].require_code_owner_reviews
      required_status_checks          = protection.required_status_checks[0].contexts
    }
  }
}

