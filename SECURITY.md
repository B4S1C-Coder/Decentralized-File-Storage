# Security Policy

## Supported Versions

The project is under development and is in the pre alpha stages. Below would be the target versions for security updates. However, vulnerability reports and security advice is welcome.

| Version | Supported          |
| ------- | ------------------ |
| 1.x.x   | ✅                 |
| 0.x.x   | ❌                 |

## Reporting a Vulnerability

If you discover a security vulnerability, please follow the steps below:

1. **Do Not Open a Public Issue:** Please do not publicly disclose the vulnerability through GitHub issues. Instead, report it directly as described below.
   
2. **Contact:** Report the vulnerability by emailing at `saksham2005mittal@gmail.com` with the subject "dFSN - Security Vulnerability Report".

   Include the following details in your email:
   - A detailed description of the vulnerability.
   - Steps to reproduce the issue.
   - Any potential fixes or mitigations you are aware of.

3. **Response Time:** As of now I am a solo developer working on this project and would try my best to respond at the earliest.

## Security Features in the Repository

This project is a decentralized file storage network designed with the following key security principles:

1. **Encryption:** 
   - All file chunks are encrypted before being stored on the storage nodes. The encryption key is stored locally in the user's SQLite database.

2. **Ownership Tokens:**
   - Each chunk of data is associated with an ownership token to prevent unauthorized access.

3. **Secure Communication:**
   - Communication between components (e.g., user client, tracking server, storage nodes) is implemented using gRPC with appropriate encryption.

4. **Node Authentication:**
   - Storage nodes and tracking servers authenticate themselves using heartbeat signals to maintain a trusted network.

5. **Isolation of Sensitive Data:**
   - Sensitive data, including encryption keys, is never stored on the storage nodes or tracking servers, ensuring data privacy.

## Security Best Practices

To ensure the security of your setup, please follow these recommendations:
- Regularly update your tracking server and storage node software to the latest version.
- Enable automatic updates for dependencies in the `CMakeLists.txt` files to include patched versions.
- Periodically audit your SQLite database for integrity and backup encrypted data securely.

## Contributing to Security

Contributions to improve the security of this project are most welcome. Please:
- Follow the development guidelines in `CONTRIBUTING.md`.
- Propose security enhancements by opening a pull request or contacting via email.

Thank you for this project secure!
