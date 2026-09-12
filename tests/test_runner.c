#include "models.h"
#include "utils.h"
#include "menu.h"
#include "inventory.h"
#include "order.h"
#include "loyalty.h"
#include "payment.h"
#include "storage.h"
#include "auth.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>

static int s_tests_run = 0;
static int s_tests_passed = 0;

#define TEST_ASSERT(cond, msg) do { \
    s_tests_run++; \
    if (cond) { \
        s_tests_passed++; \
        printf(ANSI_GREEN "  ✓ PASS: " ANSI_RESET "%s\n", msg); \
    } else { \
        printf(ANSI_BOLD ANSI_RED "  ✗ FAIL: " ANSI_RESET "%s (Line %d)\n", msg, __LINE__); \
    } \
} while(0)

static void test_menu_operations(void) {
    printf("\n" ANSI_BOLD "Testing Menu Operations:" ANSI_RESET "\n");
    menu_init();

    MenuItem item1 = {101, "Test Latte", CAT_HOT_COFFEE, 4.50, "Fresh latte", {{1, 18.0}}, 1, true};
    TEST_ASSERT(menu_add_item(&item1) == true, "Add valid menu item");
    TEST_ASSERT(menu_get_count() == 1, "Menu count is 1");

    const MenuItem *found = menu_find_by_id(101);
    TEST_ASSERT(found != NULL && strcmp(found->name, "Test Latte") == 0, "Find menu item by ID");

    TEST_ASSERT(menu_add_item(&item1) == false, "Duplicate ID rejected");

    TEST_ASSERT(menu_update_price(101, 5.00) == true, "Update item price");
    TEST_ASSERT(menu_find_by_id(101)->base_price == 5.00, "Price matches new value");

    TEST_ASSERT(menu_delete_item(101) == true, "Delete menu item");
    TEST_ASSERT(menu_get_count() == 0, "Menu count is 0 after delete");
}

static void test_inventory_and_recipes(void) {
    printf("\n" ANSI_BOLD "Testing Inventory and Recipe Consumption:" ANSI_RESET "\n");
    inventory_init();

    InventoryItem beans = {1, "Arabica Beans", "g", 100.0, 20.0, 0.05};
    InventoryItem milk = {2, "Fresh Milk", "ml", 500.0, 100.0, 0.01};
    TEST_ASSERT(inventory_add_item(&beans) == true, "Add beans to inventory");
    TEST_ASSERT(inventory_add_item(&milk) == true, "Add milk to inventory");

    TEST_ASSERT(inventory_restock(1, 50.0) == true, "Restock 50g beans");
    TEST_ASSERT(inventory_find_by_id(1)->current_stock == 150.0, "New beans stock is 150g");

    MenuItem test_drink = {
        201, "Double Cappuccino", CAT_HOT_COFFEE, 4.75, "Cappuccino",
        {{1, 20.0}, {2, 150.0}}, 2, true
    };

    TEST_ASSERT(inventory_can_prepare_item(&test_drink, 2) == true, "Can prepare 2 cappuccinos (needs 40g beans, 300ml milk)");
    TEST_ASSERT(inventory_consume_for_item(&test_drink, 2) == true, "Consume ingredients for 2 cappuccinos");
    TEST_ASSERT(inventory_find_by_id(1)->current_stock == 110.0, "Remaining beans: 110g");
    TEST_ASSERT(inventory_find_by_id(2)->current_stock == 200.0, "Remaining milk: 200ml");

    // Attempt preparing 10 drinks (needs 200g beans, only 110g available)
    TEST_ASSERT(inventory_can_prepare_item(&test_drink, 10) == false, "Cannot prepare 10 cappuccinos (insufficient stock)");
}

static void test_order_and_pricing(void) {
    printf("\n" ANSI_BOLD "Testing Order Creation and Tax Calculations:" ANSI_RESET "\n");
    Order order = order_create(101, "cashier1", "555-9999");
    TEST_ASSERT(order.item_count == 0, "Initial cart is empty");

    MenuItem item_a = {1, "Espresso", CAT_HOT_COFFEE, 3.00, "Espresso", {{0, 0}}, 0, true};
    MenuItem item_b = {2, "Croissant", CAT_PASTRY, 4.00, "Croissant", {{0, 0}}, 0, true};

    order_add_item(&order, &item_a, 2); // 2 * 3.00 = 6.00
    order_add_item(&order, &item_b, 1); // 1 * 4.00 = 4.00
    TEST_ASSERT(order.item_count == 2, "Cart contains 2 distinct items");

    // Test merging duplicate item
    order_add_item(&order, &item_a, 1); // Now 3 * 3.00 = 9.00
    TEST_ASSERT(order.item_count == 2, "Cart still has 2 distinct items after merge");
    TEST_ASSERT(order.items[0].quantity == 3, "Espresso quantity merged to 3");

    double subtotal = order_calculate_subtotal(&order);
    TEST_ASSERT(fabs(subtotal - 13.00) < 0.001, "Subtotal is exactly $13.00");

    // Apply $3 discount with 8.25% sales tax:
    // Taxable = 10.00, Tax = 0.825, Total = 10.825
    order_calculate_totals(&order, 3.00);
    TEST_ASSERT(fabs(order.discount_amount - 3.00) < 0.001, "Discount applied is $3.00");
    TEST_ASSERT(fabs(order.tax_amount - 0.825) < 0.001, "Tax calculated at 8.25%");
    TEST_ASSERT(fabs(order.total_amount - 10.825) < 0.001, "Final total is $10.825");
}

