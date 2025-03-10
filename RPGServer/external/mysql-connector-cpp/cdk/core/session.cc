/*
 * Copyright (c) 2015, 2024, Oracle and/or its affiliates.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License, version 2.0, as
 * published by the Free Software Foundation.
 *
 * This program is designed to work with certain software (including
 * but not limited to OpenSSL) that is licensed under separate terms, as
 * designated in a particular file or component or in included license
 * documentation. The authors of MySQL hereby grant you an additional
 * permission to link the program and your derivative works with the
 * separately licensed software that they have either included with
 * the program or referenced in the documentation.
 *
 * Without limiting anything contained in the foregoing, this file,
 * which is part of Connector/C++, is also subject to the
 * Universal FOSS Exception, version 1.0, a copy of which can be found at
 * https://oss.oracle.com/licenses/universal-foss-exception.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License, version 2.0, for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */


#include <mysql/cdk/session.h>
#include <mysql/cdk/mysqlx/session.h>


namespace cdk {

using std::unique_ptr;

/*
  A class that creates a session from given data source.

  Instances of this class are callable objects which can be used as visitors
  for ds::Multi_source implementing in this case the failover logic.
*/

struct Session_builder
{
  using TLS   = cdk::connection::TLS;
  using TCPIP = cdk::connection::TCPIP;
  using Socket_base = foundation::connection::Socket_base;
  using ep_filter_t = std::function<bool(size_t, option_t)>;

  struct ReportStatus
  {
    // False if not able to connect, true if all is good.
    option_t m_status = false;
    Session_builder::ep_filter_t m_filter;
    size_t m_id;

    ReportStatus(Session_builder::ep_filter_t filter, size_t id)
      : m_filter(filter)
      , m_id(id)
    {}

    /*
      Note: The only reason for ReportStatus object is to use RAII to invoke
      m_filter() on destruction to report session creation status. So this
      is part of logic of methods that use such object and as such is allowed
      to throw errors from the dtor.
    */

    ~ReportStatus()
    {
      if(m_filter)
        m_filter(m_id, m_status);
    }

  };

  unique_ptr<cdk::api::Connection> m_conn;
  mysqlx::Session      *m_sess = NULL;
  const mysqlx::string *m_database = NULL;
  bool m_throw_errors = false;
  scoped_ptr<Error>     m_error;
  unsigned              m_attempts = 0;
  size_t                m_id = 0;
  ep_filter_t           m_filter;


  Session_builder(bool throw_errors = false, ep_filter_t filter = nullptr)
    : m_throw_errors(throw_errors)
    , m_filter(filter)
  {}

  /*
    Construct a session for a given data source, if possible.

    1. If session could be constructed returns true. In this case m_sess points
       at the newly created session and m_conn points at the connection object
       used for that session. Someone must take ownership of these objects.

    2. If a network error was detected while creating session, either throws
       error if m_throw_errors is true or returns false. In the latter case,
       if used as a visitor over list of data sources, it will be called again
       to try another data source.

    3. If a bail-out error was detected, throws that error.
  */

  bool operator() (size_t, const ds::TCPIP &ds,
                   const ds::TCPIP::Options &options);

#ifndef WIN32
  bool operator() (size_t, const ds::Unix_socket&ds,
                   const ds::Unix_socket::Options &options);
#endif
  bool operator() (size_t, const ds::TCPIP_old &ds,
                   const ds::TCPIP_old::Options &options);

  /*
    Make a connection attempt using the given connection object. Returns true if
    connection was established, otherwise returns false or throws error
    depending on the value of m_throw_errors;
  */

  template <class Conn>
  bool connect(Conn&);

#ifdef WITH_SSL

  /*
    Given plain connection `conn` (which should be a dynamically allocated
    object) create a TLS connection over it, if possible.

    If a TLS object is successfully created, it takes the ownership of the
    plain connection object passed to this call. The caller of this method
    should take responsibility for the returned TLS object in that case.

    If this method throws error, then it deletes the plain connection object
    first, so that the caller does not need to bother about it. But if this
    method returns NULL, then the ownership of the plain connection
    object stays with the caller who is responsible of deleting it.

    Returns NULL if TLS connections are not supported. Throws errors in case
    of other problems.
  */

