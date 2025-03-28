
variable "github_token" {
  description = "GitHub Personal Access Token with permissions to manage repositories"
  type        = string
  sensitive   = true
}

variable "repository_name" {
  description = "Name of the GitHub repository to create or manage"
  type        = string
}

variable "organization_name" {
  description = "Name of the GitHub organization where the repository will be created"
  type        = string
  default     = null
}

variable "repository_visibility" {
  description = "Visibility of the repository (public, private, or internal)"
  type        = string
  default     = "private"
  
  validation {
    condition     = contains(["public", "private", "internal"], var.repository_visibility)
    error_message = "Repository visibility must be one of: public, private, or internal."
  }
}

variable "repository_description" {
  description = "Description of the repository"
  type        = string
  default     = ""
}

variable "enable_advanced_security" {
  description = "Enable GitHub Advanced Security features for the repository"
  type        = bool
  default     = true
}

variable "enable_secret_scanning" {
  description = "Enable GitHub Secret Scanning for the repository"
  type        = bool
  default     = true
}

variable "branch_protections" {
  description = "Map of branch protection rules to apply to the repository"
  type = map(object({
    pattern                         = string
    enforce_admins                  = bool
    require_signed_commits          = bool
    required_status_checks          = list(string)
    required_approving_review_count = number
    dismiss_stale_reviews           = bool
    require_code_owner_reviews      = bool
    allow_force_pushes              = bool
    allow_deletions                 = bool
  }))
  default = {}
}

variable "default_branch" {
  description = "Default branch of the repository"
  type        = string
  default     = "main"
}

variable "environments" {
  description = "List of environments to create in the repository"
  type = map(object({
    wait_timer                    = number
    prevent_self_review           = bool
    require_reviewers             = bool
    reviewers                     = list(string)
    deployment_branch_policy_type = string
    branch_pattern                = optional(string)
  }))
  default = {
    development = {
      wait_timer                    = 0
      prevent_self_review           = false
      require_reviewers             = false
      reviewers                     = []
      deployment_branch_policy_type = "all"
    },
    staging = {
      wait_timer                    = 10
      prevent_self_review           = true
      require_reviewers             = true
      reviewers                     = []
      deployment_branch_policy_type = "selected"
      branch_pattern                = "main"
    },
    production = {
      wait_timer                    = 30
      prevent_self_review           = true
      require_reviewers             = true
      reviewers                     = []
      deployment_branch_policy_type = "selected"
      branch_pattern                = "main"
    }
  }
}

