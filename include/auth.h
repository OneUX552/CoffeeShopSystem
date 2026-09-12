#ifndef AUTH_H
#define AUTH_H

#include "models.h"
#include <stdbool.h>

void auth_init(void);
const User *auth_login(const char *username, const char *password);
bool auth_add_user(const char *username, const char *password, const char *display_name, UserRole role);
const User *auth_get_current_user(void);
void auth_logout(void);
bool auth_is_admin(void);

#endif // AUTH_H
