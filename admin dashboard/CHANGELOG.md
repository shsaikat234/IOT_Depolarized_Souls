# Changelog

All notable changes to the AERAS Admin Panel will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [1.0.0] - 2025-11-15

### Added

#### Dashboard
- Real-time system statistics display (Active Rides, Online Pullers, Total Rides Today, Pending Reviews)
- Interactive live map with Leaflet showing puller locations and active rides
- Real-time activity feed showing the last 10 events
- WebSocket integration for live data updates

#### Rides Management
- Comprehensive rides table with server-side pagination
- Advanced filtering by status, date range, and search
- Expandable rows showing detailed ride timeline and information
- Manual points adjustment capability with reason tracking
- Real-time ride status updates via WebSocket
- Color-coded status badges for easy identification

#### Pullers Management
- Searchable puller directory with pagination
- Real-time online/offline status indicators
- Points balance display and management
- Points adjustment modal with reason tracking
- Account suspension functionality with reason logging
- Ride history viewer for individual pullers
- Search by name or phone number

#### Analytics
- Rides over time line chart (30-day historical data)
- Popular destinations horizontal bar chart
- Peak hours analysis bar chart
- Top pullers leaderboard with ranking badges
- Monthly performance metrics

#### Settings
- Settings page placeholder for future configurations

#### UI/UX
- Dark theme with professional color scheme (#1A1B1E background)
- Fully responsive design (mobile, tablet, desktop)
- Smooth animations and transitions
- Toast notifications for user feedback
- Loading states with skeleton loaders
- Comprehensive error handling and fallbacks
- Keyboard navigation support

#### Technical Features
- TypeScript for type safety
- React Query for efficient server state management
- Socket.IO integration for real-time updates
- Axios for API calls
- Mantine UI v7 component library
- React Router DOM v7 for navigation
- ESLint for code quality
- Vite for fast development and builds

### Infrastructure
- Environment variable configuration
- Development and production build scripts
- Professional project structure
- Code organization with services, hooks, and contexts

## [Unreleased]

### Planned
- User authentication improvements
- Advanced analytics dashboard
- Export functionality for reports
- Notification preferences
- Multi-language support
- Dark/Light theme toggle
- Enhanced map features
- Mobile app companion

---

For a detailed list of all changes, see the [commit history](https://github.com/yourusername/iotrix/commits/main/admin-panel).
