
locals {
  default_topics = ["cpp", "microservice", "continuous-integration"]
  
  standard_labels = {
    bug = {
      name        = "bug"
      description = "Something isn't working"
      color       = "d73a4a"
    }
    documentation = {
      name        = "documentation"
      description = "Improvements or additions to documentation"
      color       = "0075ca"
    }
    enhancement = {
      name        = "enhancement"
      description = "New feature or request"
      color       = "a2eeef"
    }
    "good first issue" = {
      name        = "good first issue"
      description = "Good for newcomers"
      color       = "7057ff"
    }
    "help wanted" = {
      name        = "help wanted"
      description = "Extra attention is needed"
      color       = "008672"
    }
    security = {
      name        = "security"
      description = "Security related issue"
      color       = "ee0701"
    }
    performance = {
      name        = "performance"
      description = "Performance improvement or degradation"
      color       = "fbca04"
    }
    dependencies = {
      name        = "dependencies"
      description = "Pull requests that update a dependency file"
      color       = "0366d6"
    }
  }
}

# Repository configuration
resource "github_repository" "microservice_repo" {
  name        = var.repository_name
  description = var.repository_description
  visibility  = var.repository_visibility

  allow_merge_commit     = true
  allow_rebase_merge     = true
  allow_squash_merge     = true
  allow_auto_merge       = true
  delete_branch_on_merge = true

  has_issues    = true
  has_projects  = true
  has_wiki      = true
  has_downloads = true

  vulnerability_alerts = true
  topics               = local.default_topics
  auto_init = true
}

# Branch protection rules
resource "github_branch_protection" "protection" {
  for_each = var.branch_protections

  repository_id = github_repository.microservice_repo.node_id
  pattern       = each.value.pattern

  enforce_admins         = each.value.enforce_admins
  require_signed_commits = each.value.require_signed_commits
  allows_force_pushes    = each.value.allow_force_pushes
  allows_deletions       = each.value.allow_deletions

  required_pull_request_reviews {
    dismiss_stale_reviews           = each.value.dismiss_stale_reviews
    require_code_owner_reviews      = each.value.require_code_owner_reviews
    required_approving_review_count = each.value.required_approving_review_count
  }

  required_status_checks {
    strict   = true
    contexts = each.value.required_status_checks
  }

  depends_on = [github_repository.microservice_repo]
}

# Standard repository labels
resource "github_issue_label" "labels" {
  for_each = local.standard_labels

  repository  = github_repository.microservice_repo.name
  name        = each.value.name
  color       = each.value.color
  description = each.value.description

  depends_on = [github_repository.microservice_repo]
}

