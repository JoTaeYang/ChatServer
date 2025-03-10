/*
 * Copyright (c) 2009, 2022, Oracle and/or its affiliates. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License, version 2.0, as
 * published by the Free Software Foundation.
 *
 * This program is also distributed with certain software (including
 * but not limited to OpenSSL) that is licensed under separate terms,
 * as designated in a particular file or component or in included license
 * documentation.  The authors of MySQL hereby grant you an
 * additional permission to link the program and your derivative works
 * with the separately licensed software that they have included with
 * MySQL.
 *
 * Without limiting anything contained in the foregoing, this file,
 * which is part of MySQL Connector/C++, is also subject to the
 * Universal FOSS Exception, version 1.0, a copy of which can be found at
 * http://oss.oracle.com/licenses/universal-foss-exception.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License, version 2.0, for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin St, Fifth Floor, Boston, MA 02110-1301  USA
 */



#ifndef _MYSQL_CONNECTION_PROXY_H_
#define _MYSQL_CONNECTION_PROXY_H_

#include "native_connection_wrapper.h"

#include <cppconn/sqlstring.h>
#include <memory>
#include <mutex>
#include <shared_mutex>

namespace sql
{
namespace mysql
{
namespace NativeAPI
{
class IMySQLCAPI;


inline const char * nullIfEmpty(const ::sql::SQLString & str)
{
  return str.length() > 0 ? str.c_str() : NULL;
}

extern std::shared_mutex plugins_cache_mutex;

class MySQL_NativeConnectionWrapper : public NativeConnectionWrapper
{
  /* api should be declared before mysql here */
  std::shared_ptr<IMySQLCAPI> api;
  bool reconnect = false;
  ::sql::SQLString m_host;
  ::sql::SQLString m_user;
  ::sql::SQLString m_passwd;
  ::sql::SQLString m_db;
  unsigned int     m_port;
  ::sql::SQLString m_socket_or_pipe;
  unsigned long    m_client_flag;
  bool             m_dns_srv = false;

#if (MYCPPCONN_STATIC_MYSQL_VERSION_ID > 80004)
struct MYSQL* mysql;
#else
struct st_mysql* mysql;
#endif

  ::sql::SQLString				serverInfo;


  MySQL_NativeConnectionWrapper(){}

public:
 MySQL_NativeConnectionWrapper(std::shared_ptr<IMySQLCAPI> _api);

 virtual ~MySQL_NativeConnectionWrapper();

 uint64_t affected_rows() override;

 bool autocommit(bool) override;

 bool connect(const ::sql::SQLString &host, const ::sql::SQLString &user,
              const ::sql::SQLString &passwd, const ::sql::SQLString &db,
              unsigned int port, const ::sql::SQLString &socket_or_pipe,
              unsigned long client_flag) override;

 bool connect_dns_srv(const ::sql::SQLString &host,
                      const ::sql::SQLString &user,
                      const ::sql::SQLString &passwd,
                      const ::sql::SQLString &db,
                      unsigned long client_flag) override;

 bool commit() override;

 void debug(const ::sql::SQLString &) override;

 unsigned int errNo() override;

 ::sql::SQLString error() override;

 ::sql::SQLString escapeString(const ::sql::SQLString &) override;

 unsigned int field_count() override;

 unsigned long get_client_version() override;

 const ::sql::SQLString &get_server_info() override;

 unsigned long get_server_version() override;

 void get_character_set_info(void *cs) override;

 bool more_results() override;

 int next_result() override;

 int options(::sql::mysql::MySQL_Connection_Options, const void *) override;
 int options(::sql::mysql::MySQL_Connection_Options,
             const ::sql::SQLString &) override;
 int options(::sql::mysql::MySQL_Connection_Options, const bool &) override;
 int options(::sql::mysql::MySQL_Connection_Options, const int &) override;
 int options(::sql::mysql::MySQL_Connection_Options, const ::sql::SQLString &,
             const ::sql::SQLString &) override;
 int options(::sql::mysql::MySQL_Connection_Options, const int &,
             const ::sql::SQLString &) override;

