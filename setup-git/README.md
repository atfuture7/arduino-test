# git access
I'd like to use token to update project. 

1. Generating a Personal Access Token (if you don't have one):
Navigate to your platform's settings (e.g., GitHub, GitLab).
Find "Developer settings" or "Personal access tokens."
Generate a new token, giving it a descriptive name and setting an expiration date.

2. Embedding the token in the remote URL (less secure for sensitive tokens):
You can modify your remote URL to include the token. This is generally discouraged for long-term use, especially if the token has broad permissions, as it exposes the token in your .git/config file.

in git-bash
```
git remote set-url origin https://<username>:<your_token>@github.com/<username>/<repository_name>.git
```
