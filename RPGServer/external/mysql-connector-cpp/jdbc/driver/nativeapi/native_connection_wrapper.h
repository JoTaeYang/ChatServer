/*
 * Copyright (c) 2009, 2018, Oracle and/or its affiliates. All rights reserved.
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



#ifndef _NATIVE_CONNECTION_WRAPPER_H_
#define _NATIVE_CONNECTION_WRAPPER_H_

#include <cppconn/config.h>
#ifdef HAVE_STDINT_H
#include <stdint.h>
#endif
#include "../mysql_util.h"

#include "../mysql_connection_options.h"

#if (MYCPPCONN_STATIC_MYSQL_VERSION_ID >= 80004)
struct MYSQL_BIND;
#else
struct st_mysql_bind;
#define MYSQL_BIND st_mysql_bind
#endif

namespace sql
{
class SQLString;

namespace mysql
{

namespace NativeAPI
{
class NativeResultsetWrapper;
class NativeStatementWrapper;

enum Protocol_Type
{
  PROTOCOL_TCP,
  PROTOCOL_SOCKET,
  PROTOCOL_PIPE,
  /* Total number of supported protocol types */
  PROTOCOL_COUNT
};


class NativeConnectionWrapper : public util::nocopy
{
public:
  virtual ~NativeConnectionWrapper() {}


  virtual uint64_t affected_rows() = 0;

  virtual bool autocommit(bool) = 0;

  virtual bool connect(const ::sql::SQLString & host,
            const ::sql::SQLString & user,
            const ::sql::SQLString & passwd,
            const ::sql::SQLString & db,
            unsigned int			 port,
            const ::sql::SQLString & socket_or_pipe,
            unsigned long			client_flag) = 0;

  virtual bool connect_dns_srv(const ::sql::SQLString & host,
            const ::sql::SQLString & user,
            const ::sql::SQLString & passwd,
            const ::sql::SQLString & db,
            unsigned long			client_flag) = 0;

  virtual bool commit() = 0;

  virtual void debug(const ::sql::SQLString &) = 0;

  virtual unsigned int errNo() = 0;

  virtual ::sql::SQLString error() = 0;

  virtual ::sql::SQLString escapeString(const ::sql::SQLString &) = 0;

  virtual unsigned int field_count() = 0;

  virtual unsigned long get_client_version() = 0;

  virtual const ::sql::SQLString & get_server_info() = 0;

  virtual unsigned long get_server_version() = 0;

  virtual void get_character_set_info(void *cs) = 0;

  virtual bool more_results() = 0;

  virtual int next_result() = 0;

  virtual int options(::sql::mysql::MySQL_Connection_Options, const void *) = 0;
  virtual int options(::sql::mysql::MySQL_Connection_Options,
            const ::sql::SQLString &) = 0;
  virtual int options(::sql::mysql::MySQL_Connection_Options,
            const bool &) = 0;
  virtual int options(::sql::mysql::MySQL_Connection_Options,
            const int &) = 0;
  virtual int options(::sql::mysql::MySQL_Connection_Options,
            const ::sql::SQLString &,
            const ::sql::SQLString &) = 0;
  virtual int options(::sql::mysql::MySQL_Connection_Options,
                      const int &,
                      const ::sql::SQLString &) = 0;

  virtual int get_option(::sql::mysql::MySQL_Connection_Options, const void *) = 0;
  virtual int get_option(::sql::mysql::MySQL_Connection_Options,
            const ::sql::SQLString &) = 0;
  virtual int get_option(::sql::mysql::MySQL_Connection_Options,
            const bool &) = 0;
  virtual int get_option(::sql::mysql::MySQL_Connection_Options,
            const int &) = 0;

  /*
    Note: The `default_value` flag informs whether the value to which option
    is set is its default value. This can be used to avoid unnecessary loading
    of the plugin.
  */

  virtual int plugin_option(
    int plugin_type,
    const ::sql::SQLString & plugin_name,
    const ::sql::SQLString & option,
    const void * value,
    bool default_value = false
  ) = 0;

  virtual int plugin_option(
    int plugin_type,
    const ::sql::SQLString & plugin_name,
    const ::sql::SQLString & option,
    const ::sql::SQLString & value,
    bool default_value = false
  ) = 0;

  virtual int get_plugin_option(
    int plugin_type,
    const ::sql::SQLString & plugin_name,
    const ::sql::SQLString & option,
    const ::sql::SQLString & value
  ) = 0;

  virtual bool has_query_attributes() = 0;

  virtual bool query_attr(unsigned nrAttr, const char** names ,::MYSQL_BIND* binds) = 0;

  virtual int query(const SQLString &) = 0;

  virtual int ping() = 0;

  /* virtual int real_query(const SQLString &, uint64_t) = 0;*/

  virtual bool rollback() = 0;

  virtual ::sql::SQLString sqlstate() = 0;

  virtual ::sql::SQLString info() = 0;

  virtual NativeResultsetWrapper * store_result() = 0;

  virtual int use_protocol(Protocol_Type) = 0;

  virtual NativeResultsetWrapper * use_result() = 0;

  virtual NativeStatementWrapper & stmt_init() = 0;

  virtual unsigned int warning_count() = 0;

  /*
    This method is used to lock plugin options.

    After lock_plugin(true) call other connections that try to set plugin
    options using plugin_option() methods will wait until lock_plugin(false)
    is called on this connection.
  */

  virtual void lock_plugin(bool) = 0;

  /*
    This method is used to lock plugin options exclusively before making any
    plugin_option() call.

    Normally a thread takes an exclusive lock on plugin options only when it
    calls `plugin_option()` method to modify one. This might however lead to
    a race condition in a situation like this:

      lock_plugin(true);             // lock plugin options
      val = get_plugin_option(...);  // read some option <1>
      plugin_option(...);            // write other option <2>

    If in <2> we want to set option X based on the value `val` of another
    option Y that was read in step <1> there is no guarantee that at the time
    of setting option X option Y has still the same value `val` that we saw
    before. This is because while plugin_option() waits for exclusive access
    to the options another thread can get that exclusive access first and
    modify the value of Y. To avoid such race condition the code should grab
    an exclusive lock on plugin options up-front rather than during
    `plugin_option()` call:

      lock_plugin_exclusive();
      val = get_plugin_option(...);
      plugin_option(...);
  */

  virtual void lock_plugin_exclusive() = 0;
};

} /* namespace NativeAPI */
} /* namespace mysql */
} /* namespace sql */
#endif /* _NATIVE_CONNECTION_WRAPPER_H_ */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