 int get_option(::sql::mysql::MySQL_Connection_Options, const void *) override;
 int get_option(::sql::mysql::MySQL_Connection_Options,
                const ::sql::SQLString &) override;
 int get_option(::sql::mysql::MySQL_Connection_Options, const bool &) override;
 int get_option(::sql::mysql::MySQL_Connection_Options, const int &) override;

 int plugin_option(
    int plugin_type, const ::sql::SQLString &plugin_name,
    const ::sql::SQLString &option, const void *, bool
  ) override;

 int plugin_option(
    int plugin_type, const ::sql::SQLString &plugin_name,
    const ::sql::SQLString &option, const ::sql::SQLString &value, bool
  ) override;

 int get_plugin_option(int plugin_type, const ::sql::SQLString &plugin_name,
                       const ::sql::SQLString &option,
                       const ::sql::SQLString &value) override;

 bool has_query_attributes() override;

 bool query_attr(unsigned number, const char **names, MYSQL_BIND *) override;

 int query(const ::sql::SQLString &) override;

 int ping() override;

 /* int real_query(const ::sql::SQLString &, uint64_t);*/

 bool rollback() override;

 ::sql::SQLString sqlstate() override;

 ::sql::SQLString info() override;

 NativeResultsetWrapper *store_result() override;

 int use_protocol(Protocol_Type protocol) override;

 NativeResultsetWrapper *use_result() override;

 NativeStatementWrapper &stmt_init() override;

 unsigned int warning_count() override;

  void lock_plugin(bool lock_or_unlock) override
  {
    lock_plugin_impl(lock_or_unlock ? lock_type::GUARD : lock_type::UNGUARD);
  }

  void lock_plugin_exclusive() override
  {
    lock_plugin_impl(lock_type::EXCLUSIVE);
  }

  /*
    Implementation of plugin locking.

    There are the following lock request that can be made with
    lock_plugin_impl() method:

    - SHARED lock can be taken if there are no EXCLUSIVE locks present (when
      requested it watis until this is the case), existence of other SHARED
      (or GUARD) locks does not prevent SHARED lock to be taken;

    - GUARD lock is like a SHARED lock but it prevents locks to be released
      by UNLOCK request (see below).

    - EXCLUSIVE lock can be taken only when no other connection holds any type
      of lock (when requested it waits until this is the case). EXCLUSIVE lock
      can be requested while holding SHARED or GUARD lock.

    The UNLOCK request is used to remove SHARED or EXCLUSIVE locks provided
    that no GUARD lock was taken. If GUARD lock was taken then an UNLOCK
    request has no effect -- any locks taken remain in place. Only UNGUARD
    request can be used to remove locks after GUARD request -- either
    the original GUARD lock or an EXCLUSIVE lock to which it was upgraded.

    If connection already holds a plugin lock then another request to get
    a lock has the following effect depending on the requested lock type:

    - EXCLUSIVE - upgrade shared lock to exclusive one
    - GUARD     - keep existing locks until UNGUARD request is made
    - SHARED    - no effect
  */

  enum class lock_type {GUARD, SHARED, EXCLUSIVE, UNLOCK, UNGUARD};

  void lock_plugin_impl(lock_type);

  // These are used by lock_plugin_impl()

  std::shared_lock<std::shared_mutex> plugins_sh_lock{plugins_cache_mutex, std::defer_lock};
  std::unique_lock<std::shared_mutex> plugins_ex_lock{plugins_cache_mutex, std::defer_lock};
  bool plugin_guard = false;

  /*
    This class handles locking of plugin when plugin options are set
    (in plugin_option() methods) as well as implements access to the plugin
    cache.
  */

  struct PluginGuard;
};

} /* namespace NativeAPI */
} /* namespace mysql */
} /* namespace sql */
#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