static void test_loyalty_and_coupons(void) {
    printf("\n" ANSI_BOLD "Testing Customer Loyalty and Coupon Engine:" ANSI_RESET "\n");
    loyalty_init();
    coupon_init();

    TEST_ASSERT(loyalty_register_customer("555-1234", "Charlie") == true, "Register customer");
    Customer *cust = loyalty_find_customer("555-1234");
    TEST_ASSERT(cust != NULL && cust->points == 10, "Welcome bonus awarded (10 points)");
    TEST_ASSERT(cust->tier == TIER_BRONZE, "Initial tier is Bronze");

    loyalty_award_points(cust, 120.0);
    TEST_ASSERT(cust->points == 130, "Points increased to 130 after $120 spend");
    TEST_ASSERT(cust->tier == TIER_SILVER, "Tier upgraded to Silver (5% discount)");

    loyalty_award_points(cust, 150.0);
    TEST_ASSERT(cust->points == 280, "Points increased to 280");
    TEST_ASSERT(cust->tier == TIER_GOLD, "Tier upgraded to Gold (10% discount)");

    Coupon cp = {"TEST15", 15.0, 20.0, true};
    TEST_ASSERT(coupon_add(&cp) == true, "Register 15% coupon with $20 min spend");

    char msg[128];
    double disc1 = coupon_validate_and_apply("TEST15", 10.00, msg, sizeof(msg));
    TEST_ASSERT(disc1 == 0.0, "Coupon rejected when below min spend ($10 < $20)");

    double disc2 = coupon_validate_and_apply("TEST15", 40.00, msg, sizeof(msg));
    TEST_ASSERT(fabs(disc2 - 6.00) < 0.01, "15% discount applied correctly ($6.00 on $40)");
}

static void test_payment_processing(void) {
    printf("\n" ANSI_BOLD "Testing Payment Processing and Checkout:" ANSI_RESET "\n");
    Order order = order_create(202, "barista1", "555-0000");
    MenuItem item = {1, "Drip Coffee", CAT_HOT_COFFEE, 5.00, "Coffee", {{0, 0}}, 0, true};
    order_add_item(&order, &item, 2);
    order_calculate_totals(&order, 0.0); // 10.00 + 0.825 = 10.825

    double change = 0.0;
    TEST_ASSERT(payment_process_cash(&order, 5.00, &change) == false, "Reject insufficient cash tender");
    TEST_ASSERT(payment_process_cash(&order, 20.00, &change) == true, "Accept valid cash tender ($20.00)");
    TEST_ASSERT(fabs(change - (20.00 - order.total_amount)) < 0.01, "Correct change calculated");
    TEST_ASSERT(order.status == ORDER_COMPLETED, "Order marked as completed");
}

static void test_auth_and_roles(void) {
    printf("\n" ANSI_BOLD "Testing Authentication and Role-Based Access:" ANSI_RESET "\n");
    auth_init();

    TEST_ASSERT(auth_login("admin", "wrongpass") == NULL, "Reject incorrect password");
    const User *admin_user = auth_login("admin", "admin123");
    TEST_ASSERT(admin_user != NULL && admin_user->role == ROLE_ADMIN, "Admin login successful");
    TEST_ASSERT(auth_is_admin() == true, "Role recognized as Admin");

    auth_logout();
    TEST_ASSERT(auth_get_current_user() == NULL, "Logout clears session");

    const User *barista_user = auth_login("barista", "coffee123");
    TEST_ASSERT(barista_user != NULL && barista_user->role == ROLE_CASHIER, "Cashier login successful");
    TEST_ASSERT(auth_is_admin() == false, "Cashier cannot access Admin privileges");
}

int main(void) {
    printf(ANSI_BOLD ANSI_CYAN "=======================================================\n" ANSI_RESET);
    printf(ANSI_BOLD ANSI_YELLOW "       COFFEE SHOP SYSTEM - AUTOMATED TEST SUITE       \n" ANSI_RESET);
    printf(ANSI_BOLD ANSI_CYAN "=======================================================\n" ANSI_RESET);

    test_menu_operations();
    test_inventory_and_recipes();
    test_order_and_pricing();
    test_loyalty_and_coupons();
    test_payment_processing();
    test_auth_and_roles();

    printf("\n" ANSI_BOLD ANSI_CYAN "=======================================================\n" ANSI_RESET);
    printf("Test Results: " ANSI_BOLD ANSI_GREEN "%d Passed" ANSI_RESET ", %s%d Failed" ANSI_RESET " (Total: %d)\n",
           s_tests_passed,
           (s_tests_run - s_tests_passed == 0) ? ANSI_GREEN : ANSI_RED,
           s_tests_run - s_tests_passed,
           s_tests_run);
    printf(ANSI_BOLD ANSI_CYAN "=======================================================\n\n" ANSI_RESET);

    return (s_tests_run == s_tests_passed) ? 0 : 1;
}
