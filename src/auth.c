#include "auth.h"
#include <string.h>

static User s_users[MAX_USERS];
static int s_user_count = 0;
static const User *s_current_user = NULL;

void auth_init(void) {
    s_user_count = 0;
    s_current_user = NULL;
    memset(s_users, 0, sizeof(s_users));

    // Default seeded staff accounts
    auth_add_user("admin", "admin123", "Store Manager", ROLE_ADMIN);
    auth_add_user("barista", "coffee123", "Staff Cashier", ROLE_CASHIER);
}

bool auth_add_user(const char *username, const char *password, const char *display_name, UserRole role) {
    if (!username || !password || s_user_count >= MAX_USERS) {
        return false;
    }
    for (int i = 0; i < s_user_count; i++) {
        if (strcmp(s_users[i].username, username) == 0) {
            return false;
        }
    }
    User *u = &s_users[s_user_count];
    strncpy(u->username, username, MAX_NAME_LEN - 1);
    strncpy(u->password, password, MAX_NAME_LEN - 1);
    strncpy(u->display_name, display_name ? display_name : username, MAX_NAME_LEN - 1);
    u->role = role;
    u->active = true;

    s_user_count++;
    return true;
}

const User *auth_login(const char *username, const char *password) {
    if (!username || !password) return NULL;
    for (int i = 0; i < s_user_count; i++) {
        if (s_users[i].active && 
            strcmp(s_users[i].username, username) == 0 &&
            strcmp(s_users[i].password, password) == 0) {
            s_current_user = &s_users[i];
            return s_current_user;
        }
    }
    return NULL;
}

const User *auth_get_current_user(void) {
    return s_current_user;
}

void auth_logout(void) {
    s_current_user = NULL;
}

bool auth_is_admin(void) {
    return (s_current_user != NULL && s_current_user->role == ROLE_ADMIN);
}
