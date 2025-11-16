# AERAS Admin Dashboard

<div align="center">

![License](https://img.shields.io/badge/license-MIT-blue.svg)
![React](https://img.shields.io/badge/React-19.2.0-61DAFB?logo=react)
![TypeScript](https://img.shields.io/badge/TypeScript-5.9.3-3178C6?logo=typescript)
![Vite](https://img.shields.io/badge/Vite-7.2.2-646CFF?logo=vite)
![Mantine](https://img.shields.io/badge/Mantine-7.17.8-339AF0?logo=mantine)

A beautiful, modern, and highly functional admin dashboard for the AERAS e-rickshaw hailing system with real-time monitoring and analytics.

[Features](#features) • [Installation](#installation) • [Documentation](#documentation) • [Contributing](#contributing)

</div>

---

## ✨ Features

### 🎯 **Dashboard**
- Real-time system statistics (Active Rides, Online Pullers, Total Rides Today, Pending Reviews)
- Live map showing puller locations and active rides
- Recent activity feed with real-time updates

### 🚗 **Rides Management**
- Comprehensive rides table with filtering and pagination
- Real-time ride status updates via WebSocket
- Expandable rows showing detailed ride information
- Ability to manually adjust points for rides
- Filter by status (Requested, Accepted, Picked Up, Completed, Cancelled)
- Search by Ride ID or Puller Name

### 👥 **Pullers Management**
- List of all registered pullers
- Real-time online/offline status
- View puller points balance and total rides
- Adjust puller points with reason tracking
- Suspend puller accounts
- View ride history for each puller
- Search by name or phone number

### 📊 **Analytics**
- **Rides Over Time:** Line chart showing rides for the last 30 days
- **Popular Destinations:** Horizontal bar chart of most requested destinations
- **Peak Hours:** Bar chart showing average rides per hour of the day
- **Puller Leaderboard:** Top 10 pullers by points and rides this month

## 🛠️ Tech Stack

| Category | Technology |
|----------|-----------|
| **Framework** | React 19 with TypeScript |
| **Build Tool** | Vite 7 |
| **UI Library** | Mantine UI v7 (dark theme) |
| **State Management** | TanStack Query (React Query) |
| **Real-time** | Socket.IO Client |
| **Routing** | React Router DOM v7 |
| **Charts** | Recharts |
| **Maps** | Leaflet with React-Leaflet |
| **Date Handling** | Day.js |

## 🚀 Quick Start

### Prerequisites

- Node.js 18+ or Bun
- pnpm (recommended) or npm
- Backend API running (see [backend documentation](../backend/README.md))

### Installation

1. **Clone the repository:**
```bash
git clone https://github.com/yourusername/iotrix.git
cd iotrix/admin-panel
```

2. **Install dependencies:**
```bash
pnpm install
```

3. **Set up environment variables:**
```bash
cp .env.example .env
```

Update `.env` with your backend URL:
```env
VITE_API_URL=http://localhost:3000/api/v1
VITE_SOCKET_URL=http://localhost:3000
```

4. **Start development server:**
```bash
pnpm dev
```

The application will be available at `http://localhost:5173`

### Build for Production

```bash
pnpm build
pnpm preview
```

## 📁 Project Structure

```
src/
├── components/          # Reusable UI components
│   ├── StatCard.tsx
│   ├── LiveRidesMap.tsx
│   └── RecentActivityFeed.tsx
├── layouts/            # Layout components
│   ├── MainLayout.tsx
│   └── MainLayout.module.css
├── pages/              # Page components
│   ├── DashboardPage.tsx
│   ├── RidesPage.tsx
│   ├── PullersPage.tsx
│   ├── AnalyticsPage.tsx
│   └── SettingsPage.tsx
├── services/           # API and WebSocket services
│   ├── api.service.ts
│   └── socket.service.ts
├── hooks/              # Custom React hooks
│   └── useSocket.ts
├── types/              # TypeScript type definitions
│   └── index.ts
├── App.tsx             # Main app component with routing
└── main.tsx            # Application entry point
```

## API Integration

The dashboard expects the following endpoints from the backend:

## 📡 API Integration

The dashboard integrates with the backend API. See [BACKEND_INTEGRATION.md](BACKEND_INTEGRATION.md) for complete details.

### Key Endpoints

**System Stats:** `GET /api/v1/stats`

**Rides:** `GET /api/v1/rides`, `PATCH /api/v1/rides/:id/points`

**Pullers:** `GET /api/v1/pullers`, `PATCH /api/v1/pullers/:id/points`

**Analytics:** `GET /api/v1/analytics/*`

### WebSocket Events

- `ride_update` - Real-time ride status updates
- `puller_update` - Puller information updates
- `stats_update` - System statistics updates
- `activity_event` - Activity feed events
- `location_update` - Puller location updates

## 🎨 Customization

### Theme

The dashboard uses a dark-first theme. Customize in `src/App.tsx`:

- Background: `#1A1B1E`
- Primary: Blue (`#228BE6`)
- Accent: Cyan (`#15AABF`)

### Features

- ✅ Real-time updates with automatic reconnection
- ✅ Optimistic UI updates
- ✅ Responsive design (mobile, tablet, desktop)
- ✅ Toast notifications
- ✅ Loading states and error handling
- ✅ React Query cache management
- ✅ Code splitting ready

## 📖 Documentation

- [Backend Integration Guide](BACKEND_INTEGRATION.md) - API endpoints and WebSocket events
- [Contributing Guidelines](CONTRIBUTING.md) - How to contribute to the project

## 🤝 Contributing

We welcome contributions! Please see [CONTRIBUTING.md](CONTRIBUTING.md) for details on:

- Code of conduct
- Development setup
- Coding guidelines
- Pull request process

## 📝 License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## 🙏 Acknowledgments

Built with:
- [React](https://react.dev/)
- [Mantine UI](https://mantine.dev/)
- [TanStack Query](https://tanstack.com/query)
- [Socket.IO](https://socket.io/)
- [Vite](https://vitejs.dev/)

---

<div align="center">
Made with ❤️ for AERAS
</div>
