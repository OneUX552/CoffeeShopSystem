#include "loyalty.h"
#include "utils.h"
#include <stdio.h>
#include <string.h>
#include <math.h>

static Customer s_customers[MAX_CUSTOMERS];
static int s_customer_count = 0;

static Coupon s_coupons[MAX_COUPONS];
static int s_coupon_count = 0;

void loyalty_init(void) {
    s_customer_count = 0;
    memset(s_customers, 0, sizeof(s_customers));
}

int loyalty_get_customer_count(void) {
    return s_customer_count;
}

const Customer *loyalty_get_customers(int *out_count) {
    if (out_count) {
        *out_count = s_customer_count;
    }
    return s_customers;
}

Customer *loyalty_find_customer(const char *phone) {
    if (!phone) return NULL;
    for (int i = 0; i < s_customer_count; i++) {
        if (strcmp(s_customers[i].phone, phone) == 0) {
            return &s_customers[i];
        }
    }
    return NULL;
}

LoyaltyTier loyalty_calculate_tier(int points) {
    if (points >= 250) return TIER_GOLD;
    if (points >= 100) return TIER_SILVER;
    return TIER_BRONZE;
}

double loyalty_get_tier_discount_rate(LoyaltyTier tier) {
    switch (tier) {
        case TIER_GOLD:   return 0.10; // 10% off
        case TIER_SILVER: return 0.05; // 5% off
        case TIER_BRONZE:
        default:          return 0.0;  // 0% off
    }
}

const char *loyalty_tier_to_string(LoyaltyTier tier) {
    switch (tier) {
        case TIER_GOLD:   return "Gold (10% off)";
        case TIER_SILVER: return "Silver (5% off)";
        case TIER_BRONZE:
        default:          return "Bronze (Standard)";
    }
}

bool loyalty_register_customer(const char *phone, const char *name) {
    if (!phone || !name || s_customer_count >= MAX_CUSTOMERS) {
        return false;
    }
    if (loyalty_find_customer(phone) != NULL) {
        return false;
    }
    Customer *cust = &s_customers[s_customer_count];
    strncpy(cust->phone, phone, MAX_PHONE_LEN - 1);
    strncpy(cust->name, name, MAX_NAME_LEN - 1);
    cust->points = 10; // 10 welcome points bonus
    cust->tier = loyalty_calculate_tier(cust->points);
    cust->total_spent = 0.0;
    cust->visit_count = 0;

    s_customer_count++;
    return true;
}

void loyalty_award_points(Customer *customer, double amount_spent) {
    if (!customer || amount_spent <= 0.0) return;
    int points_earned = (int)amount_spent; // 1 point per $1 spent
    customer->points += points_earned;
    customer->total_spent += amount_spent;
    customer->visit_count += 1;
    customer->tier = loyalty_calculate_tier(customer->points);
}

void loyalty_print_customers(void) {
    printf("\n" ANSI_BOLD ANSI_CYAN "%-16s %-20s %-8s %-20s %-12s %-6s" ANSI_RESET "\n",
           "Phone", "Name", "Points", "Tier", "Total Spent", "Visits");
    printf("------------------------------------------------------------------------------------\n");
    for (int i = 0; i < s_customer_count; i++) {
        const Customer *c = &s_customers[i];
        char spent_buf[16];
        utils_format_currency(c->total_spent, spent_buf, sizeof(spent_buf));

        printf("%-16s %-20s %-8d %-20s %-12s %-6d\n",
               c->phone,
               c->name,
               c->points,
               loyalty_tier_to_string(c->tier),
               spent_buf,
               c->visit_count);
    }
    printf("------------------------------------------------------------------------------------\n");
}

void coupon_init(void) {
    s_coupon_count = 0;
    memset(s_coupons, 0, sizeof(s_coupons));
}

int coupon_get_count(void) {
    return s_coupon_count;
}

const Coupon *coupon_get_all(int *out_count) {
    if (out_count) {
        *out_count = s_coupon_count;
    }
    return s_coupons;
}

bool coupon_add(const Coupon *coupon) {
    if (!coupon || s_coupon_count >= MAX_COUPONS) {
        return false;
    }
    if (coupon_find(coupon->code) != NULL) {
        return false;
    }
    s_coupons[s_coupon_count] = *coupon;
    s_coupon_count++;
    return true;
}

const Coupon *coupon_find(const char *code) {
    if (!code) return NULL;
    for (int i = 0; i < s_coupon_count; i++) {
        if (utils_strcasecmp(s_coupons[i].code, code) == 0) {
            return &s_coupons[i];
        }
    }
    return NULL;
}

double coupon_validate_and_apply(const char *code, double subtotal, char *err_msg, size_t err_size) {
    if (!code || strlen(code) == 0) {
        if (err_msg && err_size > 0) snprintf(err_msg, err_size, "No promo code provided.");
        return 0.0;
    }
    const Coupon *c = coupon_find(code);
    if (!c) {
        if (err_msg && err_size > 0) snprintf(err_msg, err_size, "Invalid coupon code '%s'.", code);
        return 0.0;
    }
    if (!c->active) {
        if (err_msg && err_size > 0) snprintf(err_msg, err_size, "Coupon '%s' has expired.", code);
        return 0.0;
    }
    if (subtotal < c->min_order_value) {
        if (err_msg && err_size > 0) snprintf(err_msg, err_size, "Coupon requires minimum spend of $%.2f.", c->min_order_value);
        return 0.0;
    }

    // Calculate and round discount to nearest cent
    double discount = ((int)((subtotal * (c->discount_percent / 100.0) * 100.0) + 0.5)) / 100.0;
    if (discount > subtotal) {
        discount = subtotal;
    }
    if (err_msg && err_size > 0) {
        snprintf(err_msg, err_size, "Promo code '%s' applied: %.0f%% off!", c->code, c->discount_percent);
    }
    return discount;
}

void coupon_print_all(void) {
    printf("\n" ANSI_BOLD ANSI_CYAN "%-16s %-16s %-16s %-10s" ANSI_RESET "\n",
           "Coupon Code", "Discount (%)", "Min Order ($)", "Status");
    printf("------------------------------------------------------------\n");
    for (int i = 0; i < s_coupon_count; i++) {
        const Coupon *c = &s_coupons[i];
        printf("%-16s %-16.1f $%-15.2f %-10s\n",
               c->code,
               c->discount_percent,
               c->min_order_value,
               c->active ? (ANSI_GREEN "Active" ANSI_RESET) : (ANSI_RED "Inactive" ANSI_RESET));
    }
    printf("------------------------------------------------------------\n");
}
