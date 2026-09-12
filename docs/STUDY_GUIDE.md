# Coffee Shop System in C — Complete Study Guide

Welcome to the **Coffee Shop System** study guide! This document explains how this complete terminal-based POS (Point of Sale), inventory management, and customer loyalty system was designed and constructed in C from scratch.

---

## 1. Architectural Overview

The system follows a modular architecture separating concerns into independent layers:

```
+-------------------------------------------------------------------+
|                           User Interface                          |
|                       (src/ui.c, src/main.c)                      |
+---------------------------------+---------------------------------+
                                  |
            +---------------------+---------------------+
            |                     |                     |
+-----------v-----------+ +-------v---------+ +---------v-----------+
|    Menu Catalog       | | Order & Billing | | Customer Loyalty    |
|    (src/menu.c)       | | (src/order.c)   | | & Promotional Codes |
+-----------+-----------+ +-------+---------+ | (src/loyalty.c)     |
            |                     |           +---------+-----------+
            |                     |                     |
+-----------v-----------+ +-------v---------+           |
| Raw Stock Inventory   | | Payment & Change|           |
| (src/inventory.c)     | | (src/payment.c) |           |
+-----------+-----------+ +-------+---------+           |
            |                     |                     |
            +---------------------+---------------------+
                                  |
                     +------------v------------+
                     | Storage & Audit Logging |
                     | (src/storage.c, CSVs)   |
                     +-------------------------+
```

---

## 2. Step-by-Step Module Walkthrough

### 2.1 Core Domain Models (`include/models.h`)
- **`MenuItem`**: Represents consumable beverages and merchandise. Contains base price, item category (`CAT_HOT_COFFEE`, `CAT_ICED_COFFEE`, `CAT_TEA`, `CAT_PASTRY`, `CAT_MERCH`), availability status, and an array of `RecipeItem` components.
- **`InventoryItem`**: Tracks stock of raw consumables (beans in grams, milk in milliliters, paper cups in pieces) with minimum reorder thresholds.
- **`OrderLineItem` & `Order`**: Maintains cart items, item multipliers, subtotal, discount, sales tax (8.25%), total, payment details, and timestamps.
- **`Customer`**: Stores phone number (primary identifier), accumulated loyalty points, tiered status (`Bronze`, `Silver`, `Gold`), total lifetime spend, and visit count.
- **`Coupon`**: Promotional codes offering percentage discounts with spend requirements (e.g. `WELCOME10`, `VIP20`).

### 2.2 Inventory & Recipe Depletion (`src/inventory.c`)
Real-world coffee shops don't just sell "drinks" — they consume raw ingredients:
- A double shot espresso consumes `18.0g` of coffee beans and 1 paper cup.
- A latte consumes `18.0g` of beans, `220ml` of milk, and 1 paper cup.
Before an order item can be added to the cart, `inventory_can_prepare_item()` validates that sufficient ingredients exist in stock. When payment completes, `inventory_consume_for_item()` automatically reduces the raw ingredient levels.

### 2.3 Order Lifecycle & Dynamic Pricing (`src/order.c`)
1. **Creation**: Initialized with unique auto-incrementing ID, cashier ID, and optional customer phone.
2. **Item Addition**: Duplicate item entries are merged seamlessly into existing line item quantities.
3. **Subtotal Calculation**: `sum(unit_price * quantity)`.
4. **Discount Calculation**: Applies loyalty tier discounts (Silver = 5%, Gold = 10%) combined with validated promo coupons, capped at the subtotal.
5. **Tax Calculation**: Standard 8.25% sales tax on the post-discount taxable amount.

### 2.4 Customer Loyalty & Coupon Engine (`src/loyalty.c`)
- Every dollar spent earns 1 loyalty point.
- **Tiers**:
  - **Bronze** (0–99 pts): Standard pricing.
  - **Silver** (100–249 pts): 5% automatic discount on all orders.
  - **Gold** (250+ pts): 10% automatic discount on all orders.
- Welcome bonus: 10 complimentary points upon enrollment.

### 2.5 Payment & Receipt Printing (`src/payment.c`)
- Supports **Cash** (with cash tendered validation and accurate change calculation), **Credit/Debit Card** simulation, and **Mobile Contactless Pay**.
- Renders a clean ASCII receipt with store branding, itemized order rows, tax breakdown, and footer greeting.

### 2.6 Persistence Layer (`src/storage.c`)
- Flat-file CSV storage format for human-readability and zero external database dependencies.
- Append-only audit logger writes pipe-delimited records to `data/transactions.log` for business auditing.
- Fallback default seeder initializes sample menus and ingredients on a fresh setup.

### 2.7 Role-Based Access Control (`src/auth.c`)
- **Cashier (`barista`)**: Walk-in POS checkout, view catalog, view stock levels.
- **Administrator (`admin`)**: Access to the management console: add menu items, restock ingredients, manage customers, view analytics reports, and view audit transaction logs.

---

## 3. How to Compile and Run

```bash
# Compile application
make

# Run the interactive system
./coffeeshop

# Run the automated test suite (45 unit tests)
make test

# Clean object and binary artifacts
make clean
```
