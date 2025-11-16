# Security Policy

## Supported Versions

We release patches for security vulnerabilities in the following versions:

| Version | Supported          |
| ------- | ------------------ |
| 1.0.x   | :white_check_mark: |
| < 1.0   | :x:                |

## Reporting a Vulnerability

We take the security of AERAS Admin Panel seriously. If you believe you have found a security vulnerability, please report it to us as described below.

### Please Do NOT:

- Open a public GitHub issue
- Discuss the vulnerability in public forums or social media

### Please DO:

1. **Email us directly** at security@aeras.example.com (or create a private security advisory on GitHub)
2. **Provide detailed information** including:
   - Description of the vulnerability
   - Steps to reproduce
   - Potential impact
   - Suggested fix (if any)
3. **Allow time for us to respond** - We aim to respond within 48 hours

### What to Expect:

- We will acknowledge receipt of your vulnerability report within 48 hours
- We will provide an estimated timeline for a fix
- We will notify you when the vulnerability is fixed
- We will publicly acknowledge your responsible disclosure (unless you prefer to remain anonymous)

## Security Best Practices

When deploying this application:

1. **Environment Variables**: Never commit `.env` files to version control
2. **HTTPS**: Always use HTTPS in production
3. **Authentication**: Ensure proper authentication is implemented
4. **Dependencies**: Regularly update dependencies to patch known vulnerabilities
5. **CORS**: Configure CORS properly for your backend API
6. **CSP**: Implement Content Security Policy headers
7. **Rate Limiting**: Implement rate limiting on the backend API

## Security Updates

Security updates will be released as patch versions (e.g., 1.0.1, 1.0.2) and documented in the [CHANGELOG](CHANGELOG.md).

## Vulnerability Disclosure Process

1. **Report received** - We acknowledge your report
2. **Investigation** - We investigate and validate the issue
3. **Fix development** - We develop and test a fix
4. **Release** - We release a patch version
5. **Disclosure** - We publish a security advisory with details

Thank you for helping keep AERAS Admin Panel and its users safe!
