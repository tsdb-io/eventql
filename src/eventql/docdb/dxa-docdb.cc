/**
 * Copyright (c) 2015 - The CM Authors <legal@clickmatcher.com>
 *   All Rights Reserved.
 *
 * This file is CONFIDENTIAL -- Distribution or duplication of this material or
 * the information contained herein is strictly forbidden unless prior written
 * permission is obtained.
 */
#include "eventql/util/stdtypes.h"
#include "eventql/util/application.h"
#include "eventql/util/logging.h"
#include "eventql/util/cli/flagparser.h"
#include "eventql/util/io/fileutil.h"
#include "eventql/util/http/httprouter.h"
#include "eventql/util/http/httpserver.h"
#include "eventql/util/thread/eventloop.h"
#include "eventql/util/thread/threadpool.h"
#include "eventql/docdb/DocumentDB.h"
#include "eventql/docdb/DocumentDBServlet.h"
#include "eventql/AnalyticsAuth.h"

using namespace stx;
using namespace zbase;

stx::thread::EventLoop ev;

int main(int argc, const char** argv) {
  stx::Application::init();
  stx::Application::logToStderr();

  stx::cli::FlagParser flags;

  flags.defineFlag(
      "http_port",
      stx::cli::FlagParser::T_INTEGER,
      false,
      NULL,
      "7005",
      "Start the public http server on this port",
      "<port>");

  flags.defineFlag(
      "datadir",
      cli::FlagParser::T_STRING,
      true,
      NULL,
      NULL,
      "datadir path",
      "<path>");

  flags.defineFlag(
      "loglevel",
      stx::cli::FlagParser::T_STRING,
      false,
      NULL,
      "INFO",
      "loglevel",
      "<level>");

  flags.parseArgv(argc, argv);

  Logger::get()->setMinimumLogLevel(
      strToLogLevel(flags.getString("loglevel")));

  /* thread pools */
  stx::thread::ThreadPool tpool;

  /* http */
  stx::http::HTTPRouter http_router;
  stx::http::HTTPServer http_server(&http_router, &ev);
  http_server.listen(flags.getInt("http_port"));

  ///* auth */
  //AnalyticsAuth auth;

  ///* DocumentDB */
  //DocumentDB docdb(flags.getString("datadir"));
  //DocumentDBServlet docdb_servlet(&docdb, &auth);

  //http_router.addRouteByPrefixMatch(
  //    "/",
  //    &docdb_servlet,
  //    &tpool);

  ev.run();
  stx::logInfo("dxa-docdb", "Exiting...");

  exit(0);
}

