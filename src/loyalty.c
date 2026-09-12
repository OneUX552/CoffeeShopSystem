#include "loyalty.h"
#include "utils.h"
#include <stdio.h>
#include <string.h>

static Customer s_customers[MAX_CUSTOMERS];
static int s_customer_count = 0;

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
