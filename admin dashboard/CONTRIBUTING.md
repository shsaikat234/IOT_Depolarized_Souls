# Contributing to AERAS Admin Panel

Thank you for your interest in contributing to the AERAS Admin Panel! We welcome contributions from the community.

## Table of Contents

- [Code of Conduct](#code-of-conduct)
- [Getting Started](#getting-started)
- [Development Setup](#development-setup)
- [How to Contribute](#how-to-contribute)
- [Coding Guidelines](#coding-guidelines)
- [Pull Request Process](#pull-request-process)
- [Reporting Bugs](#reporting-bugs)
- [Feature Requests](#feature-requests)

## Code of Conduct

This project adheres to a code of conduct. By participating, you are expected to uphold this code. Please be respectful and constructive in all interactions.

## Getting Started

1. Fork the repository
2. Clone your fork: `git clone https://github.com/YOUR_USERNAME/admin-panel.git`
3. Add upstream remote: `git remote add upstream https://github.com/ORIGINAL_OWNER/admin-panel.git`
4. Create a new branch: `git checkout -b feature/your-feature-name`

## Development Setup

1. Install dependencies:
   ```bash
   pnpm install
   ```

2. Copy environment variables:
   ```bash
   cp .env.example .env
   ```

3. Start the development server:
   ```bash
   pnpm dev
   ```

4. Make sure the backend is running (see backend documentation)

## How to Contribute

### Types of Contributions

- **Bug fixes**: Fix issues and improve stability
- **New features**: Add new functionality
- **Documentation**: Improve or add documentation
- **Performance**: Optimize code and improve performance
- **UI/UX**: Enhance user interface and experience
- **Tests**: Add or improve test coverage

### Contribution Workflow

1. **Find or create an issue**: Before starting work, check if an issue exists or create one
2. **Assign yourself**: Comment on the issue to let others know you're working on it
3. **Create a branch**: Use descriptive branch names (e.g., `fix/login-error`, `feature/dark-mode`)
4. **Make changes**: Follow coding guidelines and keep commits atomic
5. **Test thoroughly**: Ensure your changes work as expected
6. **Submit a PR**: Create a pull request with a clear description

## Coding Guidelines

### TypeScript

- Use TypeScript for all new code
- Define proper types and interfaces
- Avoid using `any` type
- Use meaningful variable and function names

### React

- Use functional components with hooks
- Keep components small and focused
- Use proper prop types
- Implement proper error boundaries

### Code Style

- Follow the existing code style
- Use ESLint and fix all linting errors: `pnpm lint`
- Format code before committing
- Use meaningful commit messages

### Commit Messages

Follow the conventional commits specification:

```
feat: add user profile page
fix: resolve login authentication issue
docs: update installation instructions
style: format code with prettier
refactor: simplify analytics service
test: add tests for ride management
chore: update dependencies
```

### File Organization

- Components in `src/components/`
- Pages in `src/pages/`
- Services in `src/services/`
- Types in `src/types/`
- Utilities in `src/utils/`
- Hooks in `src/hooks/`

## Pull Request Process

1. **Update documentation**: If you've added features, update the README
2. **Test your changes**: Ensure everything works correctly
3. **Update the changelog**: Add your changes to CHANGELOG.md
4. **Create the PR**: 
   - Use a clear, descriptive title
   - Reference related issues
   - Describe what changed and why
   - Add screenshots for UI changes

5. **Code review**: 
   - Address review comments promptly
   - Keep discussions constructive
   - Make requested changes

6. **Merge**: Once approved, a maintainer will merge your PR

## Reporting Bugs

When reporting bugs, include:

- **Clear title**: Summarize the issue
- **Description**: Detailed explanation of the bug
- **Steps to reproduce**: List steps to recreate the issue
- **Expected behavior**: What should happen
- **Actual behavior**: What actually happens
- **Environment**: Browser, OS, Node version
- **Screenshots**: If applicable
- **Error messages**: Console errors or stack traces

## Feature Requests

For new features:

- **Clear description**: Explain the feature and its benefits
- **Use cases**: Describe how it would be used
- **Mockups**: Include UI mockups if applicable
- **Alternatives**: Mention alternative solutions you've considered

## Questions?

If you have questions:

- Check the [README](README.md) and documentation
- Search existing issues
- Create a new issue with the "question" label

## License

By contributing, you agree that your contributions will be licensed under the MIT License.

---

Thank you for contributing to AERAS Admin Panel! 🚀
