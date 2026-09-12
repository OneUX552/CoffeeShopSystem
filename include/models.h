#ifndef MODELS_H
#define MODELS_H

#include <stdbool.h>

#define MAX_NAME_LEN 64
#define MAX_DESC_LEN 128
#define MAX_CODE_LEN 24
#define MAX_PHONE_LEN 20
#define MAX_ITEMS_IN_ORDER 32
#define MAX_RECIPE_ITEMS 8
#define MAX_MENU_ITEMS 64
#define MAX_INVENTORY_ITEMS 64
#define MAX_CUSTOMERS 128
#define MAX_COUPONS 32
#define MAX_USERS 16

#define SALES_TAX_RATE 0.0825 // 8.25% standard sales tax

typedef enum {
    CAT_HOT_COFFEE = 0,
    CAT_ICED_COFFEE,
    CAT_TEA,
    CAT_PASTRY,
    CAT_MERCH
} ItemCategory;

typedef enum {
    SIZE_REGULAR = 0,
    SIZE_LARGE
} DrinkSize;

typedef enum {
    PAYMENT_CASH = 0,
    PAYMENT_CARD,
    PAYMENT_MOBILE
} PaymentMethod;

typedef enum {
    ORDER_PENDING = 0,
    ORDER_COMPLETED,
    ORDER_CANCELLED
} OrderStatus;

typedef enum {
    ROLE_CASHIER = 0,
    ROLE_ADMIN
} UserRole;

typedef enum {
    TIER_BRONZE = 0, // 0-99 points (0% tier discount)
    TIER_SILVER,     // 100-249 points (5% tier discount)
    TIER_GOLD        // 250+ points (10% tier discount)
} LoyaltyTier;

// Ingredient requirement for a menu recipe
typedef struct {
    int inventory_id;
    double quantity; // amount needed per drink serving
} RecipeItem;

// Menu item
typedef struct {
    int id;
    char name[MAX_NAME_LEN];
    ItemCategory category;
    double base_price;
    char description[MAX_DESC_LEN];
    RecipeItem recipe[MAX_RECIPE_ITEMS];
    int recipe_count;
    bool available;
} MenuItem;

// Inventory item
typedef struct {
    int id;
    char name[MAX_NAME_LEN];
    char unit[16];        // e.g. "g", "ml", "shots", "pcs"
    double current_stock;
    double min_threshold; // trigger low-stock warning
    double unit_cost;     // purchase cost
} InventoryItem;

// Line item within an order
typedef struct {
    int menu_item_id;
    char item_name[MAX_NAME_LEN];
    double unit_price;
    int quantity;
    double line_total;
} OrderLineItem;

// Customer Order
typedef struct {
    int order_id;
    char cashier_username[MAX_NAME_LEN];
    char customer_phone[MAX_PHONE_LEN];
    OrderLineItem items[MAX_ITEMS_IN_ORDER];
    int item_count;
    double subtotal;
    double discount_amount;
    double tax_amount;
    double total_amount;
    PaymentMethod payment_method;
    double cash_tendered;
    double change_due;
    OrderStatus status;
    char timestamp[32];
} Order;

// Customer loyalty profile
typedef struct {
    char phone[MAX_PHONE_LEN];
    char name[MAX_NAME_LEN];
    int points;
    LoyaltyTier tier;
    double total_spent;
    int visit_count;
} Customer;

// Coupon discount code
typedef struct {
    char code[MAX_CODE_LEN];
    double discount_percent; // e.g. 15.0 for 15%
    double min_order_value;
    bool active;
} Coupon;

// System user / staff
typedef struct {
    char username[MAX_NAME_LEN];
    char password[MAX_NAME_LEN];
    char display_name[MAX_NAME_LEN];
    UserRole role;
    bool active;
} User;

#endif // MODELS_H
