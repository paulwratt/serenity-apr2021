/*
 * Copyright (c) 2020, Peter Elliott <pelliott@ualberta.ca>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <AK/Result.h>
#include <AK/String.h>
#include <AK/Types.h>
#include <AK/Vector.h>
#include <pwd.h>
#include <shadow.h>
#include <sys/types.h>

namespace Core {

class Account {
public:
    static Result<Account, String> from_name(const char* username);
    static Result<Account, String> from_uid(uid_t uid);

    bool authenticate(const char* password) const;
    bool login() const;

    String username() const { return m_username; }
    String password_hash() const { return m_password_hash; }

    // Setters only affect in-memory copy of password.
    // You must call sync to apply changes.
    void set_password(const char* password);
    void set_password_enabled(bool enabled);
    void delete_password();

    // A null password means that this account was missing from /etc/shadow.
    // It's considered to have a password in that case, and authentication will always fail.
    bool has_password() const { return !m_password_hash.is_empty() || m_password_hash.is_null(); }

    uid_t uid() const { return m_uid; }
    gid_t gid() const { return m_gid; }
    const String& gecos() const { return m_gecos; }
    const String& home_directory() const { return m_home_directory; }
    const String& shell() const { return m_shell; }
    const Vector<gid_t>& extra_gids() const { return m_extra_gids; }

    bool sync();

private:
    static Result<Account, String> from_passwd(const passwd&, const spwd&);

    Account(const passwd& pwd, const spwd& spwd, Vector<gid_t> extra_gids);

    String generate_passwd_file() const;
    String generate_shadow_file() const;

    String m_username;

    String m_password_hash;
    uid_t m_uid { 0 };
    gid_t m_gid { 0 };
    String m_gecos;
    String m_home_directory;
    String m_shell;
    Vector<gid_t> m_extra_gids;
};

}
