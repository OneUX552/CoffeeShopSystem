# Artisan Coffee Shop Management System

A production-grade, modular Coffee Shop Point of Sale (POS), inventory recipe tracking, and customer loyalty management system written in ISO C11.

[![C11](https://img.shields.io/badge/C-11-blue.svg)](https://en.wikipedia.org/wiki/C11_(C_standard_revision))
[![Build](https://img.shields.io/badge/build-passing-brightgreen.svg)]()
[![Tests](https://img.shields.io/badge/tests-45%20passed-success.svg)]()

---

## Features

- ☕ **Menu Catalog**: Full catalog supporting hot coffees, iced brews, teas, bakery pastries, and merchandise.
- 📦 **Recipe & Raw Inventory Depletion**: Automatic deduction of coffee beans (grams), milk (ml), syrups, and cups per order recipe.
- 💳 **Multi-Payment Checkout**: Cash tender with change calculation, Credit/Debit card simulation, and Mobile NFC pay.
- 🏷️ **Loyalty & Tier Discounts**: Phone-number loyalty rewards ($1 = 1 point) with Bronze, Silver (5% off), and Gold (10% off) tiers.
- 🎟️ **Promo Coupon Engine**: Percentage discount verification with minimum spend checks (e.g., `WELCOME10`, `VIP20`).
- 🧾 **ASCII Receipt Printing**: Clean printable formatted customer receipts.
- 📊 **Business Analytics**: Daily gross revenue, average order value, payment method breakdown, and audit logging.
- 🔐 **Role-Based Access Control**: Cashier / Barista portal vs. Administrator console with masked password authentication.
- 💾 **CSV File Persistence**: Zero external DB dependencies; auto-loads and auto-saves to readable CSV data files.
- 🧪 **Automated Test Suite**: 45 unit and integration tests verifying pricing, stock consumption, loyalty, and auth.

---

## Project Structure

```
CoffeeShopSystem/
├── Makefile                # Build targets: make, make test, make clean, make run
├── README.md               # Project documentation
├── docs/
│   └── STUDY_GUIDE.md      # Detailed architectural study guide
├── include/
│   ├── models.h            # Core data structures and domain enums
│   ├── utils.h             # Safe I/O, string trimming, currency, ANSI colors
│   ├── menu.h              # Menu catalog management headers
│   ├── inventory.h         # Ingredient stock & recipe consumption headers
│   ├── order.h             # Cart operations, pricing, and tax calculator
│   ├── loyalty.h           # Customer loyalty points, tiers, and coupon validation
│   ├── payment.h           # Cash, card, mobile payment, and receipt generation
│   ├── storage.h           # CSV data serialization and transaction logging
│   ├── analytics.h         # Sales reporting and executive analytics
│   ├── auth.h              # Role-based access control (Admin / Cashier)
│   └── ui.h                # Interactive console dashboards and menus
├── src/
│   ├── utils.c             # Terminal utilities and safe input
│   ├── menu.c              # In-memory catalog CRUD
│   ├── inventory.c         # Stock tracking and recipe deduction
│   ├── order.c             # Order cart and billing
│   ├── loyalty.c           # Customer loyalty and coupon system
│   ├── payment.c           # Payment processing and receipt formatting
│   ├── storage.c           # CSV file I/O and audit logging
│   ├── analytics.c         # Revenue calculation and analytics reporting
│   ├── auth.c              # Authentication and credentials
│   ├── ui.c                # Interactive POS and admin dashboards
│   └── main.c              # Application entrypoint and loop
├── tests/
│   └── test_runner.c       # Comprehensive unit test suite (45 tests)
└── data/
    ├── menu.csv            # Menu catalog data
    ├── inventory.csv       # Raw stock ingredients data
    ├── customers.csv       # Customer loyalty database
    ├── coupons.csv         # Active promotional coupons
    └── transactions.log    # Append-only sales audit trail
```

---

## Quick Start

### 1. Build
```bash
make
```

### 2. Run Application
```bash
./coffeeshop
```

**Default Demo Staff Credentials:**
| Role | Username | Password |
|---|---|---|
| **Administrator** | `admin` | `admin123` |
| **Barista / Cashier** | `barista` | `coffee123` |

### 3. Run Automated Tests
```bash
make test
```

---

## Co-Authors
- Sameer Al Sahab (`jahirmondol5456@gmail.com`)
- OneUX552 (`OneUX552@users.noreply.github.com`)

For complete system design details, see [docs/STUDY_GUIDE.md](docs/STUDY_GUIDE.md).