  TLS* tls_connect(Socket_base *conn, const TLS::Options &opt);
#endif
};


template <class Conn>
bool Session_builder::connect(Conn &connection)
{
  m_attempts++;

  try
  {
    connection.connect();
    return true;
  }
  catch (...)
  {
    // Use rethrow_error() to wrap arbitrary exception in cdk::Error.

    try {
      rethrow_error();
    }
    catch (Error &err)
    {
      error_code code = err.code();

      if (m_throw_errors ||
        code == cdkerrc::auth_failure ||
        code == cdkerrc::protobuf_error ||
        code == cdkerrc::tls_error)
        throw;

      m_error.reset(err.clone());
    }

    return false;
  }
}



bool
Session_builder::operator() (
  size_t id,
  const ds::TCPIP &ds,
  const ds::TCPIP::Options &options
  )
{
  using foundation::connection::TCPIP;
  using foundation::connection::Socket_base;

  ReportStatus report_status(m_filter, id);

  unique_ptr<TCPIP> connection(new TCPIP(ds.host(), ds.port(),
                               options));

  if (!connect(*connection))
    return false;  // continue to next host if available

#ifdef WITH_SSL

  /*
    Note: We must be careful to release the unique_ptr<> if tls_connect()
    throws error or returns a TLS object because in that case the plain
    connection object is either deleted by tls_connect() or its ownership
    is passed to the returned TLS object. However, if tls_connect() returns
    NULL, we are still responsible for the plain connection object.
  */

  TLS * tls_conn = nullptr;
  try {
    tls_conn = tls_connect(connection.get(), options.get_tls());
  }
  catch (...)
  {
    connection.release();
    throw;
  }

  if (tls_conn)
  {
    // Now tls_conn owns the plain connection.
    connection.release();

    /*
      Note: Order is important! We need to take ownership of tls_conn before
      calling mysqlx::Session() ctor which might throw errors.
    */

    m_conn.reset(tls_conn);
    m_sess = new mysqlx::Session(*tls_conn, options);
  }
  else
#endif
  {
    /*
      Note: Order is important! We must construct mysqlx::Session using
      connection object of type TCPIP and we must do it before
      connection.release() is called. If session ctor fails, the unique_ptr<>
      will still take care of deleting the connection object.
    */

    m_sess = new mysqlx::Session(*connection, options);
    m_conn.reset(connection.release());
  }

  m_database = options.database();
  m_id = id;
  report_status.m_status = true;
  return true;
}

#ifndef WIN32
bool
Session_builder::operator() (
  size_t id,
  const ds::Unix_socket &ds,
  const ds::Unix_socket::Options &options
  )
{
  using foundation::connection::Unix_socket;
  using foundation::connection::Socket_base;

  ReportStatus report_status(m_filter, id);

  unique_ptr<Unix_socket> connection(new Unix_socket(ds.path(), options));

  if (!connect(*connection))
    return false;  // continue to next host if available

  m_sess = new mysqlx::Session(*connection, options);
  m_conn.reset(connection.release());

  m_database = options.database();
  m_id = id;
  report_status.m_status = true;
  return true;
}

#endif //#ifndef WIN32


bool
Session_builder::operator() (
  size_t,
  const ds::TCPIP_old&,
  const ds::TCPIP_old::Options&
)
{
  throw Error(cdkerrc::generic_error, "Not supported");
  //return false;
}

#ifdef WITH_SSL

Session_builder::TLS*
Session_builder::tls_connect(Socket_base *connection, const TLS::Options &options)
{
  /*
    Note: In case of throwing any errors, the connection object should be
    deleted - this is taken care of by the unique_ptr<>. However, if returning
    NULL, the ownership stays with the caller of this method and hence we must
    release the unique_ptr<>.
  */

  unique_ptr<Socket_base> conn_ptr(connection);

  if (options.ssl_mode() >= TLS::Options::SSL_MODE::VERIFY_CA &&
      options.get_ca().empty())
    throw Error(cdkerrc::generic_error,
                "Missing ssl-ca option to verify CA");

  if (options.ssl_mode() == TLS::Options::SSL_MODE::DISABLED)
  {
    conn_ptr.release();
    return NULL;
  }

  assert(connection);

  // Negotiate TLS capabilities.

  cdk::protocol::mysqlx::Protocol proto(*connection);

  struct : cdk::protocol::mysqlx::api::Any::Document
  {
    void process(Processor &prc) const
    {
      prc.doc_begin();
      cdk::safe_prc(prc)->key_val("tls")->scalar()->yesno(true);
      prc.doc_end();
    }
  } tls_caps;

  proto.snd_CapabilitiesSet(tls_caps).wait();

  struct : cdk::protocol::mysqlx::Reply_processor
  {
    bool m_tls;
    bool m_fallback;  // fallback to plain connection if TLS not available?

    void error(unsigned int code, short int severity,
      cdk::protocol::mysqlx::sql_state_t sql_state, const string &msg)
    {
      sql_state_t expected_state("HY000");

      if (code == 5001 &&
          severity == 2 &&
          expected_state == sql_state && m_fallback)
      {
        m_tls = false;
      }
      else
      {
        throw Error(static_cast<int>(code), msg);
      }
    }
  } prc;

  prc.m_tls = true;
  prc.m_fallback = (TLS::Options::SSL_MODE::PREFERRED == options.ssl_mode());

  proto.rcv_Reply(prc).wait();

  if (!prc.m_tls)
  {
    conn_ptr.release();
    return NULL;
  }

  /*
    Capabilites OK, create TLS connection now.

    Note: The TLS object created here takes ownership of the plain connection
    object passed to this method. The TLS object itself is protected by a
    unique_ptr<> for the case that connection attempt below throws an error
    - in that case it will be deleted before leaving this method.
  */

  unique_ptr<TLS> tls_conn(new TLS(conn_ptr.release(), options));

  // TODO: attempt fail-over if TLS-layer reports network error?
  tls_conn->connect();

  /*
    Finally we pass the ownership of the created TLS object to the caller
    of this method.
  */

  return tls_conn.release();
}

#endif


Session::Session(ds::TCPIP &ds, const ds::TCPIP::Options &options)
  : m_session(NULL)
  , m_connection(NULL)
{
  Session_builder sb(true);  // throw errors if detected

  sb(0, ds, options);

  assert(sb.m_sess);

  m_session = sb.m_sess;
  m_connection = sb.m_conn.release();
  m_id = sb.m_id;
}


struct ds::Multi_source::Access
{
  template <class Visitor>
  static void visit(Multi_source &ds, Visitor &visitor,
                    Multi_source::ep_filter_t ep_filter)
  { ds.visit(visitor, ep_filter); }
};


/*
  Create session to one of the data sources in the multi-source `ds`.
  The order in which data sources are tried is determined by `Multi_source`
  class (see `Multi_source::visit()` method). For each data source with
  identifier `id`, first a call to `ep_filter(id,UNKNOWN)` is made to determine
  if that data source should be filtered. If this is not the case and session
  could be successfully created, `ep_filter(id,YES)` is called. Otherwise
  `ep_filter(id,NO)` is called and next data source is tried.
*/

Session::Session(ds::Multi_source &ds, ep_filter_t ep_filter)
  : m_session(NULL)
  , m_connection(NULL)
{
  Session_builder sb(false, ep_filter);

  auto filter = [&ep_filter](size_t id) -> bool {
    if(ep_filter)
      return ep_filter(id, option_t());
    return false;
  };

  ds::Multi_source::Access::visit(ds, sb, filter);

  if (!sb.m_sess)
  {
    if (1 == sb.m_attempts && sb.m_error)
      sb.m_error->rethrow();
    else
      throw_error(
        1 == sb.m_attempts ?
        "Could not connect to the given data source" :
        "Could not connect to any of the given data sources"
      );
  }

  m_session = sb.m_sess;
  m_database = sb.m_database;
  m_connection = sb.m_conn.release();
  m_id = sb.m_id;
}


#ifndef WIN32
Session::Session(ds::Unix_socket &ds, const ds::Unix_socket::Options &options)
  : m_session(NULL)
  , m_connection(NULL)
{
  Session_builder sb(true);  // throw errors if detected

  sb(0, ds, options);

  assert(sb.m_sess);

  m_session = sb.m_sess;
  m_connection = sb.m_conn.release();
  m_id = sb.m_id;
}
#endif //#ifndef WIN32


Session::~Session() NOEXCEPT
{
  delete m_session;
  delete m_connection;
}

option_t Session::has_prepared_statements() {
  return m_session->has_protocol_field(mysqlx::Protocol_fields::PREPARED_STATEMENTS);
}

/*
  FIXME: The set_has_prepared_statement() method is used by upper layers to
  temporarily disable PS when server limits are hit. With current implementation
  it tampers with the value of PREPARED_STATEMENTS protocol field flag, which
  should always store the result of the expectation check once it was done.
*/
void Session::set_has_prepared_statements(bool x) {
  m_session->set_protocol_field(mysqlx::Protocol_fields::PREPARED_STATEMENTS, x);
}



} //cdk
